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
	DEFAULT_MATERIAL->textures["diffuse"] = DEFAULT_TEXTURE;

	Material::getMaterial("asteroid")->vertexShader = avs;
	Material::getMaterial("asteroid")->pixelShader = aps;
#pragma endregion The shader setup and material binding phase

	//for (size_t i = 0; i < 8; ++i)
	//{
	//	GameObject* go = GameObject::getGameObject();
	//	float ang = (float)M_PI * 2 * (i / 8.0f);
	//	go->getComponent<Transform>()->translate(vec3(cosf(ang) * 4, sinf(ang) * 4, 0));
	//	go->addComponent<Asteroid>(new Asteroid());
	//}
	//
	//for (size_t i = 0; i < 20; ++i)
	//{
	//	GameObject* go = GameObject::getGameObject();
	//	float ang = (float)M_PI * 2 * (i / 20.0f);
	//	go->getComponent<Transform>()->translate(vec3(cosf(ang) * 8, sinf(ang) * 8, 0));
	//	go->addComponent<Asteroid>(new Asteroid());
	//}

	// make the ships here


#pragma region LightSetup
	DEFAULT_LIGHT->getComponent<Transform>()->rotateEuler(vec3(-M_PI_2, 0, 0));
	DEFAULT_LIGHT->addComponent<Rotate>(new Rotate(0.35f, vec3(0, 0, 1)));
#pragma endregion The light setup and binding phase

#pragma region CameraSetup
	Transform* camT = DEFAULT_CAMERA->getComponent<Transform>();
	camT->translate(camT->forward() * -120.0f);
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
