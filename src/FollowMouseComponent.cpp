#include "FollowMouseComponent.h"
#include "Entity.h"


void FollowMouseComponent::Update(const DeltaTime &dt)
{
	const InputState *in = GetGameObject()->GetInputState();
	Window *window = GetGameObject()->GetApp()->GetWindow();

	Vec2 mpos = in->GetMousePosition();
	Vec2 spos = ((Entity*)GetGameObject())->WorldPosition();
	Vec2 size = window->GetWindowSize();
	mpos.y = size.y - mpos.y;

	mpos.x /= size.x;
	mpos.y /= size.y;
	spos.x /= 1280.f;
	spos.y /= 720.f;

	Vec2 diff = Vec2(mpos.x-spos.x, mpos.y-spos.y);

	float rad = atan2f(diff.y, diff.x);
	float deg  = Rad2Deg(rad);

	//if (diff.x < 0)
		//deg += 180.f;
	if (diff.y < 0)
		deg += 360.f;

	printf("Rotation: %g\n", deg);

	Rotation() = deg;
}
