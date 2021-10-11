#include "mmultHW.h"


void mmultHW (T A_in[M][N], T B_in[N][P], T C_out[M][P]){

     T A[M][N];
     T B[N][P];
     T C[M][P];
#pragma HLS ARRAY_PARTITION variable=A complete dim=2
#pragma HLS ARRAY_PARTITION variable=B complete dim=1
//#pragma HLS ARRAY_PARTITION variable=C complete dim=1

     // Store inputs
     loadA:for (int r=0; r<M; r++){
          for (int c=0; c<N; c++){
               A[r][c] = A_in[r][c];
          }
     }
     loadB:for (int r=0; r<N; r++){
          for (int c=0; c<P; c++){
               B[r][c] = B_in[r][c];
          }
     }

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
     outC:for (int r=0; r<M; r++){
          for (int c=0; c<P; c++){
               C_out[r][c] = C[r][c];
          }
     }
}
