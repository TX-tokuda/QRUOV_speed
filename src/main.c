/**
 * @file  main.c
 * @brief main関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parameter.h"
#include "qruov.h"

//TX add
void print_QRUOV_params(const QRUOV_params* params) {
    if (params == NULL) {
        printf("params is NULL\n");
        return;
    }

    printf("QRUOV_params:\n");
    printf("  parameter = %d\n", params->parameter);
    printf("  SL = %d\n", params->SL);
    printf("  q = %d\n", params->q);
    printf("  v = %d\n", params->v);
    printf("  m = %d\n", params->m);
    printf("  l = %d\n", params->l);
    printf("  tau_lVM = %d\n", params->tau_lVM);
    printf("  tau_lVV12 = %d\n", params->tau_lVV12);
    printf("  tau_lV = %d\n", params->tau_lV);
    printf("  tau_m = %d\n", params->tau_m);
    printf("  lambda = %d\n", params->lambda);
    printf("  q_bitlen = %d\n", params->q_bitlen);
    printf("  Fq_inv_table = %p\n", (const void*)params->Fq_inv_table);
    printf("  n = %d\n", params->n);
    printf("  V = %d\n", params->V);
    printf("  M = %d\n", params->M);
    printf("  N = %d\n", params->N);
    printf("  Fql_degree = %d\n", params->Fql_degree);
    printf("  Fql_accumulator_degree = %d\n", params->Fql_accumulator_degree);
    printf("  seed_len = %d\n", params->seed_len);
    printf("  salt_len = %d\n", params->salt_len);
    printf("  f_c = %d\n", params->f_c);
    printf("  f_e = %d\n", params->f_e);
    printf("  f_c0 = %d\n", params->f_c0);
    printf("  sk_len = %zu\n", params->sk_len);
    printf("  pk_len = %zu\n", params->pk_len);
    printf("  sigma_len = %zu\n", params->sigma_len);
    printf("  prg_shake = %d\n", params->prg_shake);
    printf("  prg_ctx = %p\n", params->prg_ctx);
    printf("  SHAKE_len = %d\n", params->SHAKE_len);
    printf("  AES_len = %d\n", params->AES_len);
}
//TX add

int main(){

  int ret = 0;

  // paramgen
  QRUOV_params para;

  //--- 試験範囲 ---//
  //ret = QRUOV_paramgen("qruov1q127L3v156m54", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov3q127L3v228m78", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov1q127L3v156m54", "AES", &para);
  //ret = QRUOV_paramgen("qruov3q127L3v228m78", "AES", &para);

  //--- 試験範囲外 ---//
  ret = QRUOV_paramgen("qruov1q31L3v165m60", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov1q31L10v600m70", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov1q7L10v740m100", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov1q31L3v165m60", "AES", &para);
  //ret = QRUOV_paramgen("qruov1q31L10v600m70", "AES", &para);
  //ret = QRUOV_paramgen("qruov1q7L10v740m100", "AES", &para);

  //ret = QRUOV_paramgen("qruov3q31L3v246m87", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov3q31L10v890m100", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov3q7L10v1100m140", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov3q31L3v246m87", "AES", &para);
  //ret = QRUOV_paramgen("qruov3q31L10v890m100", "AES", &para);
  //ret = QRUOV_paramgen("qruov3q7L10v1100m140", "AES", &para);

  //ret = QRUOV_paramgen("qruov5q127L3v306m105", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov5q31L3v324m114", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov5q31L10v1120m120", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov5q7L10v1490m190", "SHAKE", &para);
  //ret = QRUOV_paramgen("qruov5q127L3v306m105", "AES", &para);
  //ret = QRUOV_paramgen("qruov5q31L3v324m114", "AES", &para);
  //ret = QRUOV_paramgen("qruov5q31L10v1120m120", "AES", &para);
  //ret = QRUOV_paramgen("qruov5q7L10v1490m190", "AES", &para);

  if (ret!=0) printf("QRUOV_paramgen returned %d.\n", ret);

  //print_QRUOV_params(&para);

  // keygen
  size_t pklen = para.pk_len;
  size_t sklen = para.sk_len;
  unsigned char* pk = NULL;
  unsigned char* sk = NULL;
  pk = (unsigned char*)malloc(sizeof(unsigned char) * para.pk_len);
  sk = (unsigned char*)malloc(sizeof(unsigned char) * para.sk_len);
  memset(pk, 0, para.pk_len);
  memset(sk, 0, para.sk_len);

  ret = QRUOV_keygen(pk, &pklen, sk, &sklen, &para);
  if (ret!=0) printf("QRUOV_keygen returned %d.\n", ret);

  //for (int i=0; i<para.pk_len; i++){
  //  printf("%02x", pk[i]);
  //}
  //printf("\n");
  //for (int i=0; i<para.sk_len; i++){
  //  printf("%02x", sk[i]);
  //}
  //printf("\n");

  // sign
  size_t siglen = para.sigma_len;
  unsigned char* sig = NULL;
  sig = (unsigned char*)malloc(sizeof(unsigned char) * para.sigma_len);
  memset(sig, 0, para.sigma_len);
  unsigned char* m = (unsigned char*)"hello world!";
  size_t mlen = 12;

  ret = QRUOV_sign(sk, para.sk_len, sig, &siglen, m, mlen, &para);
  if (ret!=0) printf("QRUOV_sign returned %d.\n", ret);

  //for (int i=0; i<para.sigma_len; i++){
  //  printf("%02x", sig[i]);
  //}
  //printf("\n");

  // verify
  ret = QRUOV_verify(pk, para.pk_len, sig, para.sigma_len, m, mlen, &para);
  if (ret!=0 && ret!=1) printf("QRUOV_verify returned %d.\n", ret);

  if (ret==0) printf("signature is rejected.\n");
  else if (ret==1) printf("signature is accepted.\n");

  if (pk!=NULL) free(pk);
  if (sk!=NULL) free(sk);
  if (sig!=NULL) free(sig);

  return 0;
}
