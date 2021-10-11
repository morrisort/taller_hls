#include "mmultSW.h"


void mmultSW (T A[M][N], T B[N][P], T C[M][P]){
    for (int row = 0; row < M; row++) {
         for (int col = 0; col < P; col++) {
              T result = 0;
              for (int k = 0; k < N; k++) {
                   result += A[row][k] * B[k][col];
              }
              C[row][col] = result;
         }
    }
}
