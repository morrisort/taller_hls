#include "mmultSW.h"


void mmultSW (T *A, T *B, T *C){
    for (int row = 0; row < M_MMULT; row++) {
         for (int col = 0; col < P_MMULT; col++) {
              T result = 0;
              for (int k = 0; k < N_MMULT; k++) {
                   result += A[row*N_MMULT +k] * B[k*P_MMULT +col];
              }
              C[row*P_MMULT +col] = result;
         }
    }
}
