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

	runSpeed = 0.1f;
	walkSpeed = 0.005f;

	Input::bindToControl("forward", 'W');
	Input::bindToControl("back", 'S');
	Input::bindToControl("left", 'A');
	Input::bindToControl("right", 'D');
	Input::bindToControl("run", VK_SHIFT);
	Input::bindToControl("up", VK_SPACE);
	Input::bindToControl("down", VK_CONTROL);
	Input::bindToControl("look", VK_LBUTTON);
}

void FPController::update(float deltaTime, float totalTime)
{
	vec3 offset = (float)(Input::isControlDown("right") - Input::isControlDown("left")) * ct->right() +
					(float)(Input::isControlDown("up") - Input::isControlDown("down")) * ct->up() +
					(float)(Input::isControlDown("forward") - Input::isControlDown("back")) * ct->forward();

	if (length(offset) != 0)
		offset = normalize(offset);

	float speed = walkSpeed;

	if (Input::isControlDown("run"))
	{
		speed = runSpeed;
	}

	ct->translate(offset * speed);

	if (Input::isControlDown("look"))
	{
		ct->rotate(angleAxis(Input::mouseOffset().x * -0.001f, ct->up()));
		ct->rotate(angleAxis(Input::mouseOffset().y * -0.001f, ct->right()));
	}
}