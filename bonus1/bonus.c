#include <stdio.h>
#include <time.h>

unsigned int counter;

void a();
void b();
void c();

void a (){

	if (counter) {
		counter--;
		b();
	}
}

void b () {

	if (counter){
		counter--;
		c();
	}
}

void c () {
	
	if (counter) {
		counter--;
		a();
	}
}

void d() {
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

int main(){

	struct timespec begin, end, result;

	
	for (unsigned int i = 2; i <= 5096; i = i*2) {

		/* Clear RAS */
		for (unsigned int p = 0; p < 1024; p++ ) 
			d();

		counter = i;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin); 

		a();

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end); 

		result = diff (begin, end);

		printf ("%u,%ld\n", i, result.tv_nsec);

	}

	return 0;

}
