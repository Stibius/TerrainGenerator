
#include <algorithm>
#include <limits>

#include "Heightmap.h"

Heightmap::Heightmap(int width, int height)
{
	setSize(width, height);
}

void Heightmap::setSize(int width, int height)
{
	m_data = std::vector<std::vector<float>>(height, std::vector<float>(width, 0.0f));
}

void Heightmap::normalize()
{
	if (isEmpty())
	{
		return;
	}

	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();

	for (const std::vector<float>& row : m_data)
	{
		for (float value : row)
		{
			min = std::min(min, value);
			max = std::max(max, value);
		}
	}

	float scale = 1.0f / (max - min);

	for (std::vector<float>& row : m_data)
	{
		for (float& value : row)
		{
			value = (value - min) * scale;
		}
	}
}

float& Heightmap::at(int row, int col)
{
	return m_data[row][col];
}

const float& Heightmap::at(int row, int col) const
{
	return m_data[row][col];
}

int Heightmap::getWidth() const
{
	if (m_data.empty())
	{
		return 0;
	}
	else
	{
		return m_data[0].size();
	}
}

int Heightmap::getHeight() const
{
	return m_data.size();
}

float Heightmap::getMin() const
{
	if (isEmpty())
	{
		return 0.0f;
	}

	float min = std::numeric_limits<float>::max();

	for (const std::vector<float>& row : m_data)
	{
		for (float value : row)
		{
			min = std::min(min, value);
		}
	}

	return min;
}

float Heightmap::getMax() const
{
	if (isEmpty())
	{
		return 0.0f;
	}

	float max = std::numeric_limits<float>::min();

	for (const std::vector<float>& row : m_data)
	{
		for (float value : row)
		{
			max = std::max(max, value);
		}
	}

	return max;
}

bool Heightmap::isEmpty() const
{
	return m_data.empty();
}
