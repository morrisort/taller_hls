
#include <iostream>
//#include <iomanip>
#include <math.h>

#include "specs.h"
#include "mmultHW.h"
#include "mmultSW.h"

using namespace std;

void genRandArray(T min, T max, int size, T *array);
int compare(T* gold, T* result, int size, T th);

#define M 2
#define N 2
#define P 2

int main (){
	int errors = 0;
	int tests = 1;
	

	// [M*N] x [N*P] = [M*P]
	T A[M*N], B[N*P];
	T C_HW[M*P], C_SW[M*P];

	T diff;
	T th = 0.000001;
	T min = 0;
	T max = 2000;
	for (int i=0; i<tests; i++){
		genRandArray(min, max, M*N, A);
		genRandArray(min, max, N*P, B);

		mmultSW (A, B, C_SW, M, N, P);
		mmultHW (A, B, C_HW, M, N, P);

		errors += compare(C_SW, C_HW, M*P, th);

		cout << "Test "<< i <<" number of errors: " << errors << endl;
	}

	// bigger!
	T D[4*M*N], E[4*N*P];
	T F_HW[4*M*P], F_SW[4*M*P];
	for (int i=0; i<tests; i++){
		genRandArray(min, max, 4*M*N, D);
		genRandArray(min, max, 4*N*P, E);

		mmultSW (D, E, F_SW, 2*M, 2*N, 2*P);
		mmultHW (D, E, F_HW, 2*M, 2*N, 2*P);

		errors += compare(F_SW, F_HW, 2*M*P, th);

		cout << "Test "<< i <<" number of errors: " << errors << endl;
	}
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
