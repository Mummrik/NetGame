#pragma once
#include <cmath>
#include <ostream>

struct Vector2
{
	float x;
	float y;

	float Magnitude() { return std::sqrt(x * x + y * y); }
	float Distance(const Vector2& other) { float x = this->x - other.x; float y = this->y - other.y; return std::sqrt((x * x) + (y * y)); }
	Vector2 Normalize() { float r = 1 / Magnitude(); return Vector2{ x * r, y * r }; }
	Vector2 operator+(const Vector2& rhs) { return Vector2{ x + rhs.x, y + rhs.y }; }
	Vector2 operator-(const Vector2& rhs) { return Vector2{ x - rhs.x, y - rhs.y }; }
	Vector2 operator*(float value) { return Vector2{ x * value, y * value }; }
	Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
	Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	Vector2& operator*=(float value) { x = x * value; y = y * value; return *this; }
	bool operator!=(const Vector2& rhs) { return x != rhs.x || y != rhs.y; }
	bool operator==(const Vector2& rhs) { return x == rhs.x && y == rhs.y; }
	friend std::ostream& operator<<(std::ostream& output, const Vector2& vector) { output << "<" << vector.x << ", " << vector.y << ">"; return output; }
};

struct Vector2Int
{
	int x;
	int y;

	float Magnitude() { return std::sqrtf((float)(x * x) + (float)(y * y)); }
	float Distance(const Vector2Int& other) { int x = this->x - other.x; int y = this->y - other.y; return std::sqrtf((float)(x * x) + (float)(y * y)); }
	Vector2Int Normalize() { float r = 1 / Magnitude(); return Vector2Int{ (int)(x * r), (int)(y * r) }; }
	Vector2Int operator+(const Vector2Int& rhs) { return Vector2Int{ x + rhs.x, y + rhs.y }; }
	Vector2Int operator-(const Vector2Int& rhs) { return Vector2Int{ x - rhs.x, y - rhs.y }; }
	Vector2Int operator*(float value) { return Vector2Int{ (int)(x * value), (int)(y * value) }; }
	Vector2Int operator*(int value) { return Vector2Int{ x * value, y * value }; }
	Vector2Int& operator+=(const Vector2Int& rhs) { x += rhs.x; y += rhs.y; return *this; }
	Vector2Int& operator-=(const Vector2Int& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	Vector2Int& operator*=(float value) { x = (int)(x * value); y = (int)(y * value); return *this; }
	Vector2Int& operator*=(int value) { x = x * value; y = y * value; return *this; }
	bool operator!=(const Vector2Int& rhs) { return x != rhs.x || y != rhs.y; }
	bool operator==(const Vector2Int& rhs) { return x == rhs.x && y == rhs.y; }
	friend std::ostream& operator<<(std::ostream& output, const Vector2Int& vector) { output << "<" << vector.x << ", " << vector.y << ">"; return output; }
};

struct Vector3
{
	float x;
	float y;
	float z;

	float Magnitude() { return std::sqrt(x * x + y * y + z * z); }
	float Distance(const Vector3& other) { float x = this->x - other.x; float y = this->y - other.y; float z = this->z - other.z; return std::sqrt((x * x) + (y * y) + (z * z)); }
	Vector3 Normalize() { float r = 1 / Magnitude(); return Vector3{ x * r, y * r, z * r }; }
	Vector3 operator+(const Vector3& rhs) { return Vector3{ x + rhs.x, y + rhs.y, z + rhs.z }; }
	Vector3 operator-(const Vector3& rhs) { return Vector3{ x - rhs.x, y - rhs.y, z - rhs.z }; }
	Vector3 operator*(float value) { return Vector3{ x * value, y * value, z * value }; }
	Vector3& operator+=(const Vector3& rhs) { x += rhs.x; y += rhs.y;  z += rhs.z; return *this; }
	Vector3& operator-=(const Vector3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3& operator*=(float value) { x = x * value; y = y * value; z = z * value; return *this; }
	bool operator!=(const Vector3& rhs) { return x != rhs.x || y != rhs.y || z != rhs.z; }
	bool operator==(const Vector3& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
	friend std::ostream& operator<<(std::ostream& output, const Vector3& vector) { output << "<" << vector.x << ", " << vector.y << ", " << vector.z << ">"; return output; }
	Vector2Int ToVector2Int() { return Vector2Int{ (int)floor(x), (int)floor(z) }; }
};

struct Vector3Int
{
	int x;
	int y;
	int z;

	float Magnitude() { return std::sqrtf((float)x * (float)x + (float)y * (float)y + (float)z * (float)z); }
	float Distance(const Vector3Int& other) { int x = this->x - other.x; int y = this->y - other.y; int z = this->z - other.z; return std::sqrtf((float)(x * x) + (float)(y * y) + (float)(z * z)); }
	Vector3Int Normalize() { float r = 1 / Magnitude(); return Vector3Int{ (int)(x * r), (int)(y * r), (int)(z * r) }; }
	Vector3Int operator+(const Vector3Int& rhs) { return Vector3Int{ x + rhs.x, y + rhs.y, z + rhs.z }; }
	Vector3Int operator-(const Vector3Int& rhs) { return Vector3Int{ x - rhs.x, y - rhs.y, z - rhs.z }; }
	Vector3Int operator*(float value) { return Vector3Int{ (int)(x * value), (int)(y * value), (int)(z * value) }; }
	Vector3Int operator*(int value) { return Vector3Int{ x * value, y * value, z * value }; }
	Vector3Int& operator+=(const Vector3Int& rhs) { x += rhs.x; y += rhs.y;  z += rhs.z; return *this; }
	Vector3Int& operator-=(const Vector3Int& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3Int& operator*=(float value) { x = (int)(x * value); y = (int)(y * value); z = (int)(z * value); return *this; }
	Vector3Int& operator*=(int value) { x = x * value; y = y * value; z = z * value; return *this; }
	bool operator!=(const Vector3Int& rhs) { return x != rhs.x || y != rhs.y || z != rhs.z; }
	bool operator==(const Vector3Int& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
	friend std::ostream& operator<<(std::ostream& output, const Vector3Int& vector) { output << "<" << vector.x << ", " << vector.y << ", " << vector.z << ">"; return output; }
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;

	Quaternion operator+(const Quaternion& rhs) { return Quaternion{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
	Quaternion operator-(const Quaternion& rhs) { return Quaternion{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }
	Quaternion operator*(float value) { return Quaternion{ x * value, y * value, z * value, w * value }; }
	Quaternion& operator+=(const Quaternion& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	Quaternion& operator-=(const Quaternion& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
	Quaternion& operator*=(float value) { x = x * value; y = y * value; z = z * value; w = w * value; return *this; }
	bool operator!=(const Quaternion& rhs) { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }
	bool operator==(const Quaternion& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
	friend std::ostream& operator<<(std::ostream& output, const Quaternion& quaternion) { output << "<" << quaternion.x << ", " << quaternion.y << ", " << quaternion.z << ", " << quaternion.w << ">"; return output; }
};