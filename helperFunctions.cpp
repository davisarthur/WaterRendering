#include <vector>
#include <math.h>
#include <random>
#include <complex>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "helperFunctions.h"
using namespace std;

float phillips_spectrum(float amplitude, glm::vec2 wave_vector, glm::vec2 wind_vector) {
    float k = glm::length(wave_vector);
    float V = glm::length(wind_vector);
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

vector<complex<float> > fft(vector<complex<float> > &input, float sign) {
    int N = input.size();
    if (N == 1) {
        return input;
    }
    else {
        pair<vector<complex<float> >, vector<complex<float> > > even_odd = even_odd_split(input);
        vector<complex<float> > even = fft(even_odd.first, sign);
        vector<complex<float> > odd = fft(even_odd.second, sign);
        vector<complex<float> > output(N, 0.0f);
        for (int k = 0; k < N / 2; k++) {
            complex<float> p = even.at(k);
            complex<float> q = odd.at(k) * complex<float>(cos(-2 * M_PI / (N * k)), sin(-2 * M_PI / (N * k)));
            output[k] = p + q;
            output[k + N / 2] = p - q;
        }
        return output;
    }
}

pair<vector<complex<float> >, vector<complex<float> > > even_odd_split(vector<complex<float> > &input) {
    vector<complex<float> > even;
    vector<complex<float> > odd;
    for (int i = 0; i < input.size() / 2; i++) {
        even.push_back(input.at(i));
        odd.push_back(input.at(i + 1));
    }
    return pair<vector<complex<float> >, vector<complex<float> > >(even, odd);
}