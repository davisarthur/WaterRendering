#include <vector>
#include <math.h>
#include <random>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
using namespace std;

float GRAVITY = 9.81f;
float phillips_spectrum(float amplitude, glm::vec2 wave_vector, glm::vec2 wind_vector) {
    float k = glm::magnitude(wave_vector);
    float V = glm::magnitude(wind_vector);
    float L = pow(V, 2.0) / GRAVITY;
    float dot_product = glm::dot(glm::normalize(wave_vector), glm::normalize(wind_vector));
    return amplitude * exp(-1.0 / pow(k * L, 2.0)) / pow(k, 4.0) * pow(dot_product, 2.0);
}

glm::vec2 height_fourier_space(glm::vec2 wave_vector, float Ph) {
    default_random_engine rng;
    normal_distribution<float> distribution(0.0, 1.0);
    float ran1 = distribution(rng);
    float ran2 = distribution(rng);
    float real = 1.0 / pow(2.0, 0.5) * ran1 * pow(Ph, 0.5);
    float imaginary = 1.0 / pow(2.0, 0.5) * ran2 * pow(Ph, 0.5);
    return glm::vec2(real, imaginary);
}