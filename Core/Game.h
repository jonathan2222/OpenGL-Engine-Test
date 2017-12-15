#ifndef GAME_H
#define GAME_H

#include "Display.h"
#include "../Player/Camera.h"

class Game
{
public:
	Game();
	virtual ~Game();

private:
	void gameLoop();

private:
	Camera* camera;
	Display* display;

};

#endif