#include "Hack.h"

/*
=========
Hackable
=========
*/
Hackable::Hackable() {}

void Hackable::Update(const DeltaTime &dt) 
{
	if (_hacking)
	{
		_hacktime -= dt.dt;
		printf("%f\n", _hacktime);
	}

	if (_hacktime <= 0) 
	{
		_hackdone = true;
		_hacking = false;
	}

	if (_hackinter)
	{
		_hacking = false;
		_resettime -= dt.dt;
	}

	if (_resettime <= 0)
	{
		_hacktime = HACKTIME;
	}

	printf("Position: %f, %f\n", Position().x, Position().y);
}

void Hackable::StartHack() {
	_hacking = true;
}

void Hackable::HackInterrupt() {
	_hackinter = true;
}

bool Hackable::GetCurHacking() {
	return _hacking;
}

bool Hackable::HackIsFinished() {
	return _hackdone;
}

Vec2 Hackable::GetTmPosition() {
	return Position();
}

/*
============
Hacker
============
*/
Hacker::Hacker() {}

void Hacker::Update(const DeltaTime &dt) {
	const InputState *in = GetGameObject()->GetInputState();
	GameScene *scene = (GameScene*)GetGameObject()->GetScene();
	vector<RemotePlayer*> rp = scene->GetRemotePlayers();

	vector<Vec2> positions = GetPositions(rp);
	Vec2 tmpos = hackable.GetTmPosition();
	printf("Spy position: %f, %f\n", Position().x, Position().y);

	if (hackable.HackIsFinished())
	{
		hacked = true;
	}

	if (PlayerInPosition(positions, tmpos))
	{
		printf("Press 'H'\n");
	}

	if (PlayerInPosition(positions, tmpos) 
		&& in->IsKeyDown(SDLK_h) && !hacked)
	{
		hackable.StartHack();
	}

	if (!PlayerInHackingArea(positions, tmpos) && hackable.GetCurHacking() && !hacked)
	{
		hackable.HackInterrupt();
	}

	printf("Spy position: %f, %f\n", Position().x, Position().x);
}

vector<Vec2> Hacker::GetPositions(vector<RemotePlayer*> rp) 
{
	vector<Vec2> pos;
	pos.push_back(Position());

	for (int i = 1; i < rp.size(); i++)
	{
		if (rp[i])
		{
			pos.push_back(rp[i]->Position());
		}
	}

	return pos;
}

bool Hacker::PlayerInPosition(vector<Vec2> pos, Vec2 tmpos)
{
	/*
	for (int i = 0; i < pos.size(); i++)
	{
		if ((pos[i].x >= (tmpos.x - HACK_RADIUS)) &&
			(pos[i].x <= (tmpos.x + HACK_RADIUS)) &&
			(pos[i].y >= (tmpos.y - HACK_RADIUS)) &&
			(pos[i].y <= (tmpos.y + HACK_RADIUS)))
		{
			return true;
		} else {
			return false;
		}
	}
	*/
	if ((Position().x >= (tmpos.x - HACK_RADIUS)) &&
		(Position().x <= (tmpos.x + HACK_RADIUS)) &&
		(Position().y >= (tmpos.y - HACK_RADIUS)) &&
		(Position().y <= (tmpos.y + HACK_RADIUS)))
	{
		printf("Hallo alle sammen som skal være hjemme i helgen!\n");
		return true;
	} else {
		return false;
	}

}

bool Hacker::PlayerInHackingArea(vector<Vec2> pos, Vec2 tmpos)
{
	for (int i = 0; i < pos.size(); i++)
	{
		if ((pos[i].x >= (tmpos.x - AREA_RADIUS)) &&
			(pos[i].x <= (tmpos.x + AREA_RADIUS)) &&
			(pos[i].y >= (tmpos.y - AREA_RADIUS)) &&
			(pos[i].y <= (tmpos.y + AREA_RADIUS)))
		{
			return true;
		} else {
			return false;
		}
	}
}