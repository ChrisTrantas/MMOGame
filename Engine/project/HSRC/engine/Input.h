#pragma once

#include <string>
#include "Common.h"
#include <map>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

using namespace std;

namespace Input
{
	void bindToControl(string name, DWORD input);
	bool isControlDown(string name);
	bool isControlUp(string name);
	bool wasControlPressed(string name);
	bool wasControlReleased(string name);
	void updateControlStates();
	void OnMouseDown(DWORD btn, int x, int y, HWND hMainWnd);
	void OnMouseUp(DWORD btn, int x, int y);
	void OnMouseMove(DWORD btn, int x, int y);
	void OnKeyDown(WPARAM btn);
	void OnKeyUp(WPARAM btn);
	glm::vec2 mouseOffset();
}