#include <vector>
#include <math.h>
#include <random>
#include <complex>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "helperFunctions.h"
using namespace std;

water_grid::water_grid(float Lx_in, float Lz_in, int M_in, int N_in) {
    Lx = Lx_in;
    Lz = Lz_in;
    M = M_in;
    N = N_in;
    Kx = vector<float>(N, 0.0);
    Kz = vector<float>(M, 0.0);
    for (int i = 0; i < N; i++) {
        fourier_grid_t0.push_back(vector<complex<float> >(M, 0.0));
        current_grid.push_back(vector<complex<float> >(M, 0.0));
    }
}

void water_grid::build_wave_vectors() {
    for (int n = -N / 2; n < N / 2; n++) {
        Kx[n + N / 2] = 2 * M_PI * n / Lx;
    }
    for (int m = -M / 2; m < M / 2; m++) {
        Kz[m + M / 2] = 2 * M_PI * m / Lz;
    }
}

void water_grid::height_grid_fourier_t0() {
    for (int i = 0; i < Kx.size(); i++) {
        for (int j = 0; j < Kz.size(); j++) {
            glm::vec2 k(Kx[i], Kz[j]);
            float Ph = phillips_spectrum(amplitude, k, wind_vector);
            fourier_grid_t0[i][j] = height_point_fourier_t0(k, Ph);
        }
    }
}

float phillips_spectrum(float amplitude, glm::vec2 wave_vector, glm::vec2 wind_vector) {
    float k = glm::length(wave_vector);
    float V = glm::length(wind_vector);
    float L = pow(V, 2.0) / GRAVITY;
    float dot_product = glm::dot(glm::normalize(wave_vector), glm::normalize(wind_vector));
    return amplitude * exp(-1.0 / pow(k * L, 2.0)) / pow(k, 4.0) * pow(dot_product, 2.0);
}

complex<float> height_point_fourier_t0(glm::vec2 wave_vector, float Ph) {
    default_random_engine rng;
    normal_distribution<float> distribution(0.0, 1.0);
    float ran1 = distribution(rng);
    float ran2 = distribution(rng);
    return (complex<float>) (sqrt(Ph) / sqrt(2.0)) * complex<float>(ran1, ran2);
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

vector<vector<complex<float> > > fft2d(vector<vector<complex<float> > > &input, float sign) {
    vector<vector<complex<float> > > row_transformed;

    // perform FFT on all rows
    for (int i = 0; i < input.size(); i++) {
        row_transformed.push_back(fft(input[i], sign));
    }

    // perform FFT on all columns
    vector<vector<complex<float> > > output;
    transpose2d(row_transformed);
    for (int i = 0; i < row_transformed.size(); i++) {
        output.push_back(fft(row_transformed[i], sign));
    }

    // transpose to return to correct form
    transpose2d(output);
    return output;
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

void transpose2d(vector<vector<complex<float> > > &A) {
    vector<vector<complex<float> > > output;
    for (int i = 0; i < A[0].size(); i++) {
        vector<complex<float> > row;
        for (int j = 0; j < A.size(); j++) {
            row.push_back(A[j][i]);
        }
        output.push_back(row);
    }
    A = output;
}