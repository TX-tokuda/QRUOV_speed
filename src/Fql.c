/**
 * @file  Fql.c
 * @brief FqとFqlに関する演算の関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include <stdio.h>

#include "Fql.h"

extern Fq Fq_add(Fq a, Fq b, const QRUOV_params* para);
extern Fq Fq_sub(Fq a, Fq b, const QRUOV_params* para);
extern Fq Fq_mul(Fq a, Fq b, const QRUOV_params* para);

int Fql_init(Fq** r, const QRUOV_params* para){

  if (*r!=NULL){
    free(*r);
    *r = NULL;
  }

  *r = (Fq*)malloc(sizeof(Fq) * (para->Fql_degree+1));
  if (*r == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fql_init] malloc failed.\n");
#endif
    return -1;
  }

  return 0;
}

void Fql_free(Fq** r){

  if (r!=NULL){
    if (*r!=NULL){
      free(*r);
      *r = NULL;
    }
  }

  return;
}

void Fql_zero(Fq* r, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_degree+1); i++){
    r[i] = 0;
  }

  return;
}

void Fql_copy(Fq* r, const Fq* a, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_degree+1); i++){
    r[i] = a[i];
  }

  return;
}

void Fql_copy_op2(Fq* r, const Fq* a, const QRUOV_params* para){

  for (int i=0; i<3; i++){
    r[i] = a[i];
  }

  return;
}

void Fql_add(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_degree+1); i++){
    r[i] = Fq_add(a[i], b[i], para);
  }

  return;
}

void Fql_add_op(Fql* r, const Fql* a, const Fql* b){

  // 和が127の時にも正しくmod127が行われるように、1を足しておく
  Fql t = ((*a) + (*b) + 0x0001000100010001);
  *r =   ((t & 0xc000c000c000c000) >> 14)
       + ((t & 0x3f803f803f803f80) >> 7)
       +  (t & 0x007f007f007f007f)
       - 0x0001000100010001;
  t = 0;

  return;
}

void Fql_add_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_degree+1); i++){
    r[i] = Fq_add_op2(a[i], b[i], para);
  }

  return;
}

void Fql_sub(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_degree+1); i++){
    r[i] = Fq_sub(a[i], b[i], para);
  }

  return;
}

void Fql_sub_op(Fql* r, const Fql* a, const Fql* b){

  // 差が0の時にも正しくmod127が行われるように、128を足しておく
  Fql t = 0x0080008000800080 + (*a) - (*b);
  *r =   ((t & 0xc000c000c000c000) >> 14)
       + ((t & 0x3f803f803f803f80) >> 7)
       +  (t & 0x007f007f007f007f)
       - 0x0001000100010001;
  t = 0;

  return;
}

void Fql_sub_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_degree+1); i++){
    r[i] = Fq_sub_op2(a[i], b[i], para);
  }

  return;
}

void Fql_mul(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  Fql_accumulator_zero(r, para);

  for (int i=0; i<(para->Fql_degree+1); i++){
    for (int j=0; j<(para->Fql_degree+1); j++){
      r[i+j] = Fq_add(r[i+j], Fq_mul(a[i], b[j], para), para);
    }
  }

  return;
}

void Fql_mul_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  Fql_accumulator_zero(r, para);

  for (int i=0; i<3; i++){
    for (int j=0; j<3; j++){
      r[i+j] = Fq_add_op2(r[i+j], Fq_mul_op2(a[i], b[j], para), para);
    }
  }

  return;
}

const uint64_t mod127_tbl[258] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x00,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 
  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
  0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
  0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
  0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
  0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x00, 0x01,
  0x02, 0x03
};

int Fql_accumulator_init(Fq** r, const QRUOV_params* para){

  if (*r!=NULL){
    free(*r);
    *r = NULL;
  }

  *r = (Fq*)malloc(sizeof(Fq) * (para->Fql_accumulator_degree+1));
  if (*r == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fql_accumulator_init] malloc failed.\n");
#endif
    return -1;
  }

  return 0;
}

void Fql_accumulator_free(Fq** r){

  if (r!=NULL){
    if (*r!=NULL){
      free(*r);
      *r = NULL;
    }
  }

  return;
}

void Fql_accumulator_zero(Fq* r, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_accumulator_degree+1); i++){
    r[i] = 0;
  }

  return;
}

void Fql_accumulator_add(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_accumulator_degree+1); i++){
    r[i] = Fq_add(a[i], b[i], para);
  }

  return;
}

void Fql_accumulator_add_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para){

  for (int i=0; i< 5; i++){
    r[i] = Fq_add_op2(a[i], b[i], para);
  }

  return;
}

void Fql_accumulator_copy(Fq* r, const Fq* a, const QRUOV_params* para){

  for (int i=0; i<(para->Fql_accumulator_degree+1); i++){
    r[i] = a[i];
  }

  return;
}

int Fql_accumulator_reduce(Fq* r, const Fq* a, const QRUOV_params* para){

  int* tmp = NULL;
  tmp = (int*)malloc(sizeof(int) * (para->Fql_accumulator_degree+1));
  if (tmp == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fql_accumulator_reduce] malloc failed.\n");
#endif
    return -1;
  }

  for (int i=0; i<(para->Fql_accumulator_degree+1); i++){
    tmp[i] = a[i];
  }

  for (int i=para->Fql_accumulator_degree; i >= para->l; i--){
    tmp[i - para->l] += para->f_c0 * tmp[i];
    tmp[i - para->l + para->f_e] += para->f_c * tmp[i];
  }

  for(int i=0; i<(para->Fql_degree+1); i++){
    r[i] = tmp[i] % para->q;
  }

  free(tmp);
  tmp = NULL;

  return 0;
}

int Fql_accumulator_reduce_op2(Fq* r, const Fq* a, const QRUOV_params* para){

  int* tmp = NULL;
  tmp = (int*)malloc(sizeof(int) * (para->Fql_accumulator_degree+1));
  if (tmp == NULL){
#ifdef DEBUG
    fprintf(stderr, "[Fql_accumulator_reduce] malloc failed.\n");
#endif
    return -1;
  }

  for (int i=0; i<5; i++){
    tmp[i] = a[i];
  }

  for (int i=4; i >= 3; i--){
    tmp[i - 3] += tmp[i];
    tmp[i - 2] += tmp[i];
  }

  for(int i=0; i<(3); i++){
    r[i] = tmp[i] % 31;
  }

  free(tmp);
  tmp = NULL;

  return 0;
}

int Fql2Fq(Fq* r, const Fq* a, const int degree, const QRUOV_params* para){

  if(degree>para->Fql_degree){
#ifdef DEBUG
    fprintf(stderr, "[Fql2Fq] degree must be <= para->Fql_degree.\n");
#endif
    return -1;
  }

  *r = a[degree];

  return 0;
}

int Fql2Fq_op(Fq* r, const Fql* a, const int degree, const QRUOV_params* para){

  if(degree>para->Fql_degree){
#ifdef DEBUG
    fprintf(stderr, "[Fql2Fq_op] degree must be <= para->Fql_degree.\n");
#endif
    return -1;
  }

  *r = (*a>>(degree*16)) & 0x7F;

  return 0;
}
