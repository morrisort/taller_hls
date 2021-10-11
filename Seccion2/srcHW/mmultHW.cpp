#include "mmultHW.h"


void mmultHW (T A[M][N], T B[N][P], T C[M][P]){
#pragma HLS ARRAY_PARTITION variable=A complete dim=2
#pragma HLS ARRAY_PARTITION variable=B complete dim=1
#pragma HLS ARRAY_PARTITION variable=C complete dim=1
    loopRow:for (int row = 0; row < M; row++) {
         loopCol:for (int col = 0; col < P; col++) {
#pragma HLS pipeline
              T result = 0;
              ProductoPunto:for (int k = 0; k < N; k++) {
#pragma HLS unroll
                   result += A[row][k] * B[k][col];
              }
              C[row][col] = result;
         }
    }
}
