#include "engine\Game.h"
#include "TestComponent.h"
#include "FPController.h"

void buildGame()
{
	Mesh* wilhelm = Mesh::getMesh("Models/wilhelm4baked.fbx");
	GameObject* test = GameObject::getGameObject();
	test->getComponent<MeshRenderer>()->mesh = wilhelm;
	test->addComponent<TestComponent>(new TestComponent());

	Mesh* sphere = Mesh::getMesh("Models/Sphere-v64-h64.obj");
	DEFAULT_LIGHT->getComponent<MeshRenderer>()->mesh = sphere;
	Transform* lightT = DEFAULT_LIGHT->getComponent<Transform>();
	lightT->translate(lightT->forward() * -15.0f);

	Light* secondLight = Light::getLight("light2");
	secondLight->getComponent<MeshRenderer>()->mesh = sphere;
	lightT = secondLight->getComponent<Transform>();
	lightT->rotate(angleAxis((float)M_PI_2, lightT->right()));
	lightT->translate(lightT->forward() * -15.0f);
	secondLight->ambientColor = vec4(0, 0.5f, 1, 1);
	secondLight->diffuseColor = vec4(1, 0.5f, 0, 1);

	Transform* camT = DEFAULT_CAMERA->getComponent<Transform>();
	camT->rotate(angleAxis((float)M_PI_2, camT->up()));
	camT->translate(camT->forward() * -220.0f);
	DEFAULT_CAMERA->addComponent<FPController>(new FPController());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	Game::init(hInstance);
	return Game::game->start(buildGame);
}
