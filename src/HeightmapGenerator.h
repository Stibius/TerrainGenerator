#pragma once

#include <random>

#include "Heightmap.h"

class HeightmapGenerator 
{

private:

    static constexpr int GRADIENT_COUNT = 8; //!<number of precomputed gradients for Perlin Noise

    static constexpr int PERMUTATION_COUNT = 256; //!<size of permutation table for Perlin noise

    static constexpr float PI = 3.14159265359f;

	static std::default_random_engine m_randomEngine;

public:

	//!<transition functions for Fault formation algorithm
    enum class faultFunctions_t 
	{ 
		LINEAR, 
		SIN, 
		COS 
	}; 

    /** \brief Generates heightmap using the Diamond-Square algorithm.
    *          Emits generatingFinished() after generating is done.
    *          viz. https://code.google.com/p/fractalterraingeneration/wiki/Diamond_Square a
    *               https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
    *   \param iterations Number of iterations of the algorithm. Has to be >=0.
    *   \param startAmplitude Initial amplitude.
    *   \param amplitudeModifier Amplitude gets multiplied by this value in each iteration.
    *                            The lower the value, the smoother the terrain. Should be 0-1.
    *   \return The generated heightmap, empty heightmap if parameters are wrong.
    */
	static Heightmap generateDiamond(int iterations, int startAmplitude, float amplitudeModifier);

    /** \brief Generates heightmap using the Circles algorithm.
    *          Emits generatingFinished() after generating is done.
    *          viz. http://www.lighthouse3d.com/opengl/terrain/index.php3?circles
    *   \param width Width of the heighmap. Has to be > 0.
    *   \param height Height of the heightmap. Has to be > 0.
    *   \param minRadius Min. radius of generated circles. Has to be > 0 and <= maxRadius.
    *   \param maxRadius Max. radius of generated circles. Has to be >= minRadius.
    *   \param minAmplitude Min. amplitude, can be negative. Has to be <= maxAmplitude.
    *   \param maxAmplitude Max. amplitude, can be negative. Has to be >= minAmplitude.
    *   \param iterations Number of iterations of the algorithm. Has to be >=0.
    *   \return The generated heightmap, empty heightmap if parameters are wrong.
    */
	static Heightmap generateCircles(int width, int height, int minRadius, int maxRadius, int minAmplitude, int maxAmplitude, int iterations);

    /** \brief Generates heightmap using the Perlin Noise algorithm.
    *          Emits generatingFinished() after generating is done.
    *          viz. http://flafla2.github.io/2014/08/09/perlinnoise.html
    *               https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
    *   \param size Size of the square heightmap. Has to be > 0.
    *   \param octaves Number of octaves (iterations) of the algorithm. Has to be >= 0.
    *   \param amplitudeModifier Amplitude gets multiplied by this value in each iteration. Should be 0-1.
    *   \param frequencyModifier Frequency gets multiplied by this value in each iteration. Should be > 1.
    *   \return The generated heightmap, empty heightmap if parameters are wrong.
    */
	static Heightmap generatePerlin(int size, int octaves, float amplitudeModifier, float frequencyModifier);

    /** \brief Generates heightmap using the Fault Formation algorithm.
    *          Emits generatingFinished() after generating is done.
    *          viz. http://www.lighthouse3d.com/opengl/terrain/index.php3?fault
    *   \param width Width of the heighmap. Has to be > 0.
    *   \param height Height of the heighmap. Has to be > 0.
    *   \param iterations Number of iterations of the algorithm. Has to be >=0.
    *   \param startAmplitude Initial amplitude. Has to be >=0.
    *   \param endAmplitude Final amplitude. Has to be >= 0.
    *   \param amplitudeChange Amplitude gets incremented or decremented (depending on the values of startAmplitude and endAmplitude) by this value after each iteration until it's value reaches endAmplitude. Has to be >=0.
    *   \param function Transition function.
    *   \param transitionLength Length of transitions. Has to be >=0.
    *   \return The generated heightmap, empty heightmap if parameters are wrong.
    */
	static Heightmap generateFault(int width, int height, int iterations, int startAmplitude, int endAmplitude, int amplitudeChange, faultFunctions_t function, int transitionLength);

};


