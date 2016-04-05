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
	perlinSeed.x = (float)rand();
	perlinSeed.y = (float)rand();
	perlinSeed.z = (float)rand();
	float deformity = 3.12f;
	float magnitude = 0.3f;
	mr->material->setAttribute(VERTEX_BIT, "deformity", &deformity, sizeof(float), true);
	mr->material->setAttribute(VERTEX_BIT, "magnitude", &magnitude, sizeof(float), true);
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

void Asteroid::draw()
{
	mr->material->setAttribute(VERTEX_BIT, "seed", &perlinSeed, sizeof(vec3), false);
}
