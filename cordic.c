/* CE4518 - COMPUTER ARCHITECTURE 
 * Assignment Title: Cordic 2.16
 *
 * The Purpose of this assignment is to implement a cordic algorith in 2.16 
 * fixed point. The algorithim is implement in the cordic() function and only uses
 * shifting and adding and subtraction. This complies with the assumption of the system
 * not having a multiplier.
 *
 * Results:
 * - The Program will output the bit accuracy for a varying number of iterations. The
 *   program will vary the iterations from 1-18 and keep track of the iteration with the 
 *   best precesion. If multiple iterations share the same precesion the lowest one will be 
 *   chosen.
 * - The output contains the first and last 10 Cordic Approximations and the builtin counterparts.
 *   This was done just for reference and to get an idea if everything is working.
 * - You will also notice the Mean Absolute Error is Displayed. Not requested but is a useful
 *   metric
 *
 * Run the C program using the following commands:
 * 	gcc -o cordic cordic.c -lm
 * 	./cordic
 *
 * Authors: Andre Costa Barros 21307237, Seamus Delaney 21341044, Deniss Svetlitsnoi 21333521 
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>

#define FIXED_POINT (1 << 16) // used to scale by 2^16
#define PI 3.14159265358979323846 // used to  represent Pi
#define STARTING_ANGLE round(PI/4 * FIXED_POINT) // The starting angle for 

/* int count_leading_zeros() 
 * - Function used to count the number of count_leading_zeros.
 * - If the input is 0 it returns 18 because 2.16 is 18 bits.
 * - Runs loop that only ends when value is shifted to be equal to 1.
 *   The Value being equal to 1 means all leading zeros have been counted.
 * - Returns number of leading zeros.
 * */
int count_leading_zeros(unsigned int x){
	if (x == 0) return 18; // initial check, if input is zero return 18 leading zeros.

	int count = 0;// initialise count variable to be returned
	for (int i = 17; i >= 0; i--){
		if ((x >> i) & 1) break; // Shift left until value is 1 (all leading zeros are accounted for).
		count++; // increment count
	}
	return count; // return count
}

/* int generate_lookup_table()
 * - Function used to generate lookup table.
 * - This is often predefined manually so atan() in math.h was used for simplicity.
 * - Code mimicks method in hints notes to calculate the angles.
 * - Returns an int pointer pointing to an array containing the angles.
 */
int* generate_lookup_table(int count) {
	int* table = malloc(count * sizeof(int)); // allocate size for iterations * sizeof(int)
	if (!table) {				  // if memomory address null exit
		exit(1);
	}
	    
	for (int i = 0; i < count; i++) {
		double angle = atan(1.0 / (2 << i)) * FIXED_POINT; // calculate angles in 2.16
		table[i] = round(angle); // round angles
	}
	
	return table; // return array
}
/* int calculate_k()
 * - Function used to calculate constant k.
 * - Implements formula discussed in the hints notes.
 * - Returns 2.16 value of K.
 * - Loop ends when desired number of iterations are completed.
 */
int calculate_k(int iterations) {
	double k = 1; // Start as one to multiply later.
	for (int i = 0; i < iterations; i++) {
		k *= 1.0 / sqrt(1.0 + pow(2.0, -2.0 * i)); // formula discussed in notes

	}
	return round(k * FIXED_POINT); // returns rounded 2.16 number
}

/* void cordic()
 * - Function to implement Cordic algorithim.
 * - If theta is negative y is also negative, else both x and y are positive.
 * - Implements version in notes that starts at 45 degrees.
 * - The loops ends when required number of iterations is a achieved.
 * - Doesn't return anything, instead takes memory addresses to change variables in other fucntions.
 */
void cordic(int iterations, int* lookup, int* cordic_cos, int* cordic_sin, int theta) {
	int k = calculate_k(iterations); // calcualtes k for desired number of iterations
	int angle = STARTING_ANGLE; // 45 degrees in 2.16
	int x = k; int y = k; // sets x and y to +pos k

	if (theta < 0){ // if theta < 0, y = -neg k and angle = - 45
		x = k;
		y = -k;
		angle = -angle;
	}

	for (int i = 1; i <= iterations; i++) {
		int new_x, new_y; // used for temporary storage

		if (theta < angle) { // For overshoot
		    new_x = x + (y >> i); // derived from notes
		    new_y = y - (x >> i);
		    angle -= lookup[i-1]; // subtract ith angle in lookup table from var angle
		} else {                      // Under Shoot
		    new_x = x - (y >> i); // Also derived from notes.
		    new_y = y + (x >> i);
		    angle += lookup[i-1];       // add ith angle in lookup table from var angle
		}
		// set new xin and yin for next iterations	
		x = new_x; 
		y = new_y;
	}
	// save final results
	*cordic_cos = x;
	*cordic_sin = y;
}

