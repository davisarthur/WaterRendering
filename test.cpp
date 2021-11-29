#include "helperFunctions.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <complex>
using namespace std;

int main() {
    int N = 8;
    vector<complex<float> > input = random_vector(N);
    vector<complex<float> > output = fft(input, 1.0);
    write_to_file("data/test_input.txt", print_vector(input));
    write_to_file("data/test_output.txt", print_vector(output));
    return 0;
}