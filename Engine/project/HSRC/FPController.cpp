#include "FPController.h"

FPController::FPController()
{

}

FPController::~FPController()
{

}

void FPController::awake()
{
	ct = gameObject->getComponent<Transform>();

	Input::bindToControl("forward", 'W');
	Input::bindToControl("back", 'S');
	Input::bindToControl("left", 'A');
	Input::bindToControl("right", 'D');
	Input::bindToControl("up", VK_SPACE);
	Input::bindToControl("down", VK_CONTROL);
	Input::bindToControl("look", VK_LBUTTON);
}

void FPController::update()
{
	vec3 offset = (float)(Input::isControlDown("right") - Input::isControlDown("left")) * ct->right() +
					(float)(Input::isControlDown("up") - Input::isControlDown("down")) * ct->up() +
					(float)(Input::isControlDown("forward") - Input::isControlDown("back")) * ct->forward();
	if (length(offset) != 0)
		offset = normalize(offset);
	ct->translate(offset * 0.005f);

	if (Input::isControlDown("look"))
	{
		ct->rotate(angleAxis(Input::mouseOffset().x * -0.001f, ct->up()));
		ct->rotate(angleAxis(Input::mouseOffset().y * -0.001f, ct->right()));
	}
}
