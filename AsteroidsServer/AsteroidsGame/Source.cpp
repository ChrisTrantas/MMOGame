#include "Game.h"

int main(){
	Game myGame = Game();
	for (int i = 0; i < 1000; ++i){
		myGame.Update(1.0f);
	}
	return 0;
}