#include "FrameManager.h"

#define NANOSECONDS_PER_SECOND 1000000000

FrameManager::FrameManager(int FPS) { nsPerFrame = NANOSECONDS_PER_SECOND / FPS; }

void FrameManager::start() { frameStart = std::chrono::high_resolution_clock::now(); }

std::chrono::high_resolution_clock::duration FrameManager::measure() { return std::chrono::high_resolution_clock::now() - frameStart; }

void FrameManager::delay() {
	long long frameDuration = measure().count();
	if (frameDuration < nsPerFrame) { std::this_thread::sleep_for(std::chrono::nanoseconds(nsPerFrame - frameDuration)); }
}
