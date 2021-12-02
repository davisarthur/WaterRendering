#include <vector>
#include <complex>
#include <math.h>
#include <string>
#include <random>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
using namespace std;

struct Vertex {
   glm::vec3 pos;
   glm::vec3 slope_vector;
};
struct Triangle {
    Vertex vertex1;
    Vertex vertex2;
    Vertex vertex3;
};

class ProjectedGrid {
    public:
        int nx;
        int nz;
        float xMin, xMax, zMin, zMax;
        vector<float> X;
        vector<float> Z;
        vector<glm::vec3> triangles;
        vector<glm::vec3> vertices;

        ProjectedGrid(int nxIn, int nyIn, float xMinIn, float xMaxIn, float zMinIn, float zMaxIn);
    
    private:
        vector<glm::vec3> gen_vertices();
        vector<glm::vec3> gen_triangles();
};

class water_grid {
    public:
        float min, max, min_slope_x, max_slope_x, min_slope_z, max_slope_z;

        // size of grid in position space
        float Lx, Lz; 

        // level of detail
        int M, N;
        
        // vectors containing all x and z positions in the water grid
        vector<float> X, Z;

        // vectors containing all kx and all ky values in the water grid
        vector<float> Kx, Kz;

        // wave properties
        glm::vec2 wind_vector;
        float amplitude;

        // fourier grid t = 0
        vector<vector<complex<float> > > fourier_grid_t0, fourier_grid, position_grid, slope_grid_x, slope_grid_z;
        vector<vector<float> > omega_grid;

        water_grid(float amplitude_in, float Lx_in, float Lz_in, int M_in, int N_in, glm::vec2 wing_vector_in);
        void eval_grids(float time);
        void slope_fourier_grids();
        vector<Triangle> gen_triangles();

    private:
        void build_grid_positions();
        void height_grid_fourier_t0();
        vector<Vertex> gen_vertices();
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
complex<float> height_point_fourier_t0(glm::vec2 wave_vector, float Ph, float ran1, float ran2);

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

string readFile(string fileName);

string print_vector_real(vector<float> &A);

string print_vector(vector<complex<float> > &A);

string print_vector_2D_real(vector<vector<float> > &A);

string print_vector_2D(vector<vector<complex<float> > > &A);

void write_to_file(string fname, string info);

vector<complex<float> > random_vector(int N);