#pragma once

#include <vector>

class Heightmap
{

private:

	std::vector<std::vector<float>> m_data;

public:

	Heightmap() = default;

	Heightmap(int width, int height);

	Heightmap(const Heightmap& other) = default;

	Heightmap(Heightmap&& other) = default;

	Heightmap& operator=(const Heightmap& other) = default;

	Heightmap& operator=(Heightmap&& other) = default;

	~Heightmap() = default;

	void setSize(int width, int height);

	void normalize();

	float& at(int row, int col);

	const float& at(int row, int col) const;

	int getWidth() const;

	int getHeight() const;

	float getMin() const;

	float getMax() const;

	bool isEmpty() const;
};