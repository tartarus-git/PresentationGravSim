#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Body {
public:
	float startX;
	float startY;
	float x;
	float y;

	float startVX;
	float startVY;
	float vx;
	float vy;

	Body(float defaultX, float defaultY, float defaultVX, float defaultVY) : startX(defaultX), startY(defaultY), x(defaultX), y(defaultY), startVX(defaultVX), startVY(defaultVY), vx(defaultVX), vy(defaultVY) { }
	
	void applyGravity(Body& other);
	void update();
};

class SimScene
{
	Body* bodies;
	size_t bodyAmount;
	size_t bodiesLastIndex;

public:
	SimScene(Body* bodyArray, size_t numOfBodies) : bodies(bodyArray), bodyAmount(numOfBodies), bodiesLastIndex(numOfBodies - 1) { }

	SimScene& operator=(SimScene& other) {
		bodies = other.bodies;
		bodyAmount = other.bodyAmount;
		bodiesLastIndex = other.bodiesLastIndex;
		return *this;
	}

	void update();
	void render();

	void reset();
};
