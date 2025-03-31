#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FIXED_POINT (1 << 16)
#define PI 3.14159265358979323846

int* generate_lookup_table(int count) {
	int* table = malloc(count * sizeof(double));
	if (!table) {
		exit(1);
	}
	    
	for (int i = 0; i < count; i++) {
		table[i] = round(atan(1.0 / (1 << i))) * FIXED_POINT;
	}
	
	return table;
}

int calculate_k(int iterations) {
	int k = 1;
	for (int i = 0; i < iterations; i++) {
	   k *= round(1.0 / sqrt(1.0 + pow(2.0, -2.0 * i))) * FIXED_POINT;
	}
	return k;
}

void cordic(int iterations, int* lookup, int* cordic_cos, int* cordic_sin, int theta) {
	int k calculate_k(iterations);	
	int guess = round(PI/4) * FIXED_POINT;
	int x = k; int y = k;
	
	if (theta < 0){
		x = k;
		y = -k;
	}

	for (int i = 0; i < iterations; i++) {
		int shift = 1;
		int new_x, new_y;

		if (theta <= angle) {
		    new_x = x + (y >> shift);
		    new_y = y - (x >> shift);
		    angle -= lookup[i];
		} else {
		    new_x = x - (y >> shift);
		    new_y = y + (x >> shift);
		    angle += lookup[i];
		}

		x = new_x;
		y = new_y;
		shift <<= 1;
	}

	*out_cos = x;
	*out_sin = y;
}

int main() {
	const int iterations = 20;
	double* lookup_table = generate_lookup_table(iterations);

	int max_input = round(PI/4) * FIXED_POINT;
	int min_input = round(-PI/4) * FIXED_POINT;
	int min_input_radians = round(-PI/4);

	int cordic_cosine, cordic_sine;
	int default_cosine, default_sine;

	double error_cosine;
	double error_sine;

	printf("Start\n----------------------------")	

	int j;
	for (int i = min_input; i <= max_input; i++) {
		cordic(iterations, &lookup_table, cordic_cosine, cordic_sine, i);

		default_sine = round(sin(min_input + j)) * FIXED_POINT;
		default_cosine = round(cos(min_input + j)) * FIXED_POINT;

		error_sine = abs( default_sine - cordic_sine) / default_sine;
		error_cosine = abs( default_cosine - cordic_cosine) / default_cosine;

		printf("input angle: %d, cordic sine: %d, cordic cosine: %d, default sine: %d, default cosine: $d", i, cordic_sine, cordic_cosine, default_sine, default_cosine);

	}
	printf("sine error: %f, cosine error: %f", error_sine, error_cosine);
	printf("----------------------------\nEnd")	

	free(lookup_table);

	return 0;
}
