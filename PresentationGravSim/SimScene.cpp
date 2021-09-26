#include "SimScene.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "globals.h"

void Body::applyGravity(Body& other) {
	float width = x - other.x;
	float height = y - other.y;
	float distSquared = width * width + height * height;
	float dist = sqrt(distSquared);
	float accelPercent = G / distSquared / dist;
	float xAccel = accelPercent * width;
	float yAccel = accelPercent * height;
	vx -= xAccel;
	vy -= yAccel;
	other.vx += xAccel;
	other.vy += yAccel;
}

void Body::update() {
	x += vx;
	y += vy;
}

void SimScene::update() {
	for (int i = 0; i < bodiesLastIndex; i++) {
		for (int j = i + 1; j < bodyAmount; j++) {
			bodies[i].applyGravity(bodies[j]);
		}
		bodies[i].update();
	}
	bodies[bodiesLastIndex].update();
}

void SimScene::render() {
	for (int i = 0; i < bodyAmount; i++) {
		int transformedX = bodies[i].x * positionMultiplier + positionOffsetX;
		int transformedY = bodies[i].y * positionMultiplier + positionOffsetY;
		Ellipse(g, transformedX - radius, transformedY - radius, transformedX + radius, transformedY + radius);
	}
}

void SimScene::reset() {
	for (int i = 0; i < bodyAmount; i++) {
		bodies[i].x = bodies[i].startX;
		bodies[i].y = bodies[i].startY;
		bodies[i].vx = bodies[i].startVX;
		bodies[i].vy = bodies[i].startVY;
	}
}
