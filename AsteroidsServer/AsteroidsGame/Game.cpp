#include "Game.h"

Game::Game()
{
	// Create everything and initialize it all to zero
	asteroidPositions = Vec2(MAX_ASTEROIDS);
	asteroidVelocities = Vec2(MAX_ASTEROIDS);
	asteroidRadius = Vec1(MAX_ASTEROIDS);

	shipPositions = Vec2(MAX_SHIPS);
	shipVelocities = Vec2(MAX_SHIPS);
	shipAccelerations = Vec2(MAX_SHIPS);
	shipCollisions = Vec1(MAX_SHIPS);

	lightPositions = Vec2(MAX_SHIPS);
	lightVelocities = Vec2(MAX_SHIPS);
	lightAccelerations = Vec2(MAX_SHIPS);

	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = false;
		shipsAlive[i + 1] = false;
		shipsAlive[i + 2] = false;
		shipsAlive[i + 3] = false;
	}

	dirtyBuffers = false;
}

Game::~Game()
{

}

void Game::Update(float deltaTime){

	// Update the buffer if necessary.
	if (dirtyBuffers){
		bufferMutex.lock();
		memcpy(shipAccelerations.x, shipAccelerationXBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(shipAccelerations.y, shipAccelerationYBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(lightAccelerations.x, lightAccelerationXBuffer, sizeof(float) * MAX_LIGHTS);
		memcpy(lightAccelerations.y, lightAccelerationYBuffer, sizeof(float) * MAX_LIGHTS);
		bufferMutex.unlock();
		dirtyBuffers = false;
	}

	__m128 dt = _mm_set1_ps(deltaTime);

	// Update positions for all the asteroids
	for (int i = 0; i < MAX_ASTEROIDS; i += 4){
		// Load necessary Memory
		__m128 velocitiesX = _mm_load_ps(asteroidVelocities.x + i);
		__m128 positionsX = _mm_load_ps(asteroidPositions.x + i);
		__m128 velocitiesY = _mm_load_ps(asteroidVelocities.y + i);
		__m128 positionsY = _mm_load_ps(asteroidPositions.y + i);


		// Perform necessary physics for moving ship
		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);

		// Store information
		_mm_store_ps(asteroidPositions.x + i, positionsX);
		_mm_store_ps(asteroidPositions.y + i, positionsY);
	}

	// Update velocities and positions for all of the ships
	// Also detect ship collisions against asteroids.
	memset(shipCollisions.value, 0, sizeof(float) * MAX_SHIPS);

	__m128 maxVel = _mm_set1_ps(MAX_SHIP_SPEED);

	for (int i = 0; i < MAX_SHIPS; i += 4){
		// Load all of the necessary memory
		__m128 accelerationsX = _mm_load_ps(shipAccelerations.x + i);
		__m128 velocitiesX = _mm_load_ps(shipVelocities.x + i);
		__m128 positionsX = _mm_load_ps(shipPositions.x + i);
		__m128 accelerationsY = _mm_load_ps(shipAccelerations.y + i);
		__m128 velocitiesY = _mm_load_ps(shipVelocities.y + i);
		__m128 positionsY = _mm_load_ps(shipPositions.y + i);

		// Do acceleration, clamp velocity, and add to position.
		accelerationsX = _mm_mul_ps(dt, accelerationsX);
		accelerationsY = _mm_mul_ps(dt, accelerationsY);

		velocitiesX = _mm_add_ps(accelerationsX, velocitiesX);
		velocitiesX = _mm_min_ps(velocitiesX, maxVel);

		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		velocitiesY = _mm_min_ps(velocitiesY, maxVel);

		_mm_store_ps(shipVelocities.x + i, velocitiesX);
		_mm_store_ps(shipVelocities.x + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(shipPositions.x + i, positionsX);
		_mm_store_ps(shipPositions.y + i, positionsY);

		// Do collisions detection
		// (x1 - x2)^2 + (y1-y2)^2 < (radius + shipRadius)^2

		__m128 shipR = _mm_set1_ps(SHIP_RADIUS);

		for (int j = 0; j < MAX_ASTEROIDS; j++){
			__m128 asteroidR = _mm_load1_ps(asteroidRadius.value + j);
			
			asteroidR = _mm_add_ps(asteroidR, shipR);
			asteroidR = _mm_mul_ps(asteroidR, asteroidR);
			
			__m128 dx = _mm_sub_ps(positionsX, _mm_load1_ps(asteroidPositions.x + j));
			__m128 dy = _mm_sub_ps(positionsY, _mm_load1_ps(asteroidPositions.y + j));
			dx = _mm_mul_ps(dx, dx);
			dy = _mm_mul_ps(dy, dy);

			__m128 result = _mm_add_ps(dx, dy);
			result = _mm_cmplt_ps(result, asteroidR);

			_mm_store_ps(shipCollisions.value + i, result);
		}
	}

	// Update velocities and positions for all of the lights
	for (int i = 0; i < MAX_LIGHTS; i += 4){
		// Load all necessary memory.
		__m128 accelerationsX = _mm_load_ps(lightAccelerations.x + i);
		__m128 velocitiesX = _mm_load_ps(lightVelocities.x + i);
		__m128 positionsX = _mm_load_ps(lightPositions.x + i);
		__m128 accelerationsY = _mm_load_ps(lightAccelerations.y + i);
		__m128 velocitiesY = _mm_load_ps(lightVelocities.y + i);
		__m128 positionsY = _mm_load_ps(lightPositions.y + i);

		// Do acceleration, clamp velocity, and add to position.

		accelerationsX = _mm_mul_ps(dt, accelerationsX);
		accelerationsY = _mm_mul_ps(dt, accelerationsY);

		velocitiesX = _mm_add_ps(accelerationsX, velocitiesX);
		velocitiesX = _mm_min_ps(velocitiesX, maxVel);

		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		velocitiesY = _mm_min_ps(velocitiesY, maxVel);

		_mm_store_ps(lightVelocities.x + i, velocitiesX);
		_mm_store_ps(lightVelocities.y + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(lightPositions.x + i, positionsX);
		_mm_store_ps(lightPositions.y + i, positionsY);
	}

	// For now we just kill for collisions.
	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = !shipCollisions.value[i];  // I can unroll the loop thanks to it being four based
		shipsAlive[i + 1] = !shipCollisions.value[i + 1];
		shipsAlive[i + 2] = !shipCollisions.value[i + 2];
		shipsAlive[i + 3] = !shipCollisions.value[i + 3];
	}
}