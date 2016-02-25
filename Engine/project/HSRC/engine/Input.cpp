#include <algorithm>
#include "Input.h"

struct control
{
	vector<DWORD> bindings;
	bool isDown;
	bool wasDown;
};

static map<string, control> controls;
static glm::vec2 prevMousePos = glm::vec2(-1, -1);
static glm::vec2 mousePos;

namespace Input
{
	void bindToControl(string name, DWORD input)
	{
		auto controlCheck = controls.find(name);
		if (controlCheck == controls.end())
		{
			control newControl = { vector<DWORD>(), false, false };
			newControl.bindings.push_back(input);
			controls.insert(pair<string, control>(name, newControl));
			return;
		}

		auto bindCheck = find(controls[name].bindings.begin(), controls[name].bindings.end(), input);
		if (bindCheck == controls[name].bindings.end())
			controls[name].bindings.push_back(input);
	}

	void updateControlStates()
	{
		auto controlBucket = controls.begin();
		while (controlBucket != controls.end())
		{
			controlBucket->second.wasDown = controlBucket->second.isDown;
			controlBucket++;
		}
		prevMousePos = mousePos;
	}

	bool isControlDown(string name)
	{
		auto controlCheck = controls.find(name);
		if (controlCheck != controls.end())
			return controlCheck->second.isDown;
		return false;
	}

	bool isControlUp(string name)
	{
		auto controlCheck = controls.find(name);
		if (controlCheck != controls.end())
			return !controlCheck->second.isDown;
		return false;
	}

	bool wasControlPressed(string name)
	{
		auto controlCheck = controls.find(name);
		if (controlCheck != controls.end())
			return controlCheck->second.isDown && !controlCheck->second.wasDown;
		return false;
	}

	bool wasControlReleased(string name)
	{
		auto controlCheck = controls.find(name);
		if (controlCheck != controls.end())
			return !controlCheck->second.isDown && controlCheck->second.wasDown;
		return false;
	}

	void OnMouseDown(DWORD btn, int x, int y, HWND hMainWnd)
	{
		mousePos.x = (float)x;
		mousePos.y = (float)y;
		SetCapture(hMainWnd);

		auto controlBucket = controls.begin();
		while (controlBucket != controls.end())
		{
			for (size_t i = 0; i < controlBucket->second.bindings.size(); i += 1)
				if (btn == controlBucket->second.bindings[i])
				{
					controlBucket->second.isDown = true;
					break;
				}
			controlBucket++;
		}
	}

	void OnMouseUp(DWORD btn, int x, int y)
	{
		auto controlBucket = controls.begin();
		while (controlBucket != controls.end())
		{
			for (size_t i = 0; i < controlBucket->second.bindings.size(); i += 1)
				if (btn == controlBucket->second.bindings[i])
				{
					controlBucket->second.isDown = false;
					break;
				}
			controlBucket++;
		}

		ReleaseCapture();
	}

	void OnMouseMove(DWORD btn, int x, int y)
	{
		mousePos.x = (float)x;
		mousePos.y = (float)y;
		if (prevMousePos.x == -1)
			prevMousePos = mousePos;
	}

	void OnKeyDown(WPARAM btn)
	{
		auto controlBucket = controls.begin();
		while (controlBucket != controls.end())
		{
			for (size_t i = 0; i < controlBucket->second.bindings.size(); i += 1)
				if (btn == controlBucket->second.bindings[i])
				{
					controlBucket->second.isDown = true;
					break;
				}
			controlBucket++;
		}
	}

	void OnKeyUp(WPARAM btn)
	{
		auto controlBucket = controls.begin();
		while (controlBucket != controls.end())
		{
			for (size_t i = 0; i < controlBucket->second.bindings.size(); i += 1)
				if (btn == controlBucket->second.bindings[i])
				{
					controlBucket->second.isDown = false;
					break;
				}
			controlBucket++;
		}
	}

	glm::vec2 mouseOffset()
	{
		return glm::vec2(mousePos - prevMousePos);
	}
}