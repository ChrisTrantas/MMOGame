#include "Game.h"
#include "ResourceManager.h"
#include "..\networking\NetworkManager.h"
#include "..\threading\Thread.h"

Game* Game::game = nullptr;
// define the common.h extern for wchart conversion
std::wstring_convert< std::codecvt<wchar_t, char, std::mbstate_t> > conv;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward the global callback params to our game's message handler
	return Game::game->ProcessMessage(hwnd, msg, wParam, lParam);
}

void CreateConsoleWindow(int numLines, int numColumns, int windowLines, int windowColumns)
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// Get the console and set the number of lines
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = numLines;
	coninfo.dwSize.X = numColumns;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = windowColumns;
	rect.Bottom = windowLines;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	FILE *stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent accidental console window close
	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

void Game::init(HINSTANCE hInstance)
{
	if (game == nullptr)
	{
		game = new Game(hInstance);
		CreateConsoleWindow(32, 120, 300, 120);
	}
}

Game::Game(HINSTANCE hAppInst)
{
	this->hAppInst = hAppInst;

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	__int64 perfFreq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
	perfCounterSeconds = 1.0 / (double)perfFreq;
}

Game::~Game()
{
	destructFunc();

	delete shadowVS;

	ResourceManager::manager->shutdown();
	freeAllGameObjects();

	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(swapChain);
	ReleaseMacro(depthStencilBuffer);

	ReleaseMacro(shadowSampler);
	ReleaseMacro(shadowMapDSV);
	ReleaseMacro(shadowMapSRV);
	ReleaseMacro(shadowRS);

	if (deviceContext)
		deviceContext->ClearState();

	ReleaseMacro(deviceContext);
	ReleaseMacro(device);

	game = nullptr;
}

int Game::start(void(*buildFunc)(), void(*destructFunc)())
{
	if (game == nullptr)
		return EXIT_FAILURE;

	this->destructFunc = destructFunc;

	if (!InitMainWindow())
	{
		delete game;
		return EXIT_FAILURE;
	}

	if (!InitDirect3D())
	{
		delete game;
		return EXIT_FAILURE;
	}

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	initEngine();

	buildFunc();

	NetworkManager::networkManager->Initialize(2);
	NetworkManager::networkManager->AssignTask([]() { NetworkManager::networkManager->startClient(); });

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		// Peek at the next message (and remove it from the queue)
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Handle this message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (!minimized) // No message to handle
		{
			if (Input::wasControlPressed("quit"))
				PostQuitMessage(0);

			// Update the timer for this frame
			UpdateTimer();

			// Standard game loop type stuff
			CalculateFrameStats();
			update(deltaTime, totalTime);

			draw();// (deltaTime, totalTime);
			Input::updateControlStates();
		}
	}

	delete game;
	
	return (int)msg.wParam;
}

bool Game::InitMainWindow()
{
	// We fill out a a big struct with the basic
	// information about the window we'd like
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;  // Can't be a member function!
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	// Register the window class struct, which must be done before
	// we attempt to create the window itself
	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Calculate the aspect ratio now that the width & height are set
	aspectRatio = (float)windowWidth / windowHeight;

	// We want the specified width and height to be the viewable client
	// area (not counting borders), so we need to adjust for borders
	RECT R = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int realWidth = R.right - R.left;
	int realHeight = R.bottom - R.top;

	// Actually create the window the user will eventually see
	hMainWnd = CreateWindow(
		L"D3DWndClassName",
		windowCaption.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		realWidth,
		realHeight,
		0,
		0,
		hAppInst,
		0);

	// Ensure the window was created properly
	if (!hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	// Finally show the window to the user
	ShowWindow(hMainWnd, SW_SHOW);
	return true;
}

bool Game::InitDirect3D()
{
	UINT createDeviceFlags = 0;

	// Do we want a debug device?
	// A debug device can give us more information when errors or
	// warnings occur, at the cost of a little performance
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Set up a swap chain description - 
	// The swap chain is used for double buffering (a.k.a. page flipping)
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hMainWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Create the device and swap chain with the following global function
	// This will also determine the supported feature level (version of DirectX)
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		0,
		driverType,
		0,
		createDeviceFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,		// We pass in the description
		&swapChain,			// And the SWAP CHAIN is created
		&device,			// As well as the DEVICE
		&featureLevel,		// The feature level is determined
		&deviceContext);	// And the CONTEXT is created

							// Was the device created properly?
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
		return false;
	}

	// There are several remaining steps before we can reasonably use DirectX.
	// These steps also need to happen each time the window is resized, 
	// so we simply call the OnResize method here.
	OnResize();
	return true;
}