/* int find_best_iteration()
 * - Function used to find the number of iterations that yields the best precesion.
 * - Keeps track of worst case precesion for each number of iterations.
 * - The number of iterations with the best precesion is returned.
 * - Modifies variable in other functions keeping track of the best precesion.
 */
int find_best_iteration(unsigned int *min_error){	
	int n; // used to return in the end
	
	// Runs 1 - 18 iterations
	for (int iterations = 1; iterations <= 18; iterations++){
		int* lookup_table = generate_lookup_table(iterations); // Generates lookup table for specified number of iterations.
		int best_precision_per_iteration = 18; // Best Precision possible is 18 bits.
		int max_input = round(PI/2 * FIXED_POINT); // Max angle input in 2.16
		double min_input = -PI/2 * FIXED_POINT;    // Min angle input in 2.16, not rounded to not lose accuracy.

		int cordic_cosine, cordic_sine; // stores cordic algorithim results
		int default_cosine, default_sine; // stores results from builtin cosine and sine functions

		// Runs through all possible angles between and including -pi/2 and pi/2
		for (double i = min_input; (int)round(i) <= max_input; i++) {
			cordic(iterations, lookup_table, &cordic_cosine, &cordic_sine, round(i)); // runs cordic algo for specified number of iterations
			default_sine = round(sin(i/(double)FIXED_POINT) * FIXED_POINT); // builtin sin
			default_cosine = round(cos(i/(double)FIXED_POINT) * FIXED_POINT); // builtin cos

			unsigned int error_sine = abs(cordic_sine - default_sine);       // absolute errors 
			unsigned int error_cosine = abs(cordic_cosine - default_cosine);

			error_sine = count_leading_zeros(error_sine);			// Counts leading zeros
			error_cosine = count_leading_zeros(error_cosine);
			
			// if statement keeps track of worst case precesion for each iteration
			if ((error_cosine < best_precision_per_iteration) || (error_sine < best_precision_per_iteration)){
				best_precision_per_iteration = (error_cosine < error_sine) ? error_cosine : error_sine;	
			}

		}
		printf("Worst Precision Using %d Iterations %d is Bits\n", iterations, best_precision_per_iteration);	
		if (*min_error < best_precision_per_iteration){  // find iteration with highest bit precesion, will took the lowest iteration with the best precesion
			*min_error = best_precision_per_iteration;
			n = iterations;
		}
		free(lookup_table); // free memory used by lookup table
	}
	return n;
}

int main() {
	unsigned int min_error = 0; // used to find best precesion in find_best_iteration()

	int iterations = find_best_iteration(&min_error); // most optimal number of iterations
	int* lookup_table = generate_lookup_table(iterations); // lookup table with optimal number of angles
	int max_input = round(PI/2 * FIXED_POINT); // Pi/2 in 2.16
	double min_input = -PI/2 * FIXED_POINT; // -Pi/2 in 2.16

	int cordic_cosine, cordic_sine; 
	int default_cosine, default_sine;

	int abs_error_cosine = 0; // used to calculate Mean Absolute Error
	int abs_error_sine = 0;
	
	int current_iteration = 0; // Keeps track of total number of iterations, used for printing and Mean Absolute Error
	printf("\nPrint First 10 and Last 10 Cordic Approximations\n");
	for (double i = min_input; (int)round(i) <= max_input; i++){
			cordic(iterations, lookup_table, &cordic_cosine, &cordic_sine, round(i)); // runs cordic algo for specified angle
			default_sine = round(sin(i/ (double)FIXED_POINT) * FIXED_POINT); 
			default_cosine = round(cos(i / (double)FIXED_POINT) * FIXED_POINT);

			abs_error_cosine += abs(cordic_cosine - default_cosine); // absolute difference
			abs_error_sine += abs(cordic_sine - default_sine);
		
		// used to print first 10 Cordic Approximations and builtin cosine and sine results
		if (current_iteration < 10){
			printf("input angle: %d, cordic sine: %d, cordic cosine: %d, default sine: %d, default cosine: %d\n", 
			(int)round(i), cordic_sine, cordic_cosine, default_sine, default_cosine);
			
			// Used to make formatting better adds newline after 10th iteration
			if (current_iteration == 9) printf("\n");
		}
		
		// used to print last 10 Cordic Approximations and builtin cosine and sine results
		if ((int)round(i) >= max_input - 10){
			printf("input angle: %d, cordic sine: %d, cordic cosine: %d, default sine: %d, default cosine: %d\n", 
			(int)round(i), cordic_sine, cordic_cosine, default_sine, default_cosine);
		}


		current_iteration++;
	}
	printf("\nBest Precision found: %d bits using %d iterations\n", min_error, iterations); // prints best precesion and its optimal number of iterations
	// Prints Mean Absolute Error for Cosine and Sine
	printf("\nMean Absolute Error in 2.16\nCosine: %f Sine: %f\n", abs_error_cosine/ (double)(current_iteration), abs_error_sine/ (double)(current_iteration));
	free(lookup_table); // frees memory used by lookup table

	return 0;
}
