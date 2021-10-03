#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "debugOutput.h"

#include <stdlib.h>																																// For EXIT_SUCCESS and EXIT_FAILURE

#include <thread>																																// For graphics thread.

#define GLOBALS_DEFINITIONS																														// Tells this header to define global variables, not just declare them like the other instances.
#include "globals.h"																															// Responsible for global variables that are shared between translation units.

#include "SimScene.h"																															// For scene handling.

#include "FrameManager.h"																														// Evenly spaces frames so there is no variable speeds anywhere.

#define FPS 120

#define KEY_X 0x58																																// Defines for handling keyboard input in windowProc.
#define KEY_R 0x52
#define KEY_T 0x54
#define KEY_G 0x47
#define KEY_H 0x48

int tempWindowWidth;																															// Variables to store client size while in resize loop.
int tempWindowHeight;
bool windowResized = false;																														// Flag to let the graphics thread know when to adjust to new size.

Body twoBodies[] = { Body(250, 500, 0, 10), Body(750, 500, 0, -10) };																			// Set up the two scenes.
SimScene twoBodyScene(twoBodies, 2);
Body threeBodies[] = { Body(100, 500, 0, 10), Body(500, 100, 10, 0), Body(750, 900, 0, 0) };
SimScene threeBodyScene(threeBodies, 3);

bool swapScene = false;																															// Flag to let the graphics thread know when to swap scenes.
bool resetScene = false;
bool isPaused = true;
SimScene scene = twoBodyScene;																													// Current scene. This is the one that will be processed in the graphics loop.

bool isAlive = true;
void graphicsLoop(HWND hWnd);

LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case KEY_X:
			resetScene = true;
			isPaused = false;
			return 0;
		case VK_SPACE:
			swapScene = true;
			isPaused = true;
			return 0;
		case KEY_R:
			radius -= 1;
			return 0;
		case KEY_T:
			radius += 1;
			return 0;
		case KEY_G:
			G -= 10000;
			return 0;
		case KEY_H:
			G += 10000;
			return 0;
		}
		return 0;
	case WM_SIZE:
		switch (wParam) {
		case SIZE_RESTORED:
			tempWindowWidth = LOWORD(lParam);
			tempWindowHeight = HIWORD(lParam);
			return 0;
		case SIZE_MAXIMIZED:																													// TODO: When unmaximizing, the size doesn't automatically update. Not possible with WM_SIZE I think. Find some other way to check for it.
			setWindowSize(LOWORD(lParam), HIWORD(lParam));
			windowResized = true;
			return 0;
		}
		return 0;
	case WM_EXITSIZEMOVE:
		setWindowSize(tempWindowWidth, tempWindowHeight);
		windowResized = true;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#ifdef UNICODE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow) {
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow) {
#endif
	debuglogger::out << "program started" << debuglogger::endl;

	debuglogger::out << "setting up window..." << debuglogger::endl;
	WNDCLASS windowClass = { };																																			// Create WNDCLASS struct for later window creation.
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = TEXT("PRESENTATION_GRAV_SIM_WINDOW");

	if (!RegisterClass(&windowClass)) {
		debuglogger::out << debuglogger::error << "failed to register window class" << debuglogger::endl;
		return EXIT_FAILURE;
	}

	HWND hWnd = CreateWindow(windowClass.lpszClassName, TEXT("Presentation Gravition Sim"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);
	if (hWnd == nullptr) {																																				// Check if creation of window was successful.
		debuglogger::out << debuglogger::error << "couldn't create window" << debuglogger::endl;
		return EXIT_FAILURE;
	}

	debuglogger::out << "showing window..." << debuglogger::endl;
	ShowWindow(hWnd, nCmdShow);

	debuglogger::out << "setting windowWidth, windowHeight and pos transformers..." << debuglogger::endl;
	RECT clientRect;
	if (!GetClientRect(hWnd, &clientRect)) {
		debuglogger::out << debuglogger::error << "failed to get client bounds" << debuglogger::endl;
		return EXIT_FAILURE;
	}
	setWindowSize(clientRect.right, clientRect.bottom);

	debuglogger::out << "starting graphics thread..." << debuglogger::endl;
	std::thread graphicsThread(graphicsLoop, hWnd);

	debuglogger::out << "running message loop..." << debuglogger::endl;
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	debuglogger::out << "joining with graphics thread and exiting..." << debuglogger::endl;
	isAlive = false;
	graphicsThread.join();
}

void graphicsLoop(HWND hWnd) {
	debuglogger::out << "setting up DCs and pens..." << debuglogger::endl;
	HDC finalG = GetDC(hWnd);
	g = CreateCompatibleDC(finalG);
	HBITMAP bmp = CreateCompatibleBitmap(finalG, windowWidth, windowHeight);
	SelectObject(g, bmp);

	HGDIOBJ bgPen = GetStockObject(BLACK_PEN);
	HGDIOBJ bgBrush = GetStockObject(BLACK_BRUSH);

	HPEN bodyPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	HBRUSH bodyBrush = CreateSolidBrush(RGB(0, 255, 0));

	bool sceneSwapState = false;
	FrameManager frameMan(FPS);
	debuglogger::out << "running graphics loop..." << debuglogger::endl;
	while (isAlive) {
		frameMan.start();

		SelectObject(g, bodyPen);																														// Render frame, draw frame, update scene and clear frame for next draw.
		SelectObject(g, bodyBrush);
		scene.render();
		BitBlt(finalG, 0, 0, windowWidth, windowHeight, g, 0, 0, SRCCOPY);
		if (!isPaused) { scene.update(); }
		SelectObject(g, bgPen);
		SelectObject(g, bgBrush);
		Rectangle(g, 0, 0, windowWidth, windowHeight);
		
		if (windowResized) {																															// If window has been resized, recreate bitmap to give g new size. Also reset current scene.
			DeleteObject(bmp);
			bmp = CreateCompatibleBitmap(finalG, windowWidth, windowHeight);
			SelectObject(g, bmp);
			scene.reset();
			windowResized = false;
			continue;
		}

		if (swapScene) {																																// If scene should be swapped, swap scene.
			G = ORIGINAL_G;
			if (sceneSwapState) {
				scene = twoBodyScene;
				scene.reset();
				sceneSwapState = false;
				swapScene = false;
				continue;
			}
			scene = threeBodyScene;
			scene.reset();
			sceneSwapState = true;
			swapScene = false;
			continue;
		}

		if (resetScene) { scene.reset(); resetScene = false; }																							// Reset scene if need be.

		frameMan.delay();
	}

	ReleaseDC(hWnd, finalG);
	DeleteDC(g);																																		// Release all resources before exiting.
	DeleteObject(bmp);
	DeleteObject(bodyPen);
	DeleteObject(bodyBrush);
}