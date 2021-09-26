#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define ORIGINAL_G 100000

#ifdef GLOBALS_DEFINITIONS
float G = ORIGINAL_G;
float radius = 20;

int windowWidth;
int windowHeight;

float positionMultiplier;
int positionOffsetX;
int positionOffsetY;

HDC g;
#else
extern float G;
extern float radius;

extern int windowWidth;
extern int windowHeight;

extern float positionMultiplier;
extern int positionOffsetX;
extern int positionOffsetY;

extern HDC g;
#endif

inline void setWindowSize(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	if (width < height) {
		positionOffsetX = 0;
		positionOffsetY = (windowHeight - windowWidth) / 2;
		positionMultiplier = windowWidth / 1000.0f;						// TODO: Is this just as efficient as multiplying by 0.0001?
		return;
	}
	positionOffsetX = (windowWidth - windowHeight) / 2;
	positionOffsetY = 0;
	positionMultiplier = windowHeight / 1000.0f;
}
