#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
using namespace std;

float GRAVITY = 9.81f;
float BACKWARD_FFT = -1.0f;
float FORWARD_FFT = 1.0f;

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

pair<vector<complex<float> >, vector<complex<float> > > even_odd_split(vector<complex<float> > &input);

/*
Requires:
    input - complex vector
    sign - direction of the transform
Returns:
    output - complex vector
*/
vector<complex<float> > fft(vector<complex<float> > &input, float sign);