#include "Game.h"

Game::Game()
{
	bulletHighIndex = 0;
	bulletLowIndex = 0;

	asteroidsHighIndex = 0;
	asteroidsLowIndex = 0;

	// Create everything and initialize it all to zero
	asteroidPositions = new Vec2(MAX_ASTEROIDS);
	asteroidVelocities = new Vec2(MAX_ASTEROIDS);
	asteroidRadius = new Vec1(MAX_ASTEROIDS);

	shipPositions = new Vec2(MAX_SHIPS);
	shipVelocities = new Vec2(MAX_SHIPS);
	shipAccelerations = new Vec2(MAX_SHIPS);
	shipCollisions = new Vec1(MAX_SHIPS);
	shipRot = new Vec2(MAX_SHIPS);

	lightPositions = new Vec2(MAX_LIGHTS);
	lightVelocities = new Vec2(MAX_LIGHTS);
	lightAccelerations = new Vec2(MAX_LIGHTS);
	lightRot = new Vec2(MAX_LIGHTS);

	bulletPositions = new Vec2(MAX_BULLETS);
	bulletPrevPositions = new Vec2(MAX_BULLETS);
	bulletVelocities = new Vec2(MAX_BULLETS);

	results = (float*)_aligned_malloc(sizeof(float) * 4, 32);
	memset(results, 0, sizeof(float) * 4);
	memset(asteroidRadius->value, STARTING_ASTEROID_RADIUS, sizeof(float) * 8);

	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = false;
		shipsAlive[i + 1] = false;
		shipsAlive[i + 2] = false;
		shipsAlive[i + 3] = false;
	}

	for (int i = 0; i < MAX_BULLETS; ++i){
		bulletsActive[i] = true;
	}

	for (int i = 0; i < MAX_ASTEROIDS; i++)
	{
		while (asteroidPositions->x[i] > -5 && asteroidPositions->x[i] < 5)
		{
			asteroidPositions->x[i] = rand() % 36 - 18;
		}
		while (asteroidPositions->y[i] > -5 && asteroidPositions->y[i] < 5)
		{
			asteroidPositions->y[i] = rand() % 20 - 10;
		}
		while (asteroidVelocities->x[i] == 0)
		{
			asteroidVelocities->x[i] = rand() % 6 - 3;
		}
		while (asteroidVelocities->y[i] == 0)
		{
			asteroidVelocities->y[i] = rand() % 6 - 3;
		}

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

	_aligned_free(results);
}

