#include <iostream>
#include <complex>
#include <vector>
#include <chrono>
#include <functional>

#include "window.h"
#include "save_image.h"
#include "utils.h"

// clang++ -std=c++11 -stdlib=libc++ -O3 save_image.cpp utils.cpp mandel.cpp -lfreeimage

// Use an alias to simplify the use of complex type
using Complex = std::complex<double>;

// Convert a pixel coordinate to the complex domain
template<typename T>
Complex scale(window<T> &scr, window<double> &fr, Complex c) {
	Complex aux(c.real() / (double)scr.width() * fr.width() + fr.x_min(),
		c.imag() / (double)scr.height() * fr.height() + fr.y_min());
	return aux;
}

// Check if a point is in the set or escapes to infinity, return the number of iterations
int escape(Complex c, int iter_max, const std::function<Complex( Complex, Complex)> &func) {
	Complex z(0);
	int iter = 0;

	while (abs(z) < 2.0 && iter < iter_max) {
		z = func(z, c);
		iter++;
	}

	return iter;
}

// Loop over each pixel from our image and check if the points associated with this pixel escape to infinity
template<typename T>
void get_number_iterations(window<T> &scr, window<double> &fract, int iter_max, height_matrix<T> &heights,
	const std::function<Complex( Complex, Complex)> &func) {
	int progress = -1;

	for(T i = scr.y_min(); i < scr.y_max(); i += scr.y_res()) {
		for(T j = scr.x_min(); j < scr.x_max(); j += scr.x_res()) {
			Complex c((double)j, (double)i);
			c = scale<T>(scr, fract, c);
			heights[i][j] = escape(c, iter_max, func);
		}
		if(progress < static_cast<int>(i*100.0/scr.y_max())){
			progress = static_cast<int>(i*100.0/scr.y_max());
			std::cout << progress << "%\n";
		}
	}
}

template<typename T>
void fractal(window<T> &scr, window<double> &fract, int iter_max, height_matrix<T> &heights,
	const std::function<Complex( Complex, Complex)> &func, const char *fname) {
	static_assert(std::is_scalar<T>::value, "Type must be scalar");

	auto start = std::chrono::steady_clock::now();
	get_number_iterations(scr, fract, iter_max, heights, func);
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time to generate " << fname << " = " << std::chrono::duration <double, std::milli> (end - start).count() << " [ms]" << std::endl;

	// Save (show) the result as an image
	plot(scr, heights, iter_max, fname);
}

void mandelbrot() {
	using screentype = double;
	// Define the size of the image
	window<screentype> scr(0, 200, 0, 200, 0.2, 0.2);
	// The domain in which we test for points
	window<double> fract(-2.2, 1.2, -1.7, 1.7);

	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z + c; };

	int iter_max = 100;
	const char *fname = "mandelbrot.3mf";
	bool smooth_color = true;

	height_matrix<screentype> heights(scr.height(), height_vector<screentype>(scr.width(), 0));

	// Experimental zoom (bugs ?). This will modify the fract window (the domain in which we calculate the fractal function) 
	//zoom(1.0, -1.225, -1.22, 0.15, 0.16, fract); //Z2
	
	fractal(scr, fract, iter_max, heights, func, fname);
}

void triple_mandelbrot() {
	using screentype = double;
	// Define the size of the image
	window<screentype> scr(0, 200, 0, 200, 0.2, 0.2);
	// The domain in which we test for points
	window<double> fract(-1.5, 1.5, -1.5, 1.5);

	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z * z + c; };

	int iter_max = 100;
	const char *fname = "triple_mandelbrot.3mf";
	height_matrix<screentype> heights(scr.height(), height_vector<screentype>(scr.width(), 0));

	fractal(scr, fract, iter_max, heights, func, fname);
}

int main() {

	mandelbrot();
	triple_mandelbrot();

	return 0;
}
