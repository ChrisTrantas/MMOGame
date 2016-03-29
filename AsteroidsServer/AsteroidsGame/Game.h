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

#define SHIP_RADIUS 1.0f
#define MAX_SHIP_SPEED 8.0f

/// <summary>
/// A single instance of the game, capable of storing all of its own data.
/// </summary>
class Game
{
private:
	// Arrays storing all of the game's data.
	Vec2 asteroidPositions;
	Vec2 asteroidVelocities;
	Vec1 asteroidRadius;

	Vec2 shipPositions;
	Vec2 shipVelocities;
	Vec2 shipAccelerations;
	Vec1 shipCollisions;
	
	Vec2 lightPositions;
	Vec2 lightVelocities;
	Vec2 lightAccelerations;

	Vec2 bulletPositions;
	Vec2 bulletPrevPositions;
	Vec2 bulletVelocities;

	bool shipsAlive[MAX_SHIPS];
	bool bulletsActive[MAX_BULLETS];
	bool splitAsteroids[MAX_ASTEROIDS];

public:
	Game();
	~Game();

	std::mutex bufferMutex;
	// If true, updates acceleration data.
	bool dirtyBuffers;

	// When the server gets new inputs, update them here.
	float shipAccelerationXBuffer[MAX_SHIPS];
	float shipAccelerationYBuffer[MAX_SHIPS];

	float lightAccelerationXBuffer[MAX_LIGHTS];
	float lightAccelerationYBuffer[MAX_LIGHTS];

	/// <summary>
	/// Updates all of the game physics.
	/// </summary>
	/// <param name="deltaTime">The change in time since the last update</param>
	void Update(float deltaTime);
};

