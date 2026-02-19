/**
 * @file  matrix.c
 * @brief FQ_MATRIX型とFQL_MATRIX型に関する演算の関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include "matrix.h"

int Fq_matrix_init(FQ_MATRIX* R, int row, int col){

  if (row<=0){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_init] row must be > 0.\n");
#endif
    return -1;
  }
  if (col<=0){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_init] col must be > 0.\n");
#endif
    return -2;
  }

  int size = row * col;

  if (R->data!=NULL){
    free(R->data);
    R->data = NULL;
  }
  R->data = (Fq*)malloc(sizeof(Fq) * size);
  if (R->data == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_init] malloc failed.\n");
#endif
    return -3;
  }

  R->row = row;
  R->col = col;
  R->size = size;

  return 0;
}

void Fq_matrix_free(FQ_MATRIX* R){

  if (R!=NULL){
    if (R->data!=NULL){
      free(R->data);
      R->data = NULL;
    }
  }

  return;
}

void Fq_matrix_clear(FQ_MATRIX* R){

  if (R->data!=NULL){
    for(int i=0; i<(R->size); i++){
      R->data[i] = 0;
    }
  }

  return;
}

int Fq_matrix_add(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B, const QRUOV_params* para){

  if (! ((R->row)==(A->row) && (A->row)==(B->row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_add] R->row, A->row, B->row must be equal.\n");
#endif
    return -1;
  }
  if (! ((R->col)==(A->col) && (A->col)==(B->col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_add] R->col, A->col, B->col must be equal.\n");
#endif
    return -2;
  }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(A, B, R, para)
#endif
  for (i=0; i<(A->size); i++){
    R->data[i] = Fq_add(A->data[i], B->data[i], para);
  }

  return 0;
}


int Fq_matrix_sub(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B, const QRUOV_params* para){

  if (! ((R->row)==(A->row) && (A->row)==(B->row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_sub] R->row, A->row, B->row must be equal.\n");
#endif
    return -1;
  }
  if (! ((R->col)==(A->col) && (A->col)==(B->col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_sub] R->col, A->col, B->col must be equal.\n");
#endif
    return -2;
  }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(A, B, R, para)
#endif
  for (i=0; i<(A->size); i++){
    R->data[i] = Fq_sub(A->data[i], B->data[i], para);
  }

  return 0;
}

int Fq_matrix_mul(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B, const QRUOV_params* para){

  if ((A->col)!=(B->row)){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_mul] A->col must be == B->row.\n");
#endif
    return -1;
  }
  if ((R->row)!=(A->row)){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_mul] R->row must be == A->row.\n");
#endif
    return -2;
  }
  if ((R->col)!=(B->col)){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_mul] R->col must be == B->col.\n");
#endif
    return -3;
  }

  int i, j, k;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j, k) shared(A, B, R, para)
#endif
  for (i=0; i<(R->row); i++){
    for (j=0; j<(R->col); j++){

      int index_r = INDEX(i, j, R->col);
      R->data[index_r] = 0;

      for (k=0; k<(A->col); k++){
        int index_a = INDEX(i, k, A->col);
        int index_b = INDEX(k, j, B->col);
        R->data[index_r] = Fq_mul_add(A->data[index_a], B->data[index_b], R->data[index_r], para);
      }
    }
  }
  return 0;
}

int Fq_matrix_mul_op(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B){

  if ((A->col)!=(B->row)){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_mul_op] A->col must be == B->row.\n");
#endif
    return -1;
  }
  if ((R->row)!=(A->row)){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_mul_op] R->row must be == A->row.\n");
#endif
    return -2;
  }
  if ((R->col)!=(B->col)){
#ifdef DEBUG
    fprintf(stderr, "[Fq_matrix_mul_op] R->col must be == B->col.\n");
#endif
    return -3;
  }

  int i, j, k;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j, k) shared(A, B, R)
#endif
  for (i=0; i<(R->row); i++){
    for (j=0; j<(R->col); j++){

      int index_r = INDEX(i, j, R->col);
      R->data[index_r] = 0;

      for (k=0; k<((A->col)/4)+1; k++){
        int16_t vec4_a[4];
        int16_t vec4_b[4];

        for (int l=0; l<4; l++){
          if ((k*4+l)<(A->col)){
            vec4_a[l] = A->data[INDEX(i, k*4+l, A->col)];
          }else{
            vec4_a[l] = 0;
          }
          if ((k*4+l)<(B->row)){
            vec4_b[l] = B->data[INDEX(k*4+l, j, B->col)];
          }else{
            vec4_b[l] = 0;
          }
	}

        R->data[index_r]
          = Fq_vec_mul_add_op((uint64_t*)vec4_a, (uint64_t*)vec4_b, &(R->data[index_r]));
      }
    }
  }
  return 0;
}

int Fql_matrix_init(FQL_MATRIX* R, int row, int col, int symmetric, int transpose,
                    const QRUOV_params* para){

  if (row<=0){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init] row must be > 0.\n");
#endif
    return -1;
  }
  if (col<=0){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init] col must be > 0.\n");
#endif
    return -2;
  }
  if (symmetric!=0 && symmetric!=1){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init] symmetric must be 0 or 1.\n");
#endif
    return -3;
  }
  if (transpose!=0 && transpose!=1){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init] transpose must be 0 or 1.\n");
#endif
    return -4;
  }
  if (symmetric && row!=col){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init] if symmetric, row must be == col.\n");
#endif
    return -5; 
  }

  int size = 0;
  if (symmetric){
    size = (row * (row + 1)) / 2;
  }else{
    size = row * col;
  }

  if (R->data!=NULL){
    free(R->data);
    R->data = NULL;
  }

  R->data = (Fq**)malloc(sizeof(Fq*) * size);
  if (R->data == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init] matrix malloc failed.\n");
#endif
    return -6;
  }

  for (int i=0;i<size;i++) {
    R->data[i] = (Fq*)malloc(sizeof(Fq) * para->l);
    if (R->data[i] == NULL){
#ifdef DEBUG
      fprintf(stderr, "[Fql_matrix_init] Fql malloc failed.\n");
#endif
      return -7;
    }
  }

  R->row = row;
  R->col = col;
  R->size = size;
  R->symmetric = symmetric;
  R->transpose = transpose;

  return 0;
}


int Fql_matrix_init_op(FQL_MATRIX_OP* R, int row, int col, int symmetric, int transpose,
                       const QRUOV_params* para){

if (row<=0){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init_op] row must be > 0.\n");
#endif
    return -1;
  }
  if (col<=0){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init_op] col must be > 0.\n");
#endif
    return -2;
  }
  if (symmetric!=0 && symmetric!=1){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init_op] symmetric must be 0 or 1.\n");
#endif
    return -3;
  }
  if (transpose!=0 && transpose!=1){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init_op] transpose must be 0 or 1.\n");
#endif
    return -4;
  }
  if (symmetric && row!=col){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init_op] if symmetric, row must be == col.\n");
#endif
    return -5; 
  }

  int size = 0;
  if (symmetric){
    size = (row * (row + 1)) / 2;
  }else{
    size = row * col;
  }

  if (R->data!=NULL){
    free(R->data);
    R->data = NULL;
  }

  R->data = (Fql*)malloc(sizeof(Fql) * size);
  if (R->data == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_init_op] matrix malloc failed.\n");
#endif
    return -6;
  }

  memset(R->data, 0, sizeof(Fql) * size);

  R->row = row;
  R->col = col;
  R->size = size;
  R->symmetric = symmetric;
  R->transpose = transpose;

  return 0;
}

void Fql_matrix_free(FQL_MATRIX* R){

  if (R!=NULL){
    if (R->data!=NULL){
      for (int i=0;i<R->size;i++) {
        if(R->data[i]!=NULL){
          free(R->data[i]);
          R->data[i] = NULL;
        }
      }

      free(R->data);
      R->data = NULL;
    }
  }

  return;
}

void Fql_matrix_free_op(FQL_MATRIX_OP* R){

  if (R!=NULL){
    if (R->data!=NULL){
      free(R->data);
      R->data = NULL;
    }
  }

  return;
}

void Fql_matrix_clear(FQL_MATRIX* R, const QRUOV_params* para){

  if (R->data!=NULL){
    for(int i=0; i<(R->size); i++){
      Fql_zero(R->data[i], para);
    }
  }

  return;
}

void Fql_matrix_clear_op(FQL_MATRIX_OP* R, const QRUOV_params* para){

  if (R->data!=NULL){
    for(int i=0; i<(R->size); i++){
      R->data[i] = 0;
    }
  }

  return;
}

void Fql_matrix_transpose(FQL_MATRIX* R){

  if (R->transpose) R->transpose = 0;
  else R->transpose = 1;

  return;
}

void Fql_matrix_transpose_op(FQL_MATRIX_OP* R){

  if (R->transpose) R->transpose = 0;
  else R->transpose = 1;

  return;
}

int Fql_matrix_copy(FQL_MATRIX* R, const FQL_MATRIX* A, const QRUOV_params* para){

  if ((R->row)!=(A->row)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy] R->row must be == A->row.\n");
#endif
    return -1;
  }
  if ((R->col)!=(A->col)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy] R->col must be == A->col.\n");
#endif
    return -2;
  }
  if ((R->symmetric)!=(A->symmetric)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy] R->symmetric must be == A->symmetric.\n");
#endif
    return -3;
  }
  if ((R->transpose)!=(A->transpose)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy] R->transpose must be == A->transpose.\n");
#endif
    return -4;
  }

  for (int i=0; i<(R->size); i++){
    Fql_copy(R->data[i], A->data[i], para);
  }

  return 0;
}

int Fql_matrix_copy_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A){

  if ((R->row)!=(A->row)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy_op] R->row must be == A->row.\n");
#endif
    return -1;
  }
  if ((R->col)!=(A->col)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy_op] R->col must be == A->col.\n");
#endif
    return -2;
  }
  if ((R->symmetric)!=(A->symmetric)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy_op] R->symmetric must be == A->symmetric.\n");
#endif
    return -3;
  }
  if ((R->transpose)!=(A->transpose)){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_copy_op] R->transpose must be == A->transpose.\n");
#endif
    return -4;
  }

  for (int i=0; i<(R->size); i++){
    R->data[i] = A->data[i];
  }

  return 0;
}

int Fql_matrix_add(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                   const QRUOV_params* para){

  int R_row = (R->transpose) ? (R->col) : (R->row);
  int R_col = (R->transpose) ? (R->row) : (R->col);
  int A_row = (A->transpose) ? (A->col) : (A->row);
  int A_col = (A->transpose) ? (A->row) : (A->col);
  int B_row = (B->transpose) ? (B->col) : (B->row);
  int B_col = (B->transpose) ? (B->row) : (B->col);

  if (! ((R_row)==(A_row) && (A_row)==(B_row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_add] R->row (or transposed R->col), "
                    "A->row (or transposed A->col), B->row (or transposed B->col) "
                    "must be equal.\n");
#endif
    return -1;
  }
  if (! ((R_col)==(A_col) && (A_col)==(B_col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_add] R->col (or transposed R->row), "
                    "A->col (or transposed A->row), B->col (or transposed B->row) "
                    "must be equal.\n");
#endif
    return -2;
  }
 
  int i, j = 0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j) shared(A, B, R, para)
#endif
  for (i=0; i<A_row; i++){
    for(j=0; j<A_col; j++){

      int index_r, index_a, index_b = 0;
      if (R->transpose){
        if (R->symmetric) index_r = SYMMETRIC_INDEX(j, i, R->col);
        else              index_r = INDEX(j, i, R->col);
      }else{
        if (R->symmetric) index_r = SYMMETRIC_INDEX(i, j, R->col);
        else              index_r = INDEX(i, j, R->col);
      }

      if (A->transpose){
        if (A->symmetric) index_a = SYMMETRIC_INDEX(j, i, A->col);
        else              index_a = INDEX(j, i, A->col);
      }else{
        if (A->symmetric) index_a = SYMMETRIC_INDEX(i, j, A->col);
        else              index_a = INDEX(i, j, A->col);
      }

      if (B->transpose){
        if (B->symmetric) index_b = SYMMETRIC_INDEX(j, i, B->col);
        else              index_b = INDEX(j, i, B->col);
      }else{
        if (B->symmetric) index_b = SYMMETRIC_INDEX(i, j, B->col);
        else              index_b = INDEX(i, j, B->col);
      }

      Fql_add(R->data[index_r], A->data[index_a], B->data[index_b], para);
    }
  }

  return 0;
}


int Fql_matrix_add_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                      const QRUOV_params* para){

  int R_row = (R->transpose) ? (R->col) : (R->row);
  int R_col = (R->transpose) ? (R->row) : (R->col);
  int A_row = (A->transpose) ? (A->col) : (A->row);
  int A_col = (A->transpose) ? (A->row) : (A->col);
  int B_row = (B->transpose) ? (B->col) : (B->row);
  int B_col = (B->transpose) ? (B->row) : (B->col);

  if (! ((R_row)==(A_row) && (A_row)==(B_row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_add_op] R->row (or transposed R->col), "
                    "A->row (or transposed A->col), B->row (or transposed B->col) "
                    "must be equal.\n");
#endif
    return -1;
  }
  if (! ((R_col)==(A_col) && (A_col)==(B_col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_add_op] R->col (or transposed R->row), "
                    "A->col (or transposed A->row), B->col (or transposed B->row) "
                    "must be equal.\n");
#endif
    return -2;
  }
 
  int i, j = 0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j) shared(A, B, R, para)
#endif
  for (i=0; i<A_row; i++){
    for(j=0; j<A_col; j++){

      int index_r, index_a, index_b = 0;
      if (R->transpose){
        if (R->symmetric) index_r = SYMMETRIC_INDEX(j, i, R->col);
        else              index_r = INDEX(j, i, R->col);
      }else{
        if (R->symmetric) index_r = SYMMETRIC_INDEX(i, j, R->col);
        else              index_r = INDEX(i, j, R->col);
      }

      if (A->transpose){
        if (A->symmetric) index_a = SYMMETRIC_INDEX(j, i, A->col);
        else              index_a = INDEX(j, i, A->col);
      }else{
        if (A->symmetric) index_a = SYMMETRIC_INDEX(i, j, A->col);
        else              index_a = INDEX(i, j, A->col);
      }

      if (B->transpose){
        if (B->symmetric) index_b = SYMMETRIC_INDEX(j, i, B->col);
        else              index_b = INDEX(j, i, B->col);
      }else{
        if (B->symmetric) index_b = SYMMETRIC_INDEX(i, j, B->col);
        else              index_b = INDEX(i, j, B->col);
      }
      Fql_add_op(&(R->data[index_r]), &(A->data[index_a]), &(B->data[index_b]));
    }
  }

  return 0;
}

int Fql_matrix_sub(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                   const QRUOV_params* para){

  int R_row = (R->transpose) ? (R->col) : (R->row);
  int R_col = (R->transpose) ? (R->row) : (R->col);
  int A_row = (A->transpose) ? (A->col) : (A->row);
  int A_col = (A->transpose) ? (A->row) : (A->col);
  int B_row = (B->transpose) ? (B->col) : (B->row);
  int B_col = (B->transpose) ? (B->row) : (B->col);

  if (! ((R_row)==(A_row) && (A_row)==(B_row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_sub] R->row (or transposed R->col), "
                    "A->row (or transposed A->col), B->row (or transposed B->col) "
                    "must be equal.\n");
#endif
    return -1;
  }
  if (! ((R_col)==(A_col) && (A_col)==(B_col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_sub] R->col (or transposed R->row), "
                    "A->col (or transposed A->row), B->col (or transposed B->row) "
                    "must be equal.\n");
#endif
    return -2;
  }

  int i, j = 0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j) shared(A, B, R, para)
#endif
  for (i=0; i<A_row; i++){
    for(j=0; j<A_col; j++){

      int index_r, index_a, index_b = 0;
      if (R->transpose){
        if (R->symmetric) index_r = SYMMETRIC_INDEX(j, i, R->col);
        else              index_r = INDEX(j, i, R->col);
      }else{
        if (R->symmetric) index_r = SYMMETRIC_INDEX(i, j, R->col);
        else              index_r = INDEX(i, j, R->col);
      }

      if (A->transpose){
        if (A->symmetric) index_a = SYMMETRIC_INDEX(j, i, A->col);
        else              index_a = INDEX(j, i, A->col);
      }else{
        if (A->symmetric) index_a = SYMMETRIC_INDEX(i, j, A->col);
        else              index_a = INDEX(i, j, A->col);
      }

      if (B->transpose){
        if (B->symmetric) index_b = SYMMETRIC_INDEX(j, i, B->col);
        else              index_b = INDEX(j, i, B->col);
      }else{
        if (B->symmetric) index_b = SYMMETRIC_INDEX(i, j, B->col);
        else              index_b = INDEX(i, j, B->col);
      }

      Fql_sub(R->data[index_r], A->data[index_a], B->data[index_b], para);

    }
  }

  return 0;
}


int Fql_matrix_sub_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                      const QRUOV_params* para){

  int R_row = (R->transpose) ? (R->col) : (R->row);
  int R_col = (R->transpose) ? (R->row) : (R->col);
  int A_row = (A->transpose) ? (A->col) : (A->row);
  int A_col = (A->transpose) ? (A->row) : (A->col);
  int B_row = (B->transpose) ? (B->col) : (B->row);
  int B_col = (B->transpose) ? (B->row) : (B->col);

  if (! ((R_row)==(A_row) && (A_row)==(B_row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_sub_op] R->row (or transposed R->col), "
                    "A->row (or transposed A->col), B->row (or transposed B->col) "
                    "must be equal.\n");
#endif
    return -1;
  }
  if (! ((R_col)==(A_col) && (A_col)==(B_col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_sub_op] R->col (or transposed R->row), "
                    "A->col (or transposed A->row), B->col (or transposed B->row) "
                    "must be equal.\n");
#endif
    return -2;
  }

  int i, j = 0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j) shared(A, B, R, para)
#endif
  for (i=0; i<A_row; i++){
    for(j=0; j<A_col; j++){

      int index_r, index_a, index_b = 0;
      if (R->transpose){
        if (R->symmetric) index_r = SYMMETRIC_INDEX(j, i, R->col);
        else              index_r = INDEX(j, i, R->col);
      }else{
        if (R->symmetric) index_r = SYMMETRIC_INDEX(i, j, R->col);
        else              index_r = INDEX(i, j, R->col);
      }

      if (A->transpose){
        if (A->symmetric) index_a = SYMMETRIC_INDEX(j, i, A->col);
        else              index_a = INDEX(j, i, A->col);
      }else{
        if (A->symmetric) index_a = SYMMETRIC_INDEX(i, j, A->col);
        else              index_a = INDEX(i, j, A->col);
      }

      if (B->transpose){
        if (B->symmetric) index_b = SYMMETRIC_INDEX(j, i, B->col);
        else              index_b = INDEX(j, i, B->col);
      }else{
        if (B->symmetric) index_b = SYMMETRIC_INDEX(i, j, B->col);
        else              index_b = INDEX(i, j, B->col);
      }

      Fql_sub_op(&(R->data[index_r]), &(A->data[index_a]), &(B->data[index_b]));

    }
  }

  return 0;
}

int Fql_matrix_mul(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                   const QRUOV_params* para){

  int ret = 0;

  int row_1 = (A->transpose) ? (A->col) : (A->row);
  int col_1 = (A->transpose) ? (A->row) : (A->col);
  int row_2 = (B->transpose) ? (B->col) : (B->row);
  int col_2 = (B->transpose) ? (B->row) : (B->col);

  if (col_1!=row_2){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] A->col (or transposed A->row) "
                    "must be == B->row (or transposed B->col).\n");
#endif
    return -1;
  }
  if ((R->row)!=row_1){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] R->row must be == A->row (or transposed A->col).\n");
#endif
    return -2;
  }
  if ((R->col)!=col_2){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] R->col must be == B->col (or transposed B->row).\n");
#endif
    return -3;
  }
  if (R->transpose){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] R must not be transpose.\n");
#endif
    return -4;
  }

  int len = col_1;

  int i, j, k;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j, k) shared(ret, len, A, B, R, para)
#endif
  for (i=0; i<(R->row); i++){
    for (j=0; j<(R->col); j++){

      int ret_for = 0;

      Fq* r = NULL;
      ret_for = Fql_accumulator_init(&r, para);
      if (ret_for!=0){
        ret = ret_for-10;
#ifndef QRUOV_USE_MULTI_THREAD
        return ret;
#endif
        goto end_for;
      }
      Fql_accumulator_zero(r, para);

      int index_r;
      if (R->symmetric){
        if (i>j) continue;
        else     index_r = SYMMETRIC_INDEX(i, j, R->col);
      }else{
        index_r = INDEX(i, j, R->col);
      }

      for (k=0; k<len; k++){

        int index_a, index_b;
        if (A->transpose){
          if (A->symmetric) index_a = SYMMETRIC_INDEX(k, i, A->col);
          else              index_a = INDEX(k, i, A->col);
        }else{
          if (A->symmetric) index_a = SYMMETRIC_INDEX(i, k, A->col);
          else              index_a = INDEX(i, k, A->col);
        }
        if (B->transpose){
          if (B->symmetric) index_b = SYMMETRIC_INDEX(j, k, B->col);
          else              index_b = INDEX(j, k, B->col);
        }else{
          if (B->symmetric) index_b = SYMMETRIC_INDEX(k, j, B->col);
          else              index_b = INDEX(k, j, B->col);
        }

        Fq* c = NULL;
        ret_for = Fql_accumulator_init(&c, para);
        if (ret_for!=0){
          ret = ret_for-20;
	  Fql_accumulator_free(&r);
#ifndef QRUOV_USE_MULTI_THREAD
          return ret;
#endif
          goto end_for;
        }
        Fql_accumulator_zero(c, para);

        Fql_mul(c, A->data[index_a], B->data[index_b], para);
        Fql_accumulator_add(r, r, c, para);
        Fql_accumulator_free(&c);
      }

      ret_for = Fql_accumulator_reduce(R->data[index_r], r, para);
        if (ret_for!=0){
          ret = ret_for-30;
          Fql_accumulator_free(&r);
#ifndef QRUOV_USE_MULTI_THREAD
          return ret;
#endif
          goto end_for;
        }
      Fql_accumulator_free(&r);
    }
    end_for: // for multi thread
    i=i; // dummy
  }

  return ret;
}

int Fql_matrix_mul_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                      const QRUOV_params* para){

  int ret = 0;

  int row_1 = (A->transpose) ? (A->col) : (A->row);
  int col_1 = (A->transpose) ? (A->row) : (A->col);
  int row_2 = (B->transpose) ? (B->col) : (B->row);
  int col_2 = (B->transpose) ? (B->row) : (B->col);

  if (col_1!=row_2){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] A->col (or transposed A->row) "
                    "must be == B->row (or transposed B->col).\n");
#endif
    return -1;
  }
  if ((R->row)!=row_1){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] R->row must be == A->row (or transposed A->col).\n");
#endif
    return -2;
  }
  if ((R->col)!=col_2){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] R->col must be == B->col (or transposed B->row).\n");
#endif
    return -3;
  }
  if (R->transpose){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul] R must not be transpose.\n");
#endif
    return -4;
  }

  int len = col_1;

  int i, j, k;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j, k) shared(ret, len, A, B, R, para)
#endif
  for (i=0; i<(R->row); i++){
    for (j=0; j<(R->col); j++){

      Fql r = 0;
      int index_r;
      if (R->symmetric){
        if (i>j) continue;
        else     index_r = SYMMETRIC_INDEX(i, j, R->col);
      }else{
        index_r = INDEX(i, j, R->col);
      }

      for (k=0; k<len; k++){

        int index_a, index_b;
        if (A->transpose){
          if (A->symmetric) index_a = SYMMETRIC_INDEX(k, i, A->col);
          else              index_a = INDEX(k, i, A->col);
        }else{
          if (A->symmetric) index_a = SYMMETRIC_INDEX(i, k, A->col);
          else              index_a = INDEX(i, k, A->col);
        }
        if (B->transpose){
          if (B->symmetric) index_b = SYMMETRIC_INDEX(j, k, B->col);
          else              index_b = INDEX(j, k, B->col);
        }else{
          if (B->symmetric) index_b = SYMMETRIC_INDEX(k, j, B->col);
          else              index_b = INDEX(k, j, B->col);
        }

        Fql_mul_add_op(&r, &(A->data[index_a]), &(B->data[index_b]), &r);
      }
      R->data[index_r] = r;
    }
  }

  return ret;
}

int Fql_matrix_mul_add(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                       const FQL_MATRIX* C, const QRUOV_params* para){

  int ret = 0;

  int row_1 = (A->transpose) ? (A->col) : (A->row);
  int col_1 = (A->transpose) ? (A->row) : (A->col);
  int row_2 = (B->transpose) ? (B->col) : (B->row);
  int col_2 = (B->transpose) ? (B->row) : (B->col);

  if (col_1!=row_2){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add] A->col (or transposed A->row) "
                    "must be == B->row (or transposed B->col).\n");
#endif
    return -1;
  }
  if (! ((R->row)==row_1 && row_1==(C->row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add] R->row, A->row (or transposed A->col), C->row "
                    "must be equal.\n");
#endif
    return -2;
  }
  if (! ((R->col)==col_2 && col_2==(C->col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add] R->col, B->col (or transposed B->row), C->col "
                    "must be equal.\n");
#endif
    return -3;
  }
  if (R->transpose){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add] R must not be transpose.\n");
#endif
    return -4;
  }
  if (C->transpose){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add] C must not be transpose.\n");
#endif
    return -5;
  }

  int len = col_1;

  int i, j, k;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j, k) shared(ret, len, A, B, C, R, para)
#endif
  for (i=0; i<(R->row); i++){
    for (j=0; j<(R->col); j++){

      int ret_for = 0;

      Fq* r = NULL;
      ret_for = Fql_accumulator_init(&r, para);
      if (ret_for!=0){
        ret = ret_for-10;
#ifndef QRUOV_USE_MULTI_THREAD
        return ret;
#endif
        goto end_for;
      }
      Fql_accumulator_zero(r, para);

      int index_r, index_c;
      if (R->symmetric){
        if (i>j) continue;
        else     index_r = SYMMETRIC_INDEX(i, j, R->col);
      }else{
        index_r = INDEX(i, j, R->col);
      }

      if (C->symmetric) index_c = SYMMETRIC_INDEX(i, j, C->col);
      else              index_c = INDEX(i, j, C->col);

      for (k=0; k<len; k++){

        int index_a, index_b;
        if (A->transpose){
          if (A->symmetric) index_a = SYMMETRIC_INDEX(k, i, A->col);
          else              index_a = INDEX(k, i, A->col);
        }else{
          if (A->symmetric) index_a = SYMMETRIC_INDEX(i, k, A->col);
          else              index_a = INDEX(i, k, A->col);
        }
        if (B->transpose){
          if (B->symmetric) index_b = SYMMETRIC_INDEX(j, k, B->col);
          else              index_b = INDEX(j, k, B->col);
        }else{
          if (B->symmetric) index_b = SYMMETRIC_INDEX(k, j, B->col);
          else              index_b = INDEX(k, j, B->col);
        }

        Fq* c = NULL;
        ret_for = Fql_accumulator_init(&c, para);
        if (ret_for!=0){
          ret = ret_for-20;
	  Fql_accumulator_free(&r);
#ifndef QRUOV_USE_MULTI_THREAD
          return ret;
#endif
          goto end_for;
        }
        Fql_accumulator_zero(c, para);

        Fql_mul(c, A->data[index_a], B->data[index_b], para);
        Fql_accumulator_add(r, r, c, para);
        Fql_accumulator_free(&c);
      }

      Fq* s = NULL;
      ret_for = Fql_init(&s, para);
      if (ret_for!=0){
        ret = ret_for-30;
        Fql_accumulator_free(&r);
#ifndef QRUOV_USE_MULTI_THREAD
        return ret;
#endif
        goto end_for;
      }
      Fql_zero(s, para);

      ret_for = Fql_accumulator_reduce(s, r, para);
      if (ret_for!=0){
        ret = ret_for-40;
        Fql_accumulator_free(&r);
        Fql_free(&s);
#ifndef QRUOV_USE_MULTI_THREAD
        return ret;
#endif
        goto end_for;
      }

      Fql_add(s, s, C->data[index_c], para);
      Fql_copy(R->data[index_r], s, para);
      Fql_free(&s);
      Fql_accumulator_free(&r);
    }
    end_for: // for multi thread
    i=i; // dummy
  }

  return ret;
}


int Fql_matrix_mul_add_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                          const FQL_MATRIX_OP* C, const QRUOV_params* para){

  int ret = 0;

  int row_1 = (A->transpose) ? (A->col) : (A->row);
  int col_1 = (A->transpose) ? (A->row) : (A->col);
  int row_2 = (B->transpose) ? (B->col) : (B->row);
  int col_2 = (B->transpose) ? (B->row) : (B->col);

  if (col_1!=row_2){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add_op] A->col (or transposed A->row) "
                    "must be == B->row (or transposed B->col).\n");
#endif
    return -1;
  }
  if (! ((R->row)==row_1 && row_1==(C->row)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add_op] R->row, A->row (or transposed A->col), C->row "
                    "must be equal.\n");
#endif
    return -2;
  }
  if (! ((R->col)==col_2 && col_2==(C->col)) ){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add_op] R->col, B->col (or transposed B->row), C->col "
                    "must be equal.\n");
#endif
    return -3;
  }
  if (R->transpose){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add_op] R must not be transpose.\n");
#endif
    return -4;
  }
  if (C->transpose){
#ifdef DEBUG
    fprintf(stderr, "[Fql_matrix_mul_add_op] C must not be transpose.\n");
#endif
    return -5;
  }

  int len = col_1;

  int i, j, k;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j, k) shared(ret, len, A, B, C, R, para)
#endif
  for (i=0; i<(R->row); i++){
    for (j=0; j<(R->col); j++){

      Fql r = 0;
      int index_r, index_c;
      if (R->symmetric){
        if (i>j) continue;
        else     index_r = SYMMETRIC_INDEX(i, j, R->col);
      }else{
        index_r = INDEX(i, j, R->col);
      }

      if (C->symmetric) index_c = SYMMETRIC_INDEX(i, j, C->col);
      else              index_c = INDEX(i, j, C->col);

      for (k=0; k<len; k++){

        int index_a, index_b;
        if (A->transpose){
          if (A->symmetric) index_a = SYMMETRIC_INDEX(k, i, A->col);
          else              index_a = INDEX(k, i, A->col);
        }else{
          if (A->symmetric) index_a = SYMMETRIC_INDEX(i, k, A->col);
          else              index_a = INDEX(i, k, A->col);
        }
        if (B->transpose){
          if (B->symmetric) index_b = SYMMETRIC_INDEX(j, k, B->col);
          else              index_b = INDEX(j, k, B->col);
        }else{
          if (B->symmetric) index_b = SYMMETRIC_INDEX(k, j, B->col);
          else              index_b = INDEX(k, j, B->col);
        }

        Fql_mul_add_op(&r, &(A->data[index_a]), &(B->data[index_b]), &r);
      }

      Fql s = 0;
      Fql_add_op(&s, &r, &(C->data[index_c]));
      R->data[index_r] = s;
    }
  }

  return ret;
}

int Fql_index_permute(int* index_a, const int index_aW, const QRUOV_params* para){

  if (index_aW<0){
#ifdef DEBUG
  fprintf(stderr, "[Fql_index_permute] index_aW must be >= 0.\n");
#endif
    return -1;
  }

  if (index_aW>=para->l){
#ifdef DEBUG
  fprintf(stderr, "[Fql_index_permute] index_aW must be < para->l.\n");
#endif
    return -2;
  }

  *index_a = (para->l + para->f_e - 1 - index_aW) % para->l; 

  return 0;
}
