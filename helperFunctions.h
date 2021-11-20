#include <vector>
#include <complex>
#include <math.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
using namespace std;

float GRAVITY = 9.81f;
float BACKWARD_FFT = -1.0f;
float FORWARD_FFT = 1.0f;

class water_grid {
    public:
        // size of grid in position space
        float Lx;
        float Lz; 

        // level of detail
        int M;
        int N;
        
        // vectors containing all kx and all ky values in the water grid
        vector<float> Kx;
        vector<float> Kz;

        // wave properties
        glm::vec2 wind_vector;
        float amplitude;

        // fourier grid t = 0
        vector<vector<complex<float> > > fourier_grid_t0;

        // current grid
        vector<vector<complex<float> > > current_grid;

        water_grid(float Lx_in, float Lz_in, int M_in, int N_in);

    private:
        void build_wave_vectors();
        void height_grid_fourier_t0();
};

/*
*/
float phillips_spectrum(float amplitude, glm::vec2 wave_vector, glm::vec2 wind_vector);

/*
Requires:
    Ph - Phillip's spectrum value
Returns:
    height map value at t = 0 in Fourier space
*/
complex<float> height_point_fourier_t0(glm::vec2 wave_vector, float Ph);

/*
*/
pair<vector<complex<float> >, vector<complex<float> > > even_odd_split(vector<complex<float> > &input);

/*
Requires:
    input - complex vector
    sign - direction of the transform
Returns:
    output - complex vector
*/
vector<complex<float> > fft(vector<complex<float> > &input, float sign);

vector<vector<complex<float> > > fft2d(vector<vector<complex<float> > > &input, float sign);

/*
Transpose 2D complex array made using vectors (array must be rectangular)
*/
void transpose2d(vector<vector<complex<float> > > &A);