void Game::OnResize()
{
	// Release any existing views, since we'll be destroying
	// the corresponding buffers.
	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(depthStencilBuffer);

	// Resize the swap chain to match the new window dimensions
	HR(swapChain->ResizeBuffers(
		1,
		windowWidth,
		windowHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0));

	// Recreate the render target view that points to the swap chain's buffer
	ID3D11Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(device->CreateRenderTargetView(backBuffer, 0, &renderTargetView));
	ReleaseMacro(backBuffer);

	// Set up the description of the texture to use for the depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = windowWidth;
	depthStencilDesc.Height = windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	// Create the depth/stencil buffer and corresponding view
	HR(device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));

	// Bind these views to the pipeline, so rendering properly 
	// uses the underlying textures
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Update the viewport to match the new window size and set it on the device
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);

	// Recalculate the aspect ratio, since it probably changed
	aspectRatio = (float)windowWidth / windowHeight;
}

void Game::UpdateTimer()
{
	// Grab the current time
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	currentTime = now;

	// Calculate delta time and clamp to zero if negative.
	//  - This could happen if the processor goes into a
	//    power save mode or the process itself gets moved
	//    to another processor core.
	deltaTime = (float)((currentTime - previousTime) * perfCounterSeconds);
	if (deltaTime < 0.0f)
		deltaTime = 0.0f;

	// Calculate total time
	totalTime = (float)((currentTime - startTime) * perfCounterSeconds);

	// Save the previous time
	previousTime = currentTime;
}

