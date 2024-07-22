#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

double mapToReal(int x, int imageWidth, double minR, double maxR) {
    double range = maxR - minR;
    return x * (range / imageWidth) + minR;
}

double mapToImaginary(int y, int imageHeight, double minI, double maxI) {
    double range = maxI - minI;
    return y * (range / imageHeight) + minI;
}

int findMandelbrot(double cr, double ci, int max_iterations) {
    int i = 0;
    double zr = 0.0, zi = 0.0;
    while (i < max_iterations && zr * zr + zi * zi < 4.0) {
        double temp = zr * zr - zi * zi + cr;
        zi = 2.0 * zr * zi + ci;
        zr = temp;
        i++;
    }
    return i;
}

int main() {
    // Get the required input values
    ifstream fin("input.txt");
    int imageWidth, imageHeight, maxN;
    double minR, maxR, minI, maxI;

    if (!fin) {
        cout << "Error opening input file" << endl;
        cin.ignore();
        return 0;
    }
    fin >> imageWidth >> imageHeight >> maxN;
    fin >> minR >> maxR >> minI >> maxI;
    fin.close();

    // Buffer to store RGB values
    vector<vector<int>> buffer(imageHeight, vector<int>(imageWidth * 3));

    auto start = high_resolution_clock::now();

    // Use OpenMP to parallelize the loop
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            // Find the real and imaginary values for c, corresponding to the pixel at position (x, y)
            double cr = mapToReal(x, imageWidth, minR, maxR);
            double ci = mapToImaginary(y, imageHeight, minI, maxI);

            // Find the number of iterations in the Mandelbrot formula using said c
            int n = findMandelbrot(cr, ci, maxN);

            // Map the resulting number to an RGB value
            int r = (n % 256);
            int g = (n % 256);
            int b = (n % 256);

            // Store the RGB values in the buffer
            buffer[y][x * 3] = r;
            buffer[y][x * 3 + 1] = g;
            buffer[y][x * 3 + 2] = b;
        }
    }

    // Open the output file, write the ppm header
    ofstream fout("output_parallel.ppm");
    fout << "P3" << endl;
    fout << imageWidth << " " << imageHeight << endl;
    fout << "255" << endl;

    // Write the buffer to the output file
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            fout << buffer[y][x * 3] << " " << buffer[y][x * 3 + 1] << " " << buffer[y][x * 3 + 2] << " ";
        }
        fout << endl;
    }

    fout.close();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "Image created in " << duration << " milliseconds." << endl;
    return 0;
}
