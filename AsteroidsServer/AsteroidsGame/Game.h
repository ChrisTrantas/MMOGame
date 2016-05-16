#pragma once
#include <intrin.h>
#include <string>
#include <mutex>
#include "MathVectors.h"

// Please make multiples of 4.
// The formula is 4 * numberofthings/4
#define MAX_ASTEROIDS 4 * (64/4)
#define MAX_SHIPS 4 * (8/4)
#define MAX_LIGHTS 4 * (8/4)
#define MAX_BULLETS 4 * (128/4)

#define STARTING_ASTEROID_RADIUS 8
#define SHIP_RADIUS 1.0f
#define MAX_SHIP_SPEED 8.0f

#define BOUNDS_LEFT -10.0f
#define BOUNDS_RIGHT 10.0f
#define BOUNDS_UP -10.0f
#define BOUNDS_DOWN 10.0f
#define SCREEN_WIDTH BOUNDS_RIGHT - BOUNDS_LEFT
#define SCREEN_HEIGHT BOUNDS_DOWN - BOUNDS_UP

/// <summary>
/// A single instance of the game, capable of storing all of its own data.
/// </summary>
class Game
{
private:
	// constants
	const __m128 c_LEFT = _mm_set1_ps(BOUNDS_LEFT);
	const __m128 c_RIGHT = _mm_set1_ps(BOUNDS_RIGHT);
	const __m128 c_DOWN = _mm_set1_ps(BOUNDS_DOWN);
	const __m128 c_UP = _mm_set1_ps(BOUNDS_UP);
	const __m128 c_WIDTH_SHIFT = _mm_set1_ps(SCREEN_WIDTH);
	const __m128 c_HEIGHT_SHIFT = _mm_set1_ps(SCREEN_HEIGHT);


	// Arrays storing all of the game's data.
	Vec2* asteroidPositions;//  sizeof(float)*64;  memcpy(buffer + sizeof(float)*64, asteroidPositions->y, sizeof(float)* 64);
	Vec2* asteroidVelocities;
	Vec1* asteroidRadius;//

	Vec2* shipPositions;//
	Vec2* shipVelocities;
	Vec2* shipAccelerations;
	Vec1* shipCollisions;
	Vec1* shipRot;//
	
	Vec2* lightPositions;//
	Vec2* lightVelocities;
	Vec2* lightAccelerations;
	Vec1* lightRot;//

	Vec2* bulletPositions;//
	Vec2* bulletPrevPositions;
	Vec2* bulletVelocities;

	bool shipsAlive[MAX_SHIPS];
	bool bulletsActive[MAX_BULLETS];
	int bulletHighIndex;//The highest index reached so far.
	int bulletLowIndex;//The lowest free index so far.
	bool splitAsteroids[MAX_ASTEROIDS];
	int asteroidsHighIndex;
	int asteroidsLowIndex;

	// Temporary buffer for math
	float* results;


public:
	Game();
	~Game();

	std::mutex bufferMutex;
	std::mutex physicsMutex;
	// If true, updates acceleration data.
	bool dirtyBuffers;

	// When the server gets new inputs, update them here.
	float shipAccelerationXBuffer[MAX_SHIPS];
	float shipAccelerationYBuffer[MAX_SHIPS];
	float shipRotBuffer[MAX_SHIPS];

	float lightAccelerationXBuffer[MAX_LIGHTS];
	float lightAccelerationYBuffer[MAX_LIGHTS];
	float lightRotBuffer[MAX_SHIPS];

	float asteroidVelocitiesXBuffer[MAX_ASTEROIDS];
	float asteroidVelocitiesYBuffer[MAX_ASTEROIDS];

	/// <summary>
	/// Updates all of the game physics.
	/// </summary>
	/// <param name="deltaTime">The change in time since the last update</param>
	void Update(float deltaTime);

	/// <summary>
	/// Fires a bullet in the direction specified from the position listed.
	/// </summary>
	void FireBullet(float x, float y, float xVel, float yVel);

	Vec2* GetAsteroidPos();
	Vec1* GetAsteroidRadius();

	Vec2* GetShipPos();
	Vec1* GetShipRot();

	Vec2* GetLightPos();
	Vec1* GetLightRot();

	Vec2* GetBulletPos();
};

