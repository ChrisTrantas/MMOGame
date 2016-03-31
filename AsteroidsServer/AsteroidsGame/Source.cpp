#include "Game.h"

#include <ctime>
#include <iostream>

int main(){
	Game myGame = Game();

	clock_t myClock;
	myClock = clock();

	for (int i = 0; i < 20000; ++i){
		myGame.Update(0.1f);
	}
	clock_t second;
	second = clock();
	float dtA = second - myClock;

	std::cout << "Average Update Time: " << (((float)dtA) / CLOCKS_PER_SEC) * 1000 / 20000.0f << "ms" << std::endl;

	int b = 0;
	std::cin >> b;

	return 0;
}