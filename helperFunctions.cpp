#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <random>
#include <complex>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "helperFunctions.h"
using namespace std;

float GRAVITY = 9.81;

water_grid::water_grid(float amplitude_in, float Lx_in, float Lz_in, int M_in, int N_in, glm::vec2 wind_vector_in) {
    amplitude = amplitude_in;
    Lx = Lx_in;
    Lz = Lz_in;
    M = M_in;
    N = N_in;
    min = 0.0;
    max = 0.0;
    wind_vector = wind_vector_in;
    for (int i = 0; i < N; i++) {
        fourier_grid_t0.push_back(vector<complex<float> >(M, 0.0));
        fourier_grid.push_back(vector<complex<float> >(M, 0.0));
        omega_grid.push_back(vector<float>(M, 0.0));
        position_grid.push_back(vector<complex<float> >(M, 0.0));
        slope_grid_x.push_back(vector<complex<float> >(M, 0.0));
        slope_grid_z.push_back(vector<complex<float> >(M, 0.0));
    }
    build_grid_positions();
    height_grid_fourier_t0();
    eval_grids(0.0);
}

void water_grid::eval_grids(float time) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            float wt = omega_grid[i][j] * time;
            fourier_grid[i][j] = fourier_grid_t0[i][j] * complex<float>(cos(wt), sin(wt)) 
                + conj(fourier_grid_t0[N - i - 1][M - j - 1]) * complex<float>(cos(-wt), sin(-wt)); 
        }
    }
    position_grid = fft2d(fourier_grid, 1.0);
    slope_fourier_grids();
}

void water_grid::slope_fourier_grids() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            slope_grid_x[i][j] = complex<float>(0.0, Kx[i]) * fourier_grid[i][j];
            slope_grid_z[i][j] = complex<float>(0.0, Kz[j]) * fourier_grid[i][j];
        }
    }
    slope_grid_x = fft2d(slope_grid_x, 1.0);
    slope_grid_z = fft2d(slope_grid_z, 1.0);
}

void water_grid::build_grid_positions() {
    for (int n = 0; n < N / 2; n++) {
        Kx.push_back(2 * M_PI * n / Lx);
    }
    for (int n = -N / 2; n < 0; n++) {
        Kx.push_back(2 * M_PI * n / Lx);
    }
    for (int m = 0; m < M / 2; m++) {
        Kz.push_back(2 * M_PI * m / Lz);
    }
    for (int m = -M / 2; m < 0; m++) {
        Kz.push_back(2 * M_PI * m / Lz);
    }
    for (int n = -N/2; n < N/2; n++) {
        X.push_back(n * Lx / N);
    }
    for (int m = -M/2; m < M/2; m++) {
        Z.push_back(m * Lz / M);
    }
}

void water_grid::height_grid_fourier_t0() {
    default_random_engine rng;
    normal_distribution<float> distribution(0.0, 1.0);
    for (int i = 0; i < Kx.size(); i++) {
        for (int j = 0; j < Kz.size(); j++) {
            glm::vec2 wave_vector(Kx[i], Kz[j]);
            float Ph = phillips_spectrum(amplitude, wave_vector, wind_vector);
            float ran1 = distribution(rng);
            float ran2 = distribution(rng);
            fourier_grid_t0[i][j] = height_point_fourier_t0(wave_vector, Ph, ran1, ran2);
            float k = glm::length(wave_vector);
            omega_grid[i][j] = sqrt(k * GRAVITY);
        }
    }
}

vector<Vertex> water_grid::gen_vertices() {
    vector<Vertex> vertices;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (position_grid[i][j].real() > max) {
                max = position_grid[i][j].real();
            }
            else if (position_grid[i][j].real() < min) {
                min = position_grid[i][j].real();
            }
            glm::vec3 position(X[i], position_grid[i][j].real(), Z[j]);
            glm::vec3 normal(-slope_grid_x[i][j].real(), 1.0, -slope_grid_z[i][j].real());
            Vertex v;
            v.pos = position;
            v.normal = normal;
            vertices.push_back(v);
        }
    }
    return vertices;
}

vector<Triangle> water_grid::gen_triangles() {
    vector<Vertex> vertices = gen_vertices();
    vector<Triangle> triangles;
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < M - 1; j++) {
            int xhat = j * N + i;
            Vertex vertex1 = vertices.at(xhat);
            Vertex vertex2 = vertices.at(xhat + 1);
            Vertex vertex3 = vertices.at(xhat + N + 1);
            Vertex vertex4 = vertices.at(xhat + N);
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
    if (k == 0) {
        return 0.0;
    }
    else {
        return amplitude * exp(-1.0 / pow(k * L, 2.0)) / pow(k, 4.0) * pow(dot_product, 2.0);
    }
}

complex<float> height_point_fourier_t0(glm::vec2 wave_vector, float Ph, float ran1, float ran2) {
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
        vector<complex<float> > output = vector<complex<float> >(N, 0.0);
        for (int k = 0; k < N/2; k++) {
            output[k] = even[k] + odd[k] * complex<float>(cos(-2.0 * M_PI * (k + N/2) / N), sin(-2.0 * M_PI * (k + N/2) / N));
            output[k + N/2] = even[k] + odd[k] * complex<float>(cos(-2.0 * M_PI * k / N), sin(-2.0 * M_PI * k / N));
        }
        return output;
    }
}

vector<vector<complex<float> > > fft2d(vector<vector<complex<float> > > &input, float sign) {
    // perform FFT on all rows
    vector<vector<complex<float> > > row_transformed;
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
        even.push_back(input.at(2 * i));
        odd.push_back(input.at(2 * i + 1));
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

string print_vector(vector<complex<float> > &A) {
    string output = "";
    for (int i = 0; i < A.size() - 1; i++) {
        complex<float> num = A.at(i);
        output += "(" + to_string(num.real()) + ", " + to_string(num.imag()) + ")" + ", ";
    }
    complex<float> num = A.at(A.size() - 1);
    output += "(" + to_string(num.real()) + ", " + to_string(num.imag()) + ")" + "\n";
    return output;
}

string print_vector_2D(vector<vector<complex<float> > > &A) {
    string output;
    for (int i = 0; i < A.size(); i++) {
        output += print_vector(A.at(i));
    }
    return output;
}

void write_to_file(string fname, string info) {
    ofstream myfile;
    myfile.open(fname);
    myfile << info << "\n";
    myfile.close();
}