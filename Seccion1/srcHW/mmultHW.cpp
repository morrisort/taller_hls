#include "mmultHW.h"


void mmultHW (T *A, T *B, T *C, int m, int n, int p){

    loopRow:for (int row = 0; row < m; row++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
         loopCol:for (int col = 0; col < p; col++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
              T result = 0;
              ProductoPunto:for (int k = 0; k < n; k++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
                   result += A[row*n+k] * B[k*p+col];
              }
              C[row*p+col] = result;
         }
    }
}
