#include <stdio.h>
#include <time.h>

#define NUMBER_ITERATIONS 	10000
#define MAX_RECURSION		128

unsigned int counter;

void a(void);
void b(void);
void c(void);	
void d(void);
void e(void);
void f(void);
void g(void);

typedef void (*fs)(void);

fs funcs[7]; /* 7 functions to prevent any kind of call patterns */

void a (void){

	if (counter) {
		counter--;
		//(*(funcs[counter % 7]))();
		b();
	}

	return;
}

void b (void) {

	if (counter){
		counter--;
		//(*(funcs[(counter * 2) % 7]))();
		c();	
	}

	return;
}

void c (void) {
	
	if (counter) {
		counter--;
		//(*(funcs[(counter * 3) % 7]))();
		d();
	}

	return;
}

void d (void) {
	
	if (counter) {
		counter--;
		//(*(funcs[(counter * 4) % 7]))();
		e();
	}

	return;
}

void e (void) {
	
	if (counter) {
		counter--;
		//(*(funcs[(counter * 5) % 7]))();
		f();
	}

	return;
}

void f (void) {
	
	if (counter) {
		counter--;
		//(*(funcs[(counter * 6) % 7]))();
		g();
	}

	return;
}

void g (void) {
	
	if (counter) {
		counter--;
		//(*(funcs[(counter * 7 - 3) % 7]))();
		a();
	}

	return;
}

struct timespec diff(struct timespec start, struct timespec end)
{
        struct timespec temp;
        if ((end.tv_nsec - start.tv_nsec) < 0) 
        {
                temp.tv_sec = end.tv_sec - start.tv_sec - 1;
                temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
        } 
        else 
        {
                temp.tv_sec = end.tv_sec - start.tv_sec;
                temp.tv_nsec = end.tv_nsec - start.tv_nsec;
        }
        return temp;
}

void nope() {
	return;
}

int main(){

	struct timespec begin, end, result;

	funcs[0] = a;
	funcs[1] = b;
	funcs[2] = c;
	funcs[3] = d;
	funcs[4] = e;
	funcs[5] = f;
	funcs[6] = g;

	/* Array to save delays */
	unsigned long int diffs[MAX_RECURSION/2 + 1];
	for (int i = 1; i <= MAX_RECURSION/2; i++) 
		diffs [i] = 0;

	for (int j = 0; j < NUMBER_ITERATIONS; j++)
		for (unsigned int i = 2; i <= MAX_RECURSION; i = i + 2) {

			/* CLOCK_REALTIME may need root privileges */
			clock_gettime(CLOCK_REALTIME, &begin); 

			/* Clear RAS - overflows RAS a considerable number of times and  
			   eliminates the effects caused by 'clock_gettime()' call. */
			for (unsigned int p = 0; p < 64; p++) 
				nope();

			counter = i;

			/* During recursion, we need to make the job as easiest as possible to the branch predictor unit. 
			   For this reason, we create a cycle call pattern, that is, a calls b, that call c and so on. 
			   The only unit we need to test is the RAS. */
			a();

			clock_gettime(CLOCK_REALTIME, &end); 

			result = diff (begin, end);

			diffs[i/2] += result.tv_nsec;

		}

	int ras = 0;
	long int diff, diff_previous, diff_sum = 0;
	for (int i = 2; i <= MAX_RECURSION; i = i + 2)  {

			if (i != 2) {
				diff = (long int) (diffs[i/2] - diffs[(i-2)/2])/NUMBER_ITERATIONS;
				printf ("%u,%ld, d=%ld\n", i, diffs[i/2]/NUMBER_ITERATIONS, diff);

				/* if the actual iteration takes more than 2 times that average of the previous ones, 
				   we conclude that there has been an overflow! */
				if (i > 12 && ((double) diff) > (((double)diff_sum) * 2 * 2.0 / (i - 4))) {
					ras = i;
					break;
				}

				diff_previous = diff;
				diff_sum += diff;
			}
			else 	printf ("%u,%ld\n", i, diffs[i/2]/NUMBER_ITERATIONS);
	}

	printf ("O RAS possui em torno de %d entradas.\n", ras);
	return 0;

}
