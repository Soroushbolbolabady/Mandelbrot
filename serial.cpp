#include <fstream>
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

double mapToReal(int x, int imageWidth, double minR, double maxR){
    double range = maxR - minR;
    return x * (range / imageWidth) + minR;
}

double mapToImaginary(int y, int imageHeight, double minI, double maxI){
    double range = maxI - minI;
    return y * (range / imageHeight) + minI;
}

int findMandelbrot(double cr, double ci, int max_iterations){
    int i = 0;
    double zr = 0.0, zi = 0.0;
    while (i < max_iterations && zr * zr + zi * zi < 4.0){
        double temp = zr * zr - zi * zi + cr;
        zi = 2.0 * zr * zi + ci;
        zr = temp;
        i++;
    }
    return i;
}

int main(){
    // get the required input values
    ifstream fin("input.txt");
    int imageWidth, imageHeight, maxN;
    double minR, maxR, minI, maxI;

    if (!fin){
        cout << "Error opening input file" << endl;
        cin.ignore();
        return 0;
    }
    fin >> imageWidth >> imageHeight >> maxN;
    fin >> minR >> maxR >> minI >> maxI;
    fin.close();

    // open the output file, write the ppm header
    ofstream fout("output_serial.ppm");
    fout << "P3" << endl;
    fout << imageWidth << " " << imageHeight << endl;
    fout << "255" << endl; 

    // Measure the start time
    auto start = high_resolution_clock::now();

    //for every pixel...
    for (int y = 0; y < imageHeight; y++){
        for (int x = 0; x < imageWidth; x++){
            // ... find the real and imaginary values for c, corresponding 
            // to the pixel at position (x, y)
            double cr = mapToReal(x, imageWidth, minR, maxR);
            double ci = mapToImaginary(y, imageHeight, minI, maxI);

            // finding the number of iterations in the Mandelbrot formula
            //using said c.
            int n = findMandelbrot(cr, ci, maxN);

            // ... map the resulting number to an RGB value
            int r = (n % 256);
            int g = (n % 256);
            int b = (n % 256);
            // output it to the image
            fout << r << " " << g << " " << b << " ";
        }
        fout << endl;
    }

    fout.close();

    // Measure the end time
    auto end = high_resolution_clock::now();

    // Calculate the duration
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "Image created in " << duration << " milliseconds." << endl;
    return 0;
}
