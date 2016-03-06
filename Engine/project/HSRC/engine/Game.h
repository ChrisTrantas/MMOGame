#pragma once

#include <Windows.h>
#include "Camera.h"
#include "Light.h"
#include <WindowsX.h>
#include <sstream>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define DEVICE Game::game->getDevice()
#define DEVICE_CONTEXT Game::game->getDeviceContext()

class Game
{
public:
	static void init(HINSTANCE hInstance);
	static Game* game;
	int start(void(*buildFunc)());
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	float getAspectRatio();

	LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Game(HINSTANCE hInstance);
	~Game();
	Game(Game const&) = delete;
	void operator=(Game const&) = delete;
	void initEngine();
	void update(float deltaTime, float totalTime);
	void draw();

	bool InitMainWindow();
	bool InitDirect3D();

	void UpdateTimer();
	void CalculateFrameStats();

	// Window handles and such
	HINSTANCE hAppInst;
	HWND hMainWnd;

	// window details and tracking
	wstring windowCaption = L"HSRC";
	int windowWidth = 1280;
	int windowHeight = 720;
	float aspectRatio;
	bool hasFocus = false;
	bool minimized = false;
	bool maximized = false;
	bool resizing = false;
	void OnResize();
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// DirectX related buffers, views, etc.
	ID3D11Device*             device;
	ID3D11DeviceContext*      deviceContext;
	IDXGISwapChain*           swapChain;
	ID3D11Texture2D*          depthStencilBuffer;
	ID3D11RenderTargetView*   renderTargetView;
	ID3D11DepthStencilView*   depthStencilView;
	D3D11_VIEWPORT            viewport;
	D3D_DRIVER_TYPE           driverType = D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL         featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Timer related data
	double perfCounterSeconds = 0;
	__int64 startTime = 0;
	__int64 currentTime = 0;
	__int64 previousTime = 0;
	float totalTime = 0;
	float deltaTime = 0;
};