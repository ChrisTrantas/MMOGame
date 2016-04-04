#include "Asteroid.h"

static bool shaderPass = false;

Asteroid::Asteroid(float speed, SimpleVertexShader* vs, SimpleVertexShader* avs)
{
	this->speed = speed;
	this->vs = vs;
	this->avs = avs;
}

Asteroid::~Asteroid()
{

}

void Asteroid::awake()
{
	t = gameObject->getComponent<Transform>();
	mr = gameObject->getComponent<MeshRenderer>();
	Input::bindToControl("swapShaders", 'V');
}

void Asteroid::earlyUpdate(float deltaTime, float totalTime)
{
	shaderPass = false;
}

void Asteroid::update(float deltaTime, float totalTime)
{
	t->rotate(angleAxis(0.0001f * speed, t->up()));

	if (Input::wasControlPressed("swapShaders") && !shaderPass)
	{
		shaderPass = true;
		if (mr->material->vertexShader == vs)
			mr->material->vertexShader = avs;
		else
			mr->material->vertexShader = vs;
	}
}
