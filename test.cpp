#include "helperFunctions.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <complex>
using namespace std;

int main() {
    int N = 16;
    vector<complex<float> > input = random_vector(N);
    vector<complex<float> > output = fft(input, 1.0);
    write_to_file("data/test_input.txt", print_vector(input));
    write_to_file("data/test_output.txt", print_vector(output));

    vector<complex<float> > v1;
    v1.push_back(complex<float>(1.0, 0.0));
    v1.push_back(complex<float>(2.0, 0.0));
    v1.push_back(complex<float>(3.0, 0.0));

    vector<complex<float> > v2;
    v2.push_back(complex<float>(4.0, 0.0));
    v2.push_back(complex<float>(5.0, 0.0));
    v2.push_back(complex<float>(6.0, 0.0));

    vector<complex<float> > v3;
    v3.push_back(complex<float>(7.0, 0.0));
    v3.push_back(complex<float>(8.0, 0.0));
    v3.push_back(complex<float>(9.0, 0.0));

    vector<vector<complex<float> > > array;
    array.push_back(v1);
    array.push_back(v2);
    array.push_back(v3);

    cout << print_vector_2D(array) << endl;
    transpose2d(array);
    cout << print_vector_2D(array) << endl;

    return 0;
}