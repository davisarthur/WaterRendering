#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
using namespace std;

/*

*/
float phillips_spectrum(float amplitude, glm::vec2 wave_vector, glm::vec2 wind_vector);

/*
Returns: 
    complex number as a 2-vector (real, imaginary)
*/
glm::vec2 height_fourier_space(glm::vec2 wave_vector);

/*
Requires:
    Ph - Phillip's spectrum value
Returns:
    height map value at t = 0 in Fourier space
*/
glm::vec2 height_fourier_space(glm::vec2 wave_vector, float Ph);