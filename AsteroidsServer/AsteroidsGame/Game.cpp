#include "Game.h"

Game::Game()
{
	// Create everything and initialize it all to zero
	asteroidPositions = new Vec2(MAX_ASTEROIDS);
	asteroidVelocities = new Vec2(MAX_ASTEROIDS);
	asteroidRadius = new Vec1(MAX_ASTEROIDS);

	shipPositions = new Vec2(MAX_SHIPS);
	shipVelocities = new Vec2(MAX_SHIPS);
	shipAccelerations = new Vec2(MAX_SHIPS);
	shipCollisions = new Vec1(MAX_SHIPS);

	lightPositions = new Vec2(MAX_SHIPS);
	lightVelocities = new Vec2(MAX_SHIPS);
	lightAccelerations = new Vec2(MAX_SHIPS);

	bulletPositions = new Vec2(MAX_BULLETS);
	bulletPrevPositions = new Vec2(MAX_BULLETS);
	bulletVelocities = new Vec2(MAX_BULLETS);

	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = false;
		shipsAlive[i + 1] = false;
		shipsAlive[i + 2] = false;
		shipsAlive[i + 3] = false;
	}

	for (int i = 0; i < MAX_BULLETS; ++i){
		bulletsActive[i] = true;
	}

	dirtyBuffers = false;
}

Game::~Game()
{
	delete asteroidPositions;
	delete asteroidVelocities;
	delete asteroidRadius;

	delete shipPositions;
	delete shipVelocities;
	delete shipAccelerations;
	delete shipCollisions;

	delete lightPositions;
	delete lightVelocities;
	delete lightAccelerations;

	delete bulletPositions;
	delete bulletPrevPositions;
	delete bulletVelocities;
}

