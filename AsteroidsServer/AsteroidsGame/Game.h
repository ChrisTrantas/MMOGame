#pragma once
#include <intrin.h>
#include <string>

// Please make multiples of 4.
// The formula is 4 * numberofthings/4
#define MAX_ASTEROIDS 4 * (64/4)
#define MAX_SHIPS 4 * (8/4)
#define MAX_LIGHTS 4 * (8/4)

#define SHIP_RADIUS 1.0f

/// <summary>
/// A single instance of the game, capable of storing all of its own data.
/// </summary>
class Game
{
private:
	float* asteroidPositionsX;
	float* asteroidPositionsY;
	float* asteroidVelocitiesX;
	float* asteroidVelocitiesY;
	float* asteroidRadius;

	float* shipPositionsX;
	float* shipPositionsY;
	float* shipVelocitiesX;
	float* shipVelocitiesY;
	float* shipAccelerationsX;
	float* shipAccelerationsY;
	float* shipCollisions;
	
	float* lightPositionsX;
	float* lightPositionsY;
	float* lightVelocitiesX;
	float* lightVelocitiesY;
	float* lightAccelerationsX;
	float* lightAccelerationsY;

	bool shipsAlive[MAX_SHIPS];
	bool activeShips[MAX_SHIPS];

public:
	Game();
	~Game();

	/// <summary>
	/// Updates all of the game physics.
	/// </summary>
	/// <param name="deltaTime">The change in time since the last update</param>
	void Update(float deltaTime);
};

