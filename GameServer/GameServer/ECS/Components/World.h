#pragma once
#include <vector>
#include "../GameServer/VectorMath.h"

template <typename T>
class dynamic_2d_array
{
public:
	dynamic_2d_array() {};
	dynamic_2d_array(int rows, int cols)
	{
		for (int i = 0; i < rows; ++i)
		{
			m_Data.push_back(std::vector<T>(cols));
		}
	}

	// other ctors ....

	inline std::vector<T>& operator[](int i) { return m_Data[i]; }

	inline const std::vector<T>& operator[] (int i) const { return m_Data[i]; }

	// other accessors, like at() ...

	void resize(int rows, int cols)
	{
		m_Data.resize(rows);
		for (int i = 0; i < rows; ++i)
			m_Data[i].resize(cols);
	}

	// other member functions, like reserve()....

private:
	std::vector<std::vector<T> > m_Data;
};

struct Tile
{
	enum class Type
	{
		Default,
		Grass,
		Dirt,
		Stone,
		Water
	};

	Type m_Type;
	Vector3 m_Position;
	//std::vector<CGameObject*> m_Content;
};

struct World
{
	uint16_t m_Id;
	int width;
	int height;
	dynamic_2d_array<Tile> tiles;

	bool IsInBounds(const Vector3& position)
	{
		return IsInBounds((int)position.x, (int)position.z);
	}

	bool IsInBounds(int x, int y)
	{
		return x >= 0 && y >= 0 && x < width && y < height;
	}
};