void Game::Update(float deltaTime){

	// Update the buffer if necessary.
	if (dirtyBuffers){
		bufferMutex.lock();
		memcpy(shipAccelerations->x, shipAccelerationXBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(shipAccelerations->y, shipAccelerationYBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(lightAccelerations->x, lightAccelerationXBuffer, sizeof(float) * MAX_LIGHTS);
		memcpy(lightAccelerations->y, lightAccelerationYBuffer, sizeof(float) * MAX_LIGHTS);
		bufferMutex.unlock();
		dirtyBuffers = false;
	}

	__m128 dt = _mm_set1_ps(deltaTime);

	//
	// Asteroid Physics
	//

	// Update positions for all the asteroids
	for (int i = 0; i < MAX_ASTEROIDS; i += 4){
		// Load necessary memory
		__m128 velocitiesX = _mm_load_ps(asteroidVelocities->x + i);
		__m128 positionsX = _mm_load_ps(asteroidPositions->x + i);
		__m128 velocitiesY = _mm_load_ps(asteroidVelocities->y + i);
		__m128 positionsY = _mm_load_ps(asteroidPositions->y + i);


		// Perform necessary physics for moving asteroids
		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);

		// Store information
		_mm_store_ps(asteroidPositions->x + i, positionsX);
		_mm_store_ps(asteroidPositions->y + i, positionsY);
	}

	//
	// Bullet Physics
	//

	// Keep track of the previous bullet positions.
	memcpy(bulletPrevPositions->x, bulletPositions->x, sizeof(float) * MAX_BULLETS);
	memcpy(bulletPrevPositions->y, bulletPositions->y, sizeof(float) * MAX_BULLETS);

	// Update data for all bullets
	for (int i = 0; i < MAX_BULLETS; i += 4){
		// Load bullet memory
		__m128 velocitiesX = _mm_load_ps(bulletVelocities->x + i);
		__m128 positionsX = _mm_load_ps(bulletPositions->x + i);
		__m128 velocitiesY = _mm_load_ps(bulletVelocities->y + i);
		__m128 positionsY = _mm_load_ps(bulletPositions->y + i);

		// Perform necessary physics for moving bullets
		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);

		// Store information
		_mm_store_ps(bulletPositions->x + i, positionsX);
		_mm_store_ps(bulletPositions->y + i, positionsY);

	}

	memset(splitAsteroids, 0, sizeof(bool) * MAX_ASTEROIDS);

	float* results = new float[4];
	memset(results, 0, sizeof(float) * 4);
	// Bullet vs asteroid collision detection
	for (int i = 0; i < MAX_BULLETS; ++i){
		if (bulletsActive[i]){

			//if asteroidRad^2 > ((dy*AstX - dx*AstY + x2*y1 - y2*x1 )^2) / (y2-y1)^2 + (x2-x1)^2
			__m128 dy = _mm_set1_ps(bulletPositions->y[i] - bulletPrevPositions->y[i]);
			__m128 dx = _mm_set1_ps(bulletPositions->x[i] - bulletPrevPositions->x[i]);
			__m128 dif = _mm_set1_ps(bulletPositions->x[i] * bulletPrevPositions->y[i] - bulletPositions->y[i] * bulletPrevPositions->x[i]);
			__m128 div = _mm_set1_ps((bulletPositions->y[i] - bulletPrevPositions->y[i]) * (bulletPositions->y[i] - bulletPrevPositions->y[i]) +
				(bulletPositions->x[i] - bulletPrevPositions->x[i]) * (bulletPositions->x[i] - bulletPrevPositions->x[i]));

			

			// We can handle four asteroids at a time
			for (int j = 0; j < MAX_ASTEROIDS; j += 4){

				__m128 astX = _mm_load_ps(asteroidPositions->x + j);
				__m128 astY = _mm_load_ps(asteroidPositions->y + j);

				astX = _mm_mul_ps(dy, astX);
				astY = _mm_mul_ps(dx, astY);

				__m128 result = _mm_sub_ps(astX, astY);
				result = _mm_add_ps(result, dif);
				result = _mm_mul_ps(result, result);
				result = _mm_div_ps(result, div);

				__m128 radius = _mm_load_ps(asteroidRadius->value + j);
				radius = _mm_mul_ps(radius, radius);

				result = _mm_cmpgt_ps(radius, result);

				_mm_store_ps(results, result);

				//  if we detect even a single collision we can actually break from the loop, since the chance of two
				//  asteroids completely overlapping when a bullet hits it is so slim that we shouldn't need to account
				//  for it.  Plus it makes more sense that it's "one-shot-one-kill".
				if (results[0]){
					splitAsteroids[j] = splitAsteroids[j] || results[0];
					bulletsActive[i] = false;
					break;
				}
				else if (results[1])
				{
					splitAsteroids[j + 1] = splitAsteroids[j + 1] || results[1];
					bulletsActive[i] = false;
					break;
				}
				else if (results[2]){
					splitAsteroids[j + 2] = splitAsteroids[j + 2] || results[2];
					bulletsActive[i] = false;
					break;
				}
				else if (results[3]){
					splitAsteroids[j + 3] = splitAsteroids[j + 3] || results[3];
					bulletsActive[i] = false;
					break;
				}

			}
		}
	}
	delete[] results;

	//
	// Ship Physics
	//

	// Update velocities and positions for all of the ships
	// Also detect ship collisions against asteroids.
	memset(shipCollisions->value, 0, sizeof(float) * MAX_SHIPS);

	__m128 maxVel = _mm_set1_ps(MAX_SHIP_SPEED);

	for (int i = 0; i < MAX_SHIPS; i += 4){
		// Load all of the necessary memory
		__m128 accelerationsX = _mm_load_ps(shipAccelerations->x + i);
		__m128 velocitiesX = _mm_load_ps(shipVelocities->x + i);
		__m128 positionsX = _mm_load_ps(shipPositions->x + i);
		__m128 accelerationsY = _mm_load_ps(shipAccelerations->y + i);
		__m128 velocitiesY = _mm_load_ps(shipVelocities->y + i);
		__m128 positionsY = _mm_load_ps(shipPositions->y + i);

		// Do acceleration, clamp velocity, and add to position.
		accelerationsX = _mm_mul_ps(dt, accelerationsX);
		accelerationsY = _mm_mul_ps(dt, accelerationsY);

		velocitiesX = _mm_add_ps(accelerationsX, velocitiesX);
		velocitiesX = _mm_min_ps(velocitiesX, maxVel);

		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		velocitiesY = _mm_min_ps(velocitiesY, maxVel);

		_mm_store_ps(shipVelocities->x + i, velocitiesX);
		_mm_store_ps(shipVelocities->x + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(shipPositions->x + i, positionsX);
		_mm_store_ps(shipPositions->y + i, positionsY);

		// Do collisions detection
		// (x1 - x2)^2 + (y1-y2)^2 < (radius + shipRadius)^2

		__m128 shipR = _mm_set1_ps(SHIP_RADIUS);

		for (int j = 0; j < MAX_ASTEROIDS; j++){
			__m128 asteroidR = _mm_load1_ps(asteroidRadius->value + j);
			
			asteroidR = _mm_add_ps(asteroidR, shipR);
			asteroidR = _mm_mul_ps(asteroidR, asteroidR);
			
			__m128 dx = _mm_sub_ps(positionsX, _mm_load1_ps(asteroidPositions->x + j));
			__m128 dy = _mm_sub_ps(positionsY, _mm_load1_ps(asteroidPositions->y + j));
			dx = _mm_mul_ps(dx, dx);
			dy = _mm_mul_ps(dy, dy);

			__m128 result = _mm_add_ps(dx, dy);
			result = _mm_cmplt_ps(result, asteroidR);

			_mm_store_ps(shipCollisions->value + i, result);
		}
	}

	//
	// Light physics
	//

	// Update velocities and positions for all of the lights
	for (int i = 0; i < MAX_LIGHTS; i += 4){
		// Load all necessary memory.
		__m128 accelerationsX = _mm_load_ps(lightAccelerations->x + i);
		__m128 velocitiesX = _mm_load_ps(lightVelocities->x + i);
		__m128 positionsX = _mm_load_ps(lightPositions->x + i);
		__m128 accelerationsY = _mm_load_ps(lightAccelerations->y + i);
		__m128 velocitiesY = _mm_load_ps(lightVelocities->y + i);
		__m128 positionsY = _mm_load_ps(lightPositions->y + i);

		// Do acceleration, clamp velocity, and add to position.

		accelerationsX = _mm_mul_ps(dt, accelerationsX);
		accelerationsY = _mm_mul_ps(dt, accelerationsY);

		velocitiesX = _mm_add_ps(accelerationsX, velocitiesX);
		velocitiesX = _mm_min_ps(velocitiesX, maxVel);

		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		velocitiesY = _mm_min_ps(velocitiesY, maxVel);

		_mm_store_ps(lightVelocities->x + i, velocitiesX);
		_mm_store_ps(lightVelocities->y + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);
		_mm_store_ps(lightPositions->x + i, positionsX);
		_mm_store_ps(lightPositions->y + i, positionsY);
	}

	// Updating collision

	// For now we just kill for collisions.
	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = !shipCollisions->value[i];  // I can unroll the loop thanks to it being four based
		shipsAlive[i + 1] = !shipCollisions->value[i + 1];
		shipsAlive[i + 2] = !shipCollisions->value[i + 2];
		shipsAlive[i + 3] = !shipCollisions->value[i + 3];
	}
}