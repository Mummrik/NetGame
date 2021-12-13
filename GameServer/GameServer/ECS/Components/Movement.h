#pragma once
#include "../GameServer/VectorMath.h"

struct Movement
{
	Vector2 m_Direction;
	float m_Speed;
	Vector2Int m_TilePosition;
	uint16_t m_WorldId;

	Vector3 GetVelocity() { return Vector3{ m_Direction.x, 0, m_Direction.y } * m_Speed; }
};