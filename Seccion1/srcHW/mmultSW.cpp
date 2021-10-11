#include "mmultSW.h"


void mmultSW (T *A, T *B, T *C, int m, int n, int p){
    for (int row = 0; row < m; row++) {
         for (int col = 0; col < p; col++) {
              T result = 0;
              for (int k = 0; k < n; k++) {
                   result += A[row*n+k] * B[k*p+col];
              }
              C[row*p+col] = result;
         }
    }
}
