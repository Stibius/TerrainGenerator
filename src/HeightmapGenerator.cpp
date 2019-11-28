
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

#include "HeightmapGenerator.h"

std::default_random_engine HeightmapGenerator::m_randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

Heightmap HeightmapGenerator::generateDiamond(int iterations, int startAmplitude, float amplitudeModifier)
{
    /*
    * viz. https://code.google.com/p/fractalterraingeneration/wiki/Diamond_Square
    *      https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
    */

	Heightmap heightmap;

    if (iterations < 1)
    {
		return heightmap;
    }

    int rows = (1 << iterations) + 1;
    int cols = rows;

	heightmap.setSize(cols, rows);

    int d = rows; //distance of the center of the square from border

    int amplitude = startAmplitude;

    for (int it = 1; it <= iterations; ++it) //iterations
    {
        d /= 2;

        //diamond step

        for (int row = d; row < rows - d; row += 2 * d)
        {
            for (int col = d; col < cols - d; col += 2 * d)
            {
                float sum = 0;

                for (int row2 = row - d; row2 <= row + d; row2 += 2 * d)
                {
                    for (int col2 = col - d; col2 <= col + d; col2 += 2 * d)
                    {
                        sum += heightmap.at(row2, col2);
                    }
                }

                float average = sum / 4;

				std::uniform_int_distribution<int> distribution(-amplitude, amplitude);
                int displacement = distribution(m_randomEngine);

				heightmap.at(row, col) = average + displacement;
            }
        }

        //square step

        for (int row = d; row < rows - d; row += 2 * d)
        {
            for (int col = d; col < cols - d; col += 2 * d)
            {
                int translations[4][2] =
                {
                    {  0,  -d },
                    {  0,   d },
                    { -d,   0 },
                    {  d,   0 }
                };

                for (int neighbor = 0; neighbor < 4; ++neighbor)
                {
                    float sum = 0;

                    int row2 = row + translations[neighbor][0];
                    int col2 = col + translations[neighbor][1];

                    for (int neighbor2 = 0; neighbor2 < 4; ++neighbor2)
                    {
                        int row3 = row2 + translations[neighbor2][0];
                        int col3 = col2 + translations[neighbor2][1];

                        if (row3 < 0)
                            row3 = rows + row3;
                        else if (row3 > rows - 1)
                            row3 = row3 - rows;

                        if (col3 < 0)
                            col3 = cols + col3;
                        else if (col3 > cols - 1)
                            col3 = col3 - cols;

                        sum += heightmap.at(row3, col3);
                    }

                    float average = sum / 4.0f;

					std::uniform_int_distribution<int> distribution(-amplitude, amplitude);
					int displacement = distribution(m_randomEngine);

					heightmap.at(row2, col2) = average + displacement;
                }
            }
        }

        amplitude = static_cast<int>(amplitude * amplitudeModifier);

        if (amplitude == 0)
        {
            amplitude = 1;
        }
    }

	heightmap.normalize();
    
    return heightmap;
}

Heightmap HeightmapGenerator::generateCircles(int width, int height, int minRadius, int maxRadius, int minAmplitude, int maxAmplitude, int iterations)
{
    /*
    * viz. http://www.lighthouse3d.com/opengl/terrain/index.php3?circles
    */

	Heightmap heightmap;

    if (iterations < 1 || width < 1 || height < 1 || minRadius > maxRadius || minRadius <= 0 || minAmplitude > maxAmplitude)
    {
		return heightmap;
    }

	heightmap.setSize(width, height);

    for (int it = 1; it <= iterations; ++it)
    {
		std::uniform_int_distribution<int> distribution1(0, width - 1);
		int centerX = distribution1(m_randomEngine);

		std::uniform_int_distribution<int> distribution2(0, height - 1);
		int centerY = distribution2(m_randomEngine);

		std::uniform_int_distribution<int> distribution3(minRadius, maxRadius);
		int radius = distribution3(m_randomEngine);

		std::uniform_int_distribution<int> distribution4(minAmplitude, maxAmplitude);
		int amplitude = distribution4(m_randomEngine);

        for (int x = centerX - radius; x <= centerX + radius; ++x)
        {
            if (x < 0 || x >= width) continue;

            for (int y = centerY - radius; y <= centerY + radius; ++y)
            {
                if (y < 0 || y >= height) continue;

				float distance = sqrt(pow(x - centerX, 2.0f) + pow(y - centerY, 2.0f));

                if (distance <= radius)
                {
                    heightmap.at(y, x) += sin((1 - (distance/radius)) * (PI/2)) * amplitude;
                }
            }
        }
    }

	heightmap.normalize();

	return heightmap;
}

