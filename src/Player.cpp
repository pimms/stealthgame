#include "Player.h"
#include "PlayerUpdateComponent.h"
#include "MovePlayer.h"
#include "net/Socket.h"
#include "LightSource.h"
#include "GameScene.h"
#include "FireComponent.h"
#include "MovePlayer.h"
#include "Hack.h"
#include "NightVisionComponent.h"


/*
================
Player Public
================
*/
Player::Player(b2World *world, Team team, unsigned playerID, string texture)  
	:	Entity(world),
		_team(team),
		_playerID(playerID),
		_lightSource(NULL)
{
	LoadTexture(texture);

	int scale = GameScene::GetMapScalingFactor();
	if (team == TEAM_MERC) {
		Position() = Vec2(37.f * scale, 35.f * scale);
	} else {
		Position() = Vec2(67.f * scale, 35.f * scale);
	}

	CreateSquareBody();

	AddComponent<PlayerUpdateComponent>(this);
	_updateComponent = GetComponent<PlayerUpdateComponent>(this);

	if (team == TEAM_MERC) {
		ConeLightSource *light = new ConeLightSource();
		light->SetLightDistance(350.f);
		light->SetColor(Color(1.f, 1.f, 1.f, 1.f));
		AddChild(light);
		GameScene::Singleton()->GetShadowLayer()->AddLightSource(light);
	}
}


unsigned Player::GetPlayerID() const
{
	return _playerID;
}


Team Player::GetTeam() const
{
	return _team;
}


void Player::Update(const DeltaTime &dt)
{
	Entity::Update(dt);

	if (_lightSource) {
		_lightSource->Rotation() = Rotation();
	}
}

void Player::DeductHP() {
	hp--;
}


/*
================
LocalPlayer Public
================
*/
LocalPlayer::LocalPlayer(b2World *world, Team team, unsigned playerID, Socket *udpSocket)
	:	Player(world, team, playerID),
		_udpSocket(udpSocket)
{
	AddComponent<MovePlayer>(this);
	_updateComponent->SetUDPSocket(_udpSocket);

	if (team == TEAM_MERC) {
		AddComponent<FireComponent>(this);
		GetComponent<FireComponent>(this)->SetUDPSocket(udpSocket);
	} else if (team == TEAM_SPY) {
		AddComponent<Hacker>(this);
		AddComponent<NightVisionComponent>(this);
	}
}


void LocalPlayer::Update(const DeltaTime &dt)
{
	Player::Update(dt);
	_updateComponent->SendUpdatePacket();
}

/*
================
RemotePlayer Public
================
*/
RemotePlayer::RemotePlayer(b2World *world, Team team, unsigned playerID)
	:	Player(world, team, playerID)
{
	_body->SetType(b2_staticBody);
}


void RemotePlayer::Update(const DeltaTime &dt)
{
	Player::Update(dt);
}

void RemotePlayer::HandleUpdatePacket(const PacketPlayerUpdate *packet)
{
	_updateComponent->HandleUpdatePacket(packet);
}
