
#include <iostream>
//#include <iomanip>
#include <math.h>

#include "specs.h"
#include "mmultHW.h"
#include "mmultSW.h"

using namespace std;

void genRandArray(T min, T max, int size, T *array);
int compare(T* gold, T* result, int size, T th);

int main (){
	int errors = 0;
	int tests = 100;
	

	// [M*N] x [N*P] = [M*P]
	T A[M][N], B[N][P];
	T C_HW[M][P], C_SW[M][P];

	T diff;
	T th = 0.001;
	T min = 0;
	T max = 300;

	cout << "Matrix multiplication: ["<< M <<"*"<<N << "] x ["<< N << "*" << P << "]"<< endl;

	for (int i=0; i<tests; i++){
		genRandArray(min, max, M*N, A[0]);
		genRandArray(min, max, N*P, B[0]);

		mmultSW (A, B, C_SW);
		mmultHW (A, B, C_HW);

		errors += compare(C_SW[0], C_HW[0], M*P, th);
	}

	cout <<"Number of errors: " << errors << endl;

	return errors;
}


void genRandArray(T min, T max, int size, T *array){
    for(int i=0; i<size; i++){
        array[i] = min + static_cast <T> (rand()) / ( static_cast <T> (RAND_MAX/(max-min)));
    }
}

int compare(T* gold, T* result, int size, T th){
        int errors = 0;
        double dif = 0;
        for (int i=0; i<size; i++){
                dif = fabs((double)gold[i] - (double)result[i]);
                // a comparison with NaN will always be false
                if (!(dif <= (double)th)){
                        errors++;
                }
        }
        return errors;
}
