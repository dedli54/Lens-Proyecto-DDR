#pragma once
#include "Arrow.h"

class gameMap {
public:
	Arrow arrow;
	int actual, prev, antprev;
	void loadMap();
};