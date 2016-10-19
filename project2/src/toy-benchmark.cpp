#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#define PAGE_SIZE 4*1024 // 4KB pages
#define NUMBER_OF_PAGES_LEVEL_1 512
#define NUMBER_OF_PAGES_LEVEL_2 1

#define RANDOM_ACCESSES 15000

using namespace std;

int main () {

	ofstream myfile;
	myfile.open ("toy.benchmark",  ofstream::out);

	srand(time(0));

	unsigned int 	double_pointer_s = sizeof (double*),
			double_s = sizeof (double),
			int_pointer_s = sizeof (int*),
			int_s = sizeof(int);

	unsigned int 	double_pointer_per_page = PAGE_SIZE / double_pointer_s,
			double_per_page = PAGE_SIZE / double_s,
			int_pointer_per_page = PAGE_SIZE / int_pointer_s,
			int_per_page = PAGE_SIZE / int_s;

	double* 	*double_pointer_array = (double**) malloc (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page * sizeof (double*));
	int*		*int_pointer_array = (int**) malloc (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page * sizeof (int*));

	unsigned int i, j, k;
	for (i = 0; i < (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page); i++)
		*(double_pointer_array + i) = (double*) malloc (NUMBER_OF_PAGES_LEVEL_2 * double_per_page * sizeof(double));
	
	for (i = 0; i < (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page); i++)
		*(int_pointer_array + i) = (int*) malloc (NUMBER_OF_PAGES_LEVEL_2 * int_per_page * sizeof(double));			

	cout << "Passou alocacao de memoria L1" << endl;

	// First, we access and define all positions. We could have been done it at the same time as we created the arrays.
	// NUMBER_OF_ACCESSES = (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page) * (i < NUMBER_OF_PAGES_LEVEL_2 * double_per_page)
	for (i = 0; i < (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page); i++)
		for (j = 0; (j < NUMBER_OF_PAGES_LEVEL_2 * double_per_page); j++)  {
			*(*(double_pointer_array + i) + j) = (double) rand();
		}	

	// NUMBER_OF_ACCESSES = (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page) * (i < NUMBER_OF_PAGES_LEVEL_2 * int_per_page)
	for (i = 0; i < (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page); i++)
		for (j = 0; (j < NUMBER_OF_PAGES_LEVEL_2 * int_per_page); j++)  {
			*(*(int_pointer_array + i) + j) = rand();
		}

	cout << "Passou alocacao de memoria L2" << endl;

	// Access them randomly RANDOM_ACCESSES times
	for (k = 0; k < RANDOM_ACCESSES; k++) {
		i = (unsigned int) (rand () % (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page));
		j = (unsigned int) (rand () % (NUMBER_OF_PAGES_LEVEL_2 * double_per_page));

		*(*(double_pointer_array + i) + j) = (i - j);
	}

	cout << "Passou acessos aleatorios em double**" << endl;

	// Access them randomly RANDOM_ACCESSES times
	for (k = 0; k < RANDOM_ACCESSES; k++) {
		i = (unsigned int) rand () % (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page);
		j = (unsigned int) rand () % (NUMBER_OF_PAGES_LEVEL_2 * int_per_page);

		*(*(int_pointer_array + i) + j) =  (i + j);
	}

	cout << "Passou acessos aleatorios em int**" << endl;

	// Save all in file
	// NUMBER_OF_ACCESSES = (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page) * (i < NUMBER_OF_PAGES_LEVEL_2 * double_per_page)
	for (i = 0; i < (NUMBER_OF_PAGES_LEVEL_1 * double_pointer_per_page); i++) {
		for (j = 0; j < (NUMBER_OF_PAGES_LEVEL_2 * double_per_page); j++)
			// myfile << *(*(double_pointer_array + i) + j) << endl;
			*(*(double_pointer_array + i) + j) = 0;
		free(*(double_pointer_array + i));
	}

	free(double_pointer_array);

	cout << "Passou salvar em arquivo double**" << endl;

	// NUMBER_OF_ACCESSES = (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page) * (i < NUMBER_OF_PAGES_LEVEL_2 * int_per_page)
	for (i = 0; i < (NUMBER_OF_PAGES_LEVEL_1 * int_pointer_per_page); i++) {
		for (j = 0; j < (NUMBER_OF_PAGES_LEVEL_2 * int_per_page); j++) 
			//myfile << *(*(int_pointer_array + i) + j) << endl;
			*(*(int_pointer_array + i) + j) = 0;
		free(*(int_pointer_array + i));
	}

	free(int_pointer_array);

	cout << "Passou salvar em arquivo int**" << endl;

	myfile.close();

	return 0;
}
