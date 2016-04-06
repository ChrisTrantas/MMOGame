#include "Asteroid.h"
#include "Rotate.h"

Asteroid::Asteroid()
{
	
}

Asteroid::~Asteroid()
{

}

void Asteroid::awake()
{
	t = gameObject->getComponent<Transform>();
	mr = gameObject->getComponent<MeshRenderer>();

	mr->mesh = Mesh::getMesh("Models/geodesicTriSphere20.fbx");
	mr->material = Material::getMaterial("asteroid");
	mr->material->textures["diffuse"] = Texture::getTexture("Textures/rock.jpg");
	mr->material->textures["normalMap"] = Texture::getTexture("Textures/rockNormals.jpg");

	perlinSeed = RAND_VEC3;
	float deformity = 3.12f;
	float magnitude = 0.3f;
	mr->material->setAttribute(VERTEX_BIT, "deformity", &deformity, sizeof(float), true);
	mr->material->setAttribute(VERTEX_BIT, "magnitude", &magnitude, sizeof(float), true);

	Rotate* r = gameObject->getComponent<Rotate>();
	if (!r)
	{
		r = new Rotate(0.5f + RAND_FLOAT, RAND_VEC3);
		gameObject->addComponent<Rotate>(r);
	}
	else
	{
		r->setSpeed(0.5f + RAND_FLOAT);
		r->setAxis(RAND_VEC3);
	}
}

void Asteroid::update(float deltaTime, float totalTime)
{
	// move the asteroid here
}

void Asteroid::draw()
{
	mr->material->setAttribute(VERTEX_BIT, "seed", &perlinSeed, sizeof(vec3), false);
}