void Game::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = totalTime;

	frameCount++;

	// Compute averages over ONE SECOND.
	if ((totalTime - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount; // Count over one second
		float mspf = 1000.0f / fps;    // Milliseconds per frame

									   // Quick and dirty string manipulation for title bar text
		wostringstream outs;
		outs.precision(6);
		outs << windowCaption << L"    "
			<< L"Width: " << windowWidth << L"    "
			<< L"Height: " << windowHeight << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L"ms";

		// Include feature level
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1: outs << "    DX 11.1"; break;
		case D3D_FEATURE_LEVEL_11_0: outs << "    DX 11.0"; break;
		case D3D_FEATURE_LEVEL_10_1: outs << "    DX 10.1"; break;
		case D3D_FEATURE_LEVEL_10_0: outs << "    DX 10.0"; break;
		case D3D_FEATURE_LEVEL_9_3:  outs << "    DX 9.3";  break;
		case D3D_FEATURE_LEVEL_9_2:  outs << "    DX 9.2";  break;
		case D3D_FEATURE_LEVEL_9_1:  outs << "    DX 9.1";  break;
		default:                     outs << "    DX ???";  break;
		}

		SetWindowText(hMainWnd, outs.str().c_str());

		// Reset frame count, and adjust time elapsed
		// to wait another second
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

LRESULT Game::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DWORD mouseButton = 0;

	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.
		// We're using this to keep track of focus, if that's useful to you.
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			hasFocus = false;
		}
		else
		{
			hasFocus = true;
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		if (device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				hasFocus = false;
				minimized = true;
				maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				hasFocus = true;
				minimized = false;
				maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (minimized)
				{
					hasFocus = true;
					minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (maximized)
				{
					hasFocus = true;
					maximized = false;
					OnResize();
				}
				else if (resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		resizing = true;
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		resizing = false;
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

		// Messages that correspond to mouse button being pressed while the cursor
		// is currently over our window
	case WM_LBUTTONDOWN:
		if (mouseButton == 0) mouseButton = VK_LBUTTON;
	case WM_MBUTTONDOWN:
		if (mouseButton == 0) mouseButton = VK_MBUTTON;
	case WM_RBUTTONDOWN:
		if (mouseButton == 0) mouseButton = VK_RBUTTON;
		Input::OnMouseDown(mouseButton, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hMainWnd);
		return 0;

		// Messages that correspond to mouse button being released while the cursor
		// is currently over our window
	case WM_LBUTTONUP:
		if (mouseButton == 0) mouseButton = VK_LBUTTON;
	case WM_MBUTTONUP:
		if (mouseButton == 0) mouseButton = VK_MBUTTON;
	case WM_RBUTTONUP:
		if (mouseButton == 0) mouseButton = VK_RBUTTON;
		Input::OnMouseUp(mouseButton, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		// Message that occurs while the mouse moves over the window or while
		// we're currently capturing it
	case WM_MOUSEMOVE:
		Input::OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_KEYDOWN:
		Input::OnKeyDown(wParam);
		return 0;

	case WM_KEYUP:
		Input::OnKeyUp(wParam);
		return 0;
	}

	// Some other message was sent, so call the default window procedure for it
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Game::initEngine()
{
	srand((unsigned int)time(NULL));
	Input::bindToControl("quit", VK_ESCAPE);
	ResourceManager::init();

	// Create texture for shadow map
	D3D11_TEXTURE2D_DESC shadowDesc;
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS; // 32 bits in red channel, not tied to a specific type
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// Create depth/stencil for shadow map
	D3D11_DEPTH_STENCIL_VIEW_DESC dDesc;
	ZeroMemory(&dDesc, sizeof(dDesc));
	dDesc.Flags = 0;
	dDesc.Format = DXGI_FORMAT_D32_FLOAT; // Gotta give it the D
	dDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &dDesc, &shadowMapDSV);

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowMapSRV);

	// All done with this texture reference
	shadowTexture->Release();

	// Create a sampler state for the shadow map
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	device->CreateSamplerState(&sampDesc, &shadowSampler);

	// Create a rasterizer state for rendering the shadow map
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;//D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;
	rsDesc.DepthBias = 1000; // this is multiplied by (smallest possible value > 0 in depth buffer)
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&rsDesc, &shadowRS);

	shadowVS = new SimpleVertexShader(device, deviceContext);
	shadowVS->LoadShaderFile(L"Shaders/ShadowMap.cso");
}

ID3D11Device* Game::getDevice() { return device; }
ID3D11DeviceContext* Game::getDeviceContext() { return deviceContext; }
float Game::getAspectRatio() { return aspectRatio; }
ID3D11SamplerState* Game::getShadowSampler() { return shadowSampler; }
ID3D11ShaderResourceView* Game::getShadowSRV() { return shadowMapSRV; }

void Game::update(float deltaTime, float totalTime)
{
	earlyUpdateAllGameObjects(deltaTime, totalTime);
	updateAllGameObjects(deltaTime, totalTime);
}

void Game::draw()
{
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	renderShadowMap();
	drawAllGameObjects();
	// render volumetric light(s)

	HR(swapChain->Present(0, 0));
}

// Renders the shadow map for this frame
void Game::renderShadowMap()
{
	// Initial setup - No RTV necessary - Clear shadow map
	deviceContext->OMSetRenderTargets(0, 0, shadowMapDSV);
	deviceContext->ClearDepthStencilView(shadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->RSSetState(shadowRS);

	// Need a viewport!  Copy from existing, and change w/h
	D3D11_VIEWPORT shadowVP = viewport;
	shadowVP.Width = (float)shadowMapSize;
	shadowVP.Height = (float)shadowMapSize;
	deviceContext->RSSetViewports(1, &shadowVP);

	// Set up shader
	shadowVS->SetShader(false); // Don't copy yet

	SpotLight light = DEFAULT_LIGHT->getSpotLight();
	shadowVS->SetData("spotLight", &light, sizeof(SpotLight));
	deviceContext->PSSetShader(0, 0, 0);

	shadowAllGameObjects(shadowVS);

	// Revert to original targets and viewport
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->RSSetState(0);
}
