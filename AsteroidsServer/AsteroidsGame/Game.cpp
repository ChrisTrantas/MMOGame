#include "Game.h"

inline void alignInit(float* data, int numObjects)
{
	data = (float*)_aligned_malloc(sizeof(float) * numObjects, 32);
	memset(data, 0, sizeof(float) * numObjects);
}

Game::Game()
{
	// Create everything and initialize it all to zero
	alignInit(asteroidPositionsX, MAX_ASTEROIDS);
	alignInit(asteroidPositionsY, MAX_ASTEROIDS);
	alignInit(asteroidVelocitiesX, MAX_ASTEROIDS);
	alignInit(asteroidVelocitiesY, MAX_ASTEROIDS);
	alignInit(asteroidRadius, MAX_ASTEROIDS);

	alignInit(shipPositionsX, MAX_SHIPS);
	alignInit(shipPositionsY, MAX_SHIPS);
	alignInit(shipVelocitiesX, MAX_SHIPS);
	alignInit(shipVelocitiesY, MAX_SHIPS);
	alignInit(shipAccelerationsX, MAX_SHIPS);
	alignInit(shipAccelerationsY, MAX_SHIPS);
	alignInit(shipCollisions, MAX_SHIPS);

	alignInit(lightPositionsX, MAX_LIGHTS);
	alignInit(lightPositionsY, MAX_LIGHTS);
	alignInit(lightVelocitiesX, MAX_LIGHTS);
	alignInit(lightVelocitiesY, MAX_LIGHTS);
	alignInit(lightAccelerationsX, MAX_LIGHTS);
	alignInit(lightAccelerationsY, MAX_LIGHTS);

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
	// Deallocate all of the aligned memory
	_aligned_free(asteroidPositionsX);
	_aligned_free(asteroidVelocitiesX);
	_aligned_free(asteroidPositionsY);
	_aligned_free(asteroidVelocitiesY);
	_aligned_free(asteroidRadius);

	_aligned_free(shipPositionsX);
	_aligned_free(shipVelocitiesX);
	_aligned_free(shipAccelerationsX);
	_aligned_free(shipPositionsY);
	_aligned_free(shipVelocitiesY);
	_aligned_free(shipAccelerationsY);

	_aligned_free(lightPositionsX);
	_aligned_free(lightVelocitiesX);
	_aligned_free(lightAccelerationsX);
	_aligned_free(lightPositionsY);
	_aligned_free(lightVelocitiesY);
	_aligned_free(lightAccelerationsY);

}

void Game::Update(float deltaTime){

	// Update the buffer if necessary.
	if (dirtyBuffers){
		bufferMutex.lock();
		memcpy(shipAccelerationsX, shipAccelerationXBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(shipAccelerationsY, shipAccelerationYBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(lightAccelerationsX, lightAccelerationXBuffer, sizeof(float) * MAX_LIGHTS);
		memcpy(lightAccelerationsY, lightAccelerationYBuffer, sizeof(float) * MAX_LIGHTS);
		bufferMutex.unlock();
		dirtyBuffers = false;
	}

	__m128 dt = _mm_set1_ps(deltaTime);

	// Update positions for all the asteroids
	for (int i = 0; i < MAX_ASTEROIDS; i += 4){
		__m128 velocitiesX = _mm_load_ps(asteroidVelocitiesX + i);
		__m128 positionsX = _mm_load_ps(asteroidPositionsX + i);
		__m128 velocitiesY = _mm_load_ps(asteroidVelocitiesY + i);
		__m128 positionsY = _mm_load_ps(asteroidPositionsY + i);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(asteroidPositionsX + i, positionsX);
		_mm_store_ps(asteroidPositionsY + i, positionsY);
	}

	// Update velocities and positions for all of the ships
	// Also detect ship collisions against asteroids.
	memset(shipCollisions, 0, sizeof(float) * MAX_SHIPS);

	for (int i = 0; i < MAX_SHIPS; i += 4){
		__m128 accelerationsX = _mm_load_ps(shipAccelerationsX + i);
		__m128 velocitiesX = _mm_load_ps(shipVelocitiesX + i);
		__m128 positionsX = _mm_load_ps(shipPositionsX + i);
		__m128 accelerationsY = _mm_load_ps(shipAccelerationsY + i);
		__m128 velocitiesY = _mm_load_ps(shipVelocitiesY + i);
		__m128 positionsY = _mm_load_ps(shipPositionsY + i);

		accelerationsX = _mm_mul_ps(dt, accelerationsX);
		accelerationsY = _mm_mul_ps(dt, accelerationsY);

		velocitiesX = _mm_add_ps(accelerationsX, velocitiesX);
		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		_mm_store_ps(shipVelocitiesX + i, velocitiesX);
		_mm_store_ps(shipVelocitiesY + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(shipPositionsX + i, positionsX);
		_mm_store_ps(shipPositionsY + i, positionsY);

		// Do collisions detection
		// (x1 - x2)^2 + (y1-y2)^2 < (radius + shipRadius)^2

		__m128 shipR = _mm_set1_ps(SHIP_RADIUS);

		for (int j = 0; j < MAX_ASTEROIDS; j++){
			__m128 asteroidR = _mm_load1_ps(asteroidRadius + j);
			
			asteroidR = _mm_add_ps(asteroidR, shipR);
			asteroidR = _mm_mul_ps(asteroidR, asteroidR);
			
			__m128 dx = _mm_sub_ps(positionsX, _mm_load1_ps(asteroidPositionsX + j));
			__m128 dy = _mm_sub_ps(positionsY, _mm_load1_ps(asteroidPositionsY + j));
			dx = _mm_mul_ps(dx, dx);
			dy = _mm_mul_ps(dy, dy);

			__m128 result = _mm_add_ps(dx, dy);
			result = _mm_cmplt_ps(result, asteroidR);

			_mm_store_ps(shipCollisions + i, result);
		}
	}

	// Update velocities and positions for all of the lights
	for (int i = 0; i < MAX_LIGHTS; i += 4){
		__m128 accelerationsX = _mm_load_ps(lightAccelerationsX + i);
		__m128 velocitiesX = _mm_load_ps(lightVelocitiesX + i);
		__m128 positionsX = _mm_load_ps(lightPositionsX + i);
		__m128 accelerationsY = _mm_load_ps(lightAccelerationsY + i);
		__m128 velocitiesY = _mm_load_ps(lightVelocitiesY + i);
		__m128 positionsY = _mm_load_ps(lightPositionsY + i);

		accelerationsX = _mm_mul_ps(dt, accelerationsX);
		accelerationsY = _mm_mul_ps(dt, accelerationsY);

		velocitiesX = _mm_add_ps(accelerationsX, velocitiesX);
		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		_mm_store_ps(lightVelocitiesX + i, velocitiesX);
		_mm_store_ps(lightVelocitiesY + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(lightPositionsX + i, positionsX);
		_mm_store_ps(lightPositionsY + i, positionsY);
	}

	// For now we just kill for collisions.
	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = !shipCollisions[i];  // I can unroll the loop thanks to it being four based
		shipsAlive[i + 1] = !shipCollisions[i + 1];
		shipsAlive[i + 2] = !shipCollisions[i + 2];
		shipsAlive[i + 3] = !shipCollisions[i + 3];
	}
}