#include "engine\Game.h"
#include "TestComponent.h"
#include "FPController.h"
#include "Asteroid.h"

SimpleVertexShader* vs, *avs;
SimplePixelShader* ps;

void buildGame()
{
	vs = new SimpleVertexShader(DEVICE, DEVICE_CONTEXT);
	avs = new SimpleVertexShader(DEVICE, DEVICE_CONTEXT);
	ps = new SimplePixelShader(DEVICE, DEVICE_CONTEXT);

	vs->LoadShaderFile(L"Shaders/VertexShader.cso");
	avs->LoadShaderFile(L"Shaders/AsteroidVertexShader.cso");
	ps->LoadShaderFile(L"Shaders/PixelShader.cso");

	DEFAULT_MATERIAL->vertexShader = vs;
	DEFAULT_MATERIAL->pixelShader = ps;

	GameObject* test = GameObject::getGameObject();
	MeshRenderer* mr = test->getComponent<MeshRenderer>();
	mr->mesh = Mesh::getMesh("Models/wilhelm4baked.fbx");
	mr->material = Material::getMaterial("wilhelm");
	mr->material->vertexShader = vs;
	mr->material->pixelShader = ps;
	mr->material->textures["diffuse"] = Texture::getTexture("Textures/wilhelm_color.png");
	mr->material->textures["normalMap"] = Texture::getTexture("Textures/wilhelm_normal.png");
	test->addComponent<TestComponent>(new TestComponent(1));

	Mesh* sphere = Mesh::getMesh("Models/geodesicTriSphere20.fbx");
	mr = DEFAULT_LIGHT->getComponent<MeshRenderer>();
	mr->mesh = sphere;
	mr->material = Material::getMaterial("rocky");
	mr->material->vertexShader = avs;
	mr->material->pixelShader = ps;
	mr->material->textures["diffuse"] = Texture::getTexture("Textures/rock.jpg");
	mr->material->textures["normalMap"] = Texture::getTexture("Textures/rockNormals.jpg");
	Transform* lightT = DEFAULT_LIGHT->getComponent<Transform>();
	lightT->translate(lightT->forward() * -15.0f);
	DEFAULT_LIGHT->addComponent<Asteroid>(new Asteroid(1.25f, vs, avs));

	Light* secondLight = Light::getLight("light2");
	mr = secondLight->getComponent<MeshRenderer>();
	mr->mesh = sphere;
	mr->material = Material::getMaterial("rocky");
	lightT = secondLight->getComponent<Transform>();
	lightT->rotate(angleAxis((float)M_PI_2, lightT->right()));
	lightT->translate(lightT->forward() * -15.0f);
	secondLight->ambientColor = vec4(0, 0.5f, 1, 1);
	secondLight->diffuseColor = vec4(1, 0.5f, 0, 1);
	secondLight->addComponent<Asteroid>(new Asteroid(1.6f, vs, avs));

	Transform* camT = DEFAULT_CAMERA->getComponent<Transform>();
	camT->rotate(angleAxis((float)M_PI_2, camT->up()));
	camT->translate(camT->forward() * -220.0f);
	DEFAULT_CAMERA->addComponent<FPController>(new FPController());
}

void destructGame()
{
	delete vs;
	delete avs;
	delete ps;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	Game::init(hInstance);
	return Game::game->start(buildGame, destructGame);
}