Heightmap HeightmapGenerator::generatePerlin(int size, int octaves, float amplitudeModifier, float frequencyModifier)
{
    /*
    * viz. http://flafla2.github.io/2014/08/09/perlinnoise.html
    *      https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
    */

	Heightmap heightmap;

    if (size < 1 || octaves < 1)
    {
		return heightmap;
    }

	heightmap.setSize(size, size);

    float gradients[GRADIENT_COUNT][2];

    for (int i = 0; i < GRADIENT_COUNT; ++i)
    {
        float angle = ((2 * PI) / GRADIENT_COUNT) * i;

        gradients[i][0] = cos(angle);
        gradients[i][1] = sin(angle);
    }

	std::vector<int> permutations;
	permutations.reserve(PERMUTATION_COUNT);
    for (int i = 0; i < PERMUTATION_COUNT; ++i)
    {
        permutations.push_back(i);
    }

    std::shuffle(permutations.begin(), permutations.end(), m_randomEngine);

    float amplitude = 255.0f; //initial amplitude
    float frequency = 1.0f / size; //initial frequency

    for (int i = 0; i < octaves; ++i)
    {
        for (int row = 0; row < size; ++row)
        {
            for (int col = 0; col < size; ++col)
            {
                float x = col * frequency;
                float y = row * frequency;

                int x0 = static_cast<int>(floor(x));
                int y0 = static_cast<int>(floor(y));
                int x1 = static_cast<int>(ceil(x));
                int y1 = static_cast<int>(ceil(y));

                float gsx = gradients[permutations[(x0 + permutations[y0 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][0];
                float gsy = gradients[permutations[(x0 + permutations[y0 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][1];
                float gtx = gradients[permutations[(x1 + permutations[y0 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][0];
                float gty = gradients[permutations[(x1 + permutations[y0 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][1];
                float gux = gradients[permutations[(x0 + permutations[y1 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][0];
                float guy = gradients[permutations[(x0 + permutations[y1 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][1];
                float gvx = gradients[permutations[(x1 + permutations[y1 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][0];
                float gvy = gradients[permutations[(x1 + permutations[y1 % PERMUTATION_COUNT]) % PERMUTATION_COUNT] % GRADIENT_COUNT][1];

				float s = gsx * (x - x0) + gsy * (y - y0);
				float t = gtx * (x - x1) + gty * (y - y0);
				float u = gux * (x - x0) + guy * (y - y1);
				float v = gvx * (x - x1) + gvy * (y - y1);

                float Sx = (3 * pow(x - x0, 2.0f) - (2 * pow(x - x0, 3.0f)));
                float Sy = (3 * pow(y - y0, 2.0f) - (2 * pow(y - y0, 3.0f)));

                float a = s + (Sx * (t - s));
                float b = u + (Sx * (v - u));
                float z = a + (Sy * (b - a));

                heightmap.at(row, col) += z * amplitude;
            }
        }

        amplitude *= amplitudeModifier;
        frequency *= frequencyModifier;
    }

	heightmap.normalize();

	return heightmap;
}

Heightmap HeightmapGenerator::generateFault(int width, int height, int iterations, int startAmplitude, int endAmplitude, int amplitudeChange, faultFunctions_t function, int transitionLength)
{
    /*
    * viz. http://www.lighthouse3d.com/opengl/terrain/index.php3?fault
    */

	Heightmap heightmap;

    if (width < 1 || height < 1 || iterations < 1 || startAmplitude < 0 || endAmplitude < 0 || amplitudeChange < 0 || transitionLength < 0)
    {
		return heightmap;
    }

	heightmap.setSize(width, height);

    int amplitude = startAmplitude;
    float displacement = amplitude / 2.0f;

    for (int i = 0; i < iterations; ++i)
    {
        //two random points which determine the dividing line
		std::uniform_int_distribution<int> distribution1(0, width);
		int x1 = distribution1(m_randomEngine);
		int x2 = distribution1(m_randomEngine);

		std::uniform_int_distribution<int> distribution2(0, height);
		int y1 = distribution2(m_randomEngine);
		int y2 = distribution2(m_randomEngine);

        int a = y2 - y1;
        int b = x1 - x2;
        int c = - a*x1 - b*y1;
        float s = sqrt(pow(a, 2.0f) + pow(b, 2.0f));

        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                float distance = ((a * x) + (b * y) + c) / s; //distance of point from line, viz. http://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line

                if (distance > transitionLength / 2)
                {
                    if (function != faultFunctions_t::COS) heightmap.at(y, x) += displacement;
                }
                else if ((distance < -transitionLength / 2 || transitionLength == 0))
                {
                    if (function != faultFunctions_t::COS) heightmap.at(y, x) -= displacement;
                }
                else
                {
                    switch (function)
                    {
                    case faultFunctions_t::LINEAR:
						heightmap.at(y, x) += (distance / (transitionLength / 2)) * displacement;
                        break;
                    case faultFunctions_t::SIN:
						heightmap.at(y, x) += sin((distance / (transitionLength / 2)) * (PI / 2)) * displacement;
                        break;
                    case faultFunctions_t::COS:
						heightmap.at(y, x) += (cos((distance / (transitionLength / 2)) * PI) + 1) * displacement;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if (startAmplitude > endAmplitude && amplitude >= endAmplitude + amplitudeChange)
            amplitude -= amplitudeChange;
        else if (startAmplitude < endAmplitude && amplitude <= endAmplitude - amplitudeChange)
            amplitude += amplitudeChange;

        displacement = amplitude / 2.0f;
    }

	heightmap.normalize();

	return heightmap;
}

