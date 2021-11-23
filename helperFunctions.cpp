#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <random>
#include <complex>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "helperFunctions.h"
using namespace std;

float GRAVITY = 9.81;

water_grid::water_grid(float Lx_in, float Lz_in, int M_in, int N_in, glm::vec2 wind_vector_in) {
    Lx = Lx_in;
    Lz = Lz_in;
    M = M_in;
    N = N_in;
    X = vector<float>(N, 0.0);
    Z = vector<float>(M, 0.0);
    Kx = vector<float>(N, 0.0);
    Kz = vector<float>(M, 0.0);
    wind_vector = wind_vector_in;
    for (int i = 0; i < N; i++) {
        fourier_grid_t0.push_back(vector<complex<float> >(M, 0.0));
        current_grid.push_back(vector<complex<float> >(M, 0.0));
    }
    build_grid_positions();
    //height_grid_fourier_t0();
    //current_grid = fourier_grid_t0;
}

void water_grid::build_grid_positions() {
    for (int n = -N / 2; n < N / 2; n++) {
        Kx[n + N / 2] = 2 * M_PI * n / Lx;
        X[n + N / 2] = n * Lx / N;
    }
    for (int m = -M / 2; m < M / 2; m++) {
        Kz[m + M / 2] = 2 * M_PI * m / Lz;
        Z[m + M / 2] = m * Lz / M;
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

vector<glm::vec3> water_grid::gen_vertices() {
    vector<glm::vec3> vertices;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            vertices.push_back(glm::vec3(X[i], current_grid[i][j].real(), Z[j]));
        }
    }
    return vertices;
}

vector<Triangle> water_grid::gen_triangles() {
    vector<glm::vec3> vertices = gen_vertices();
    vector<Triangle> triangles;
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < M - 1; j++) {
            int xhat = j * N + i;
            glm::vec3 vertex1 = vertices.at(xhat);
            glm::vec3 vertex2 = vertices.at(xhat + 1);
            glm::vec3 vertex3 = vertices.at(xhat + N + 1);
            glm::vec3 vertex4 = vertices.at(xhat + N);
            Triangle t1; 
            t1.vertex1 = vertex1; t1.vertex2 = vertex2; t1.vertex3 = vertex3;
            Triangle t2;
            t2.vertex1 = vertex1; t2.vertex2 = vertex3; t2.vertex3 = vertex4;
            triangles.push_back(t1);
            triangles.push_back(t2);
        }
    }
    return triangles;
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

string readFile(string fileName) {
   string output = "";
   string line;
   ifstream myfile(fileName);
   if (myfile.is_open()) {
      while (getline(myfile, line)) {
         output += line + "\n";
      }
      myfile.close();
   }

   else cout << "Unable to open file: " << fileName;
   
   return output;
}