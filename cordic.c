#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FIXED_POINT (1 << 16)
#define PI 3.14159265358979323846

int* generate_lookup_table(int count) {
	int* table = malloc(count * sizeof(int));
	if (!table) {
		exit(1);
	}
	    
	for (int i = 0; i < count; i++) {
		double angle = atan(1.0 / (2 << i)) * FIXED_POINT;
		table[i] = round(angle);
		//printf("%f\n",round(atan(1.0 / (2 << i + 1))) * FIXED_POINT ); 
	}
	
	return table;
}

int calculate_k(int iterations) {
	double k = 1;
	for (int i = 0; i < iterations; i++) {
		k *= 1.0 / sqrt(1.0 + pow(2.0, -2.0 * i));

	}
	return round(k * FIXED_POINT);
}

void cordic(int iterations, int* lookup, int* cordic_cos, int* cordic_sin, int theta) {
	int k = calculate_k(iterations);
	int angle = round(PI/4 * FIXED_POINT);
	int x = k; int y = k;

	if (theta < 0){
		x = k;
		y = -k;
		angle = round(-PI/4 * FIXED_POINT);
	}
	int shift = 1;
	for (int i = 0; i < iterations; i++) {
		int new_x, new_y;

		if (theta < angle) {
		    new_x = x + (y >> shift);
		    new_y = y - (x >> shift);
		    angle -= lookup[i];
		    //printf("angle: %d x: %d, y: %d shift: %d over\n", angle, new_x, new_y, shift);
		} else {
		    new_x = x - (y >> shift);
		    new_y = y + (x >> shift);
		    angle += lookup[i];
		    //printf("angle: %d x: %d, y: %d under\n", angle, new_x, new_y);
		}

		x = new_x;
		y = new_y;
		shift <<= 1;
	}
	*cordic_cos = x;
	*cordic_sin = y;
}

int main() {
	const int iterations = 5; // after 5 start getting overflow because of shifting
	int* lookup_table = generate_lookup_table(iterations);

//	for (int i = 0; i < iterations; i++){
//		printf("index: %d, value: %d\n", i, lookup_table[i]);	
//	}

	double max_input = round(PI/2 * FIXED_POINT);
	double min_input = -PI/2 * FIXED_POINT;

	int cordic_cosine, cordic_sine;
	double default_cosine, default_sine;

	double error_cosine = 0;
	double error_sine = 0;

	printf("Start\n----------------------------\n");
	int number_of_iterations = 0;
	for (double i = min_input; (int)round(i) <= max_input; i++) {
		cordic(iterations, lookup_table, &cordic_cosine, &cordic_sine, round(i));
		default_sine = sin(i/ (double)FIXED_POINT);
		default_cosine = cos(i / (double)FIXED_POINT);

		error_sine += fabs( default_sine - (cordic_sine / (double)FIXED_POINT));
		error_cosine += fabs( default_cosine - (cordic_cosine / (double)FIXED_POINT));

		number_of_iterations++;
		printf("input angle: %d, cordic sine 2.16: %d, cordic cosine 2.16: %d, cordic sin radians: %f, cordic cosine radians: %f default sine: %f, default cosine: %f\n", (int)round(i), cordic_sine, cordic_cosine, cordic_sine/(double)(FIXED_POINT), cordic_cosine/(double)(FIXED_POINT), default_sine, default_cosine);

	}
	printf("\nMean Absolute Errors\nsine error: %f, cosine error: %f\n", error_sine/ (double)number_of_iterations, error_cosine/ (double) number_of_iterations);
	printf("\n----------------------------\nEnd\n");	

	free(lookup_table);

	return 0;
}
