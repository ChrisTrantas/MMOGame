#include "engine\Game.h"
#include "FPController.h"
#include "Asteroid.h"
#include "Rotate.h"
#include "networking\NetworkManager.h"
#include "threading\Thread.h"

namespace
{
	SimpleVertexShader* vs, *avs;
	SimplePixelShader* ps, *aps;
}

void buildGame()
{
#pragma region ShaderSetup
	vs = new SimpleVertexShader(DEVICE, DEVICE_CONTEXT);
	avs = new SimpleVertexShader(DEVICE, DEVICE_CONTEXT);
	ps = new SimplePixelShader(DEVICE, DEVICE_CONTEXT);
	aps = new SimplePixelShader(DEVICE, DEVICE_CONTEXT);

	vs->LoadShaderFile(L"Shaders/VertexShader.cso");
	ps->LoadShaderFile(L"Shaders/PixelShader.cso");
	avs->LoadShaderFile(L"Shaders/AsteroidVertexShader.cso");
	aps->LoadShaderFile(L"Shaders/AsteroidPixelShader.cso");

	DEFAULT_MATERIAL->vertexShader = vs;
	DEFAULT_MATERIAL->pixelShader = ps;

	Material::getMaterial("asteroid")->vertexShader = avs;
	Material::getMaterial("asteroid")->pixelShader = aps;
#pragma endregion The shader setup and material binding phase

	for (size_t i = 0; i < 8; ++i)
	{
		GameObject* go = GameObject::getGameObject();
		float ang = M_PI * 2 * (i / 8.0f);
		go->getComponent<Transform>()->translate(vec3(cosf(ang) * 4, sinf(ang) * 4, 0));
		go->addComponent<Asteroid>(new Asteroid());
	}

#pragma region LightSetup
	DEFAULT_LIGHT->addComponent<Rotate>(new Rotate(1.18f, vec3(1, 0, 0)));
	Light* secondLight = Light::getLight("light2");
	secondLight->addComponent<Rotate>(new Rotate(0.87f, vec3(0, 1, 0)));
	secondLight->ambientColor = vec4(0, 0.5f, 1, 1);
	secondLight->diffuseColor = vec4(1, 0.5f, 0, 1);
#pragma endregion The light setup and binding phase

#pragma region CameraSetup
	Transform* camT = DEFAULT_CAMERA->getComponent<Transform>();
	camT->translate(camT->forward() * -15.0f);
	DEFAULT_CAMERA->addComponent<FPController>(new FPController());
#pragma endregion The camera setup and binding phase

	NetworkManager::init();
}

void destructGame()
{
	delete vs;
	delete ps;
	delete avs;
	delete aps;

	NetworkManager::networkManager->ShutDownAllThreads();
	NetworkManager::networkManager->shutDownClient();
	delete NetworkManager::networkManager;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Game::init(hInstance);
	return Game::game->start(buildGame, destructGame);
}
