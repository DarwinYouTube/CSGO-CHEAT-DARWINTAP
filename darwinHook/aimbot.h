#pragma once
#include "Player.h"
#include "Localplayer.h"
Player* GetClosestEnemy()
{
	LocalPlayer* localPlayer = LocalPlayer::Get();

	float closestDistance = 1000000;
	int closestDistanceIndex = -1;

	for (int i = 1; i < *Player::GetMaxPlayer(); i++)
	{
		Player* currentPlayer = Player::GetPlayer(i);

		if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
		{
			continue;
		}
		if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
		{
			continue;
		}
		if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
		{
			continue;
		}
		float currentDistance = localPlayer->GetDistance(currentPlayer->GetOrigin());
		if (currentDistance < closestDistance)
		{
			closestDistance = currentDistance;
			closestDistanceIndex = i;
		}
	}
	if (closestDistanceIndex == -1)
	{
		return NULL;
	}
	return Player::GetPlayer(closestDistanceIndex);
}

void Run()
{
	Player* closestEnemy = GetClosestEnemy();
	if (closestEnemy)
	{
		LocalPlayer::Get()->AimAt(closestEnemy->GetBonePos(8));
	}
}