void Game::Update(float deltaTime){
	
	// Update the buffer if necessary.
	if (dirtyBuffers){
		bufferMutex.lock();
		memcpy(shipAccelerations->x, shipAccelerationXBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(shipAccelerations->y, shipAccelerationYBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(lightAccelerations->x, lightAccelerationXBuffer, sizeof(float) * MAX_LIGHTS);
		memcpy(lightAccelerations->y, lightAccelerationYBuffer, sizeof(float) * MAX_LIGHTS);
		memcpy(shipRot->y, shipRotBuffer, sizeof(float) * MAX_SHIPS);
		memcpy(lightRot->y, lightRotBuffer, sizeof(float) * MAX_LIGHTS);

		dirtyBuffers = false;
		bufferMutex.unlock();
	}
	//printf("dt: %f \n", deltaTime);
	// A few physics constants
	__m128 dt = _mm_set1_ps(deltaTime);

	//printf("Asteroid X Position: %f \n", asteroidPositions->x[0]);

	//
	// Asteroid Physics
	//

	physicsMutex.lock();

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

		// Shift things if they have gone beyond the bounds.
		__m128 mask;
		mask = _mm_cmplt_ps(positionsX, c_LEFT);
		mask = _mm_and_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_add_ps(positionsX, mask);

		mask = _mm_cmpgt_ps(positionsX, c_RIGHT);
		mask = _mm_and_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_sub_ps(positionsX, mask);

		mask = _mm_cmplt_ps(positionsY, c_UP);
		mask = _mm_and_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_add_ps(positionsY, mask);

		mask = _mm_cmpgt_ps(positionsY, c_DOWN);
		mask = _mm_and_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_sub_ps(positionsY, mask);

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
	for (int i = 0; i < bulletHighIndex / 4; i += 4){
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

		//Bounds happens later since handling wrapping now would lead to issues with the continuous collision detection.

		// Store information
		_mm_store_ps(bulletPositions->x + i, positionsX);
		_mm_store_ps(bulletPositions->y + i, positionsY);

	}

	memset(splitAsteroids, 0, sizeof(bool) * MAX_ASTEROIDS);

	
	// Bullet vs asteroid collision detection
	for (int i = 0; i < bulletHighIndex; ++i){
		if (bulletsActive[i]){

			//  Continuous because I'm assuming the bullet is a point, and the asteroids are all circles.
			//  As a result I just need to check if the line is ever within the circle.
			//  The below formula finds the shortest length of distance from a point to a line.
			//  Comparing that to the radius squared (the whole formula is squared) should tell me if the line is ever in the circle.
			
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

				astX = _mm_mul_ps(dy, astX);//dy*Astx
				astY = _mm_mul_ps(dx, astY);//dx*Asty

				__m128 result = _mm_sub_ps(astX, astY);// dy*AstX - dx*AstY
				result = _mm_add_ps(result, dif);// dy*AstX - dx*AstY + x2*y1 - y2*x1 
				result = _mm_mul_ps(result, result);//^2
				result = _mm_div_ps(result, div);//Division against the length of the line

				// Now compare it to the radius squared
				__m128 radius = _mm_load_ps(asteroidRadius->value + j);
				radius = _mm_mul_ps(radius, radius);

				result = _mm_cmpgt_ps(radius, result);

				_mm_store_ps(results, result); //There is a ghost bug somewhere about this line

				//  if we detect even a single collision we can actually break from the loop, since the chance of two
				//  asteroids completely overlapping when a bullet hits it is so slim that we shouldn't need to account
				//  for it.  Plus it makes more sense that it's "one-shot-one-kill".
				if (results[0]){
					splitAsteroids[j] = splitAsteroids[j] || results[0];
					bulletsActive[i] = false;
					if (i < bulletLowIndex){
						bulletLowIndex = i;
					}
					break;
				}
				if (results[1])
				{
					splitAsteroids[j + 1] = splitAsteroids[j + 1] || results[1];
					bulletsActive[i] = false;
					if (i < bulletLowIndex){
						bulletLowIndex = i;
					}
					break;
				}
				if (results[2]){
					splitAsteroids[j + 2] = splitAsteroids[j + 2] || results[2];
					bulletsActive[i] = false;
					if (i < bulletLowIndex){
						bulletLowIndex = i;
					}
					break;
				}
				if (results[3]){
					splitAsteroids[j + 3] = splitAsteroids[j + 3] || results[3];
					bulletsActive[i] = false;
					if (i < bulletLowIndex){
						bulletLowIndex = i;
					}
					break;
				}

			}
		}
	}
	// Now that we know where any split asteroids are, let's split them.
	for (int i = 0; i < asteroidsHighIndex; ++i){
		if (splitAsteroids[i]){
			splitAsteroids[i] = false;
			asteroidRadius->value[i] = asteroidRadius->value[i] / 2.0f;
			if (asteroidRadius->value[i] < 0.37f){
				asteroidRadius->value[i] = 0;
				continue;
			}
			asteroidRadius->value[asteroidsLowIndex] = asteroidRadius->value[i];
			for (int j = asteroidsLowIndex; j < MAX_ASTEROIDS; ++j){
				if (asteroidRadius->value[j] == 0){
					asteroidsLowIndex = j;
					if (j > asteroidsHighIndex){
						asteroidsHighIndex = j;
					}
					break;
				}
			}

		}
	}

	// Need to do the bullet boundary movement after the collision check since otherwise that bullet moves really far really fast.
	for (int i = 0; i < bulletHighIndex / 4; i += 4){
		// Load bullet memory
		__m128 positionsX = _mm_load_ps(bulletPositions->x + i);
		__m128 positionsY = _mm_load_ps(bulletPositions->y + i);

		// Shift things if they have gone beyond the bounds.
		__m128 mask;
		mask = _mm_cmplt_ps(positionsX, c_LEFT);
		mask = _mm_and_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_add_ps(positionsX, mask);

		mask = _mm_cmpgt_ps(positionsX, c_RIGHT);
		mask = _mm_and_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_sub_ps(positionsX, mask);

		mask = _mm_cmplt_ps(positionsY, c_UP);
		mask = _mm_and_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_add_ps(positionsY, mask);

		mask = _mm_cmpgt_ps(positionsY, c_DOWN);
		mask = _mm_and_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_sub_ps(positionsY, mask);

		// Store information
		_mm_store_ps(bulletPositions->x + i, positionsX);
		_mm_store_ps(bulletPositions->y + i, positionsY);

	}

	//
	// Ship Physics
	//

	// Update velocities and positions for all of the ships
	// Also detect ship collisions against asteroids.
	memset(shipCollisions->value, 0, sizeof(float) * MAX_SHIPS);

	__m128 maxVel = _mm_set1_ps(MAX_SHIP_SPEED);
	__m128 minVel = _mm_set1_ps(-MAX_SHIP_SPEED);
	__m128 shipR = _mm_set1_ps(SHIP_RADIUS);

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
		velocitiesX = _mm_max_ps(velocitiesX, minVel);

		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		velocitiesY = _mm_min_ps(velocitiesY, maxVel);
		velocitiesY = _mm_max_ps(velocitiesY, minVel);

		// Store the velocity ahead so we can multiply it by dt instead of making another variable for that.
		_mm_store_ps(shipVelocities->x + i, velocitiesX);
		_mm_store_ps(shipVelocities->y + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);


		// Shift things if they have gone beyond the bounds.
		__m128 mask;
		mask = _mm_cmplt_ps(positionsX, c_LEFT);
		mask = _mm_and_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_add_ps(positionsX, mask);

		mask = _mm_cmpgt_ps(positionsX, c_RIGHT);
		mask = _mm_and_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_sub_ps(positionsX, mask);

		mask = _mm_cmplt_ps(positionsY, c_UP);
		mask = _mm_and_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_add_ps(positionsY, mask);

		mask = _mm_cmpgt_ps(positionsY, c_DOWN);
		mask = _mm_and_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_sub_ps(positionsY, mask);

		_mm_store_ps(shipPositions->x + i, positionsX);
		_mm_store_ps(shipPositions->y + i, positionsY);

		// Do collisions detection
		// (x1 - x2)^2 + (y1-y2)^2 < (radius + shipRadius)^2

		for (int j = 0; j < MAX_ASTEROIDS; j++){
			__m128 asteroidR = _mm_load1_ps(asteroidRadius->value + j);
			
			asteroidR = _mm_add_ps(asteroidR, shipR);
			asteroidR = _mm_mul_ps(asteroidR, asteroidR);
			
			
			__m128 dx = _mm_sub_ps(positionsX, _mm_load1_ps(asteroidPositions->x + j));// x1-x2
			__m128 dy = _mm_sub_ps(positionsY, _mm_load1_ps(asteroidPositions->y + j));// y1-y2
			dx = _mm_mul_ps(dx, dx);//^2
			dy = _mm_mul_ps(dy, dy);//^2

			__m128 result = _mm_add_ps(dx, dy);
			result = _mm_cmplt_ps(result, asteroidR);

			_mm_store_ps(shipCollisions->value + i, result);
		}

		__m128 rotPos = _mm_load_ps(shipRot->x + i);
		__m128 rotVel = _mm_mul_ps(_mm_load_ps(shipRot->y + i), dt);

		rotPos = _mm_add_ps(rotPos, rotVel);		

		mask = _mm_cmplt_ps(rotPos, c_ROT_MIN);
		mask = _mm_and_ps(mask, c_ROT_MAX);
		rotPos = _mm_add_ps(rotPos, mask);

		mask = _mm_cmpgt_ps(rotPos, c_ROT_MAX);
		mask = _mm_and_ps(mask, c_ROT_MAX);
		rotPos = _mm_sub_ps(rotPos, mask);

		_mm_store_ps(shipRot->x, rotPos);
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
		velocitiesX = _mm_max_ps(velocitiesX, minVel);

		velocitiesY = _mm_add_ps(accelerationsY, velocitiesY);
		velocitiesY = _mm_min_ps(velocitiesY, maxVel);
		velocitiesY = _mm_max_ps(velocitiesY, minVel);

		// Store the velocity ahead so we can multiply it by dt instead of making another variable for that.
		_mm_store_ps(lightVelocities->x + i, velocitiesX);
		_mm_store_ps(lightVelocities->y + i, velocitiesY);

		velocitiesX = _mm_mul_ps(dt, velocitiesX);
		velocitiesY = _mm_mul_ps(dt, velocitiesY);

		positionsX = _mm_add_ps(velocitiesX, positionsX);
		positionsY = _mm_add_ps(velocitiesY, positionsY);

		// Shift things if they have gone beyond the bounds.
		__m128 mask;
		mask = _mm_cmplt_ps(positionsX, c_LEFT);
		mask = _mm_mul_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_add_ps(positionsX, mask);

		mask = _mm_cmpgt_ps(positionsX, c_RIGHT);
		mask = _mm_mul_ps(mask, c_WIDTH_SHIFT);
		positionsX = _mm_sub_ps(positionsX, mask);

		mask = _mm_cmplt_ps(positionsY, c_UP);
		mask = _mm_mul_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_add_ps(positionsY, mask);

		mask = _mm_cmpgt_ps(positionsY, c_DOWN);
		mask = _mm_mul_ps(mask, c_HEIGHT_SHIFT);
		positionsY = _mm_sub_ps(positionsY, mask);

		_mm_store_ps(lightPositions->x + i, positionsX);
		_mm_store_ps(lightPositions->y + i, positionsY);

		__m128 rotPos = _mm_load_ps(lightRot->x + i);
		__m128 rotVel = _mm_mul_ps(_mm_load_ps(lightRot->y + i), dt);

		rotPos = _mm_add_ps(rotPos, rotVel);

		mask = _mm_cmplt_ps(rotPos, c_ROT_MIN);
		mask = _mm_and_ps(mask, c_ROT_MAX);
		rotPos = _mm_add_ps(rotPos, mask);

		mask = _mm_cmpgt_ps(rotPos, c_ROT_MAX);
		mask = _mm_and_ps(mask, c_ROT_MAX);
		rotPos = _mm_sub_ps(rotPos, mask);

		_mm_store_ps(lightRot->x, rotPos);
	}

	// Updating collision

	// For now we just kill for collisions.
	for (int i = 0; i < MAX_SHIPS; i+=4){
		shipsAlive[i] = !shipCollisions->value[i];  // I can unroll the loop thanks to it being four based
		shipsAlive[i + 1] = !shipCollisions->value[i + 1];
		shipsAlive[i + 2] = !shipCollisions->value[i + 2];
		shipsAlive[i + 3] = !shipCollisions->value[i + 3];
	}

	physicsMutex.unlock();
}


void Game::FireBullet(float x, float y, float xVel, float yVel){

	physicsMutex.lock();

	bulletPositions->x[bulletLowIndex] = x;
	bulletPositions->y[bulletLowIndex] = y;
	bulletPrevPositions->x[bulletLowIndex] = x;
	bulletPrevPositions->y[bulletLowIndex] = y;
	bulletVelocities->x[bulletLowIndex] = xVel;
	bulletVelocities->y[bulletLowIndex] = yVel;

	bulletsActive[bulletLowIndex] = true;

	for (int i = bulletLowIndex; i < MAX_BULLETS; ++i){
		if (!bulletsActive[i]){
			bulletLowIndex = i;
			if (i > bulletHighIndex){
				bulletHighIndex = i;
			}
			break;
		}
	}

	physicsMutex.unlock();

}

Vec2* Game::GetAsteroidPos()
{
	return asteroidPositions;
}

Vec1* Game::GetAsteroidRadius()
{
	return asteroidRadius;
}

Vec2* Game::GetShipPos()
{
	return shipPositions;
}

Vec2* Game::GetShipRot()
{
	return shipRot;
}

Vec2* Game::GetLightPos()
{
	return lightPositions;
}

Vec2* Game::GetLightRot()
{
	return lightRot;
}

Vec2* Game::GetBulletPos()
{
	return bulletPositions;
}