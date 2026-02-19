/**
 * @file  parameter.h
 * @brief QR-UOV署名ライブラリのパラメータの構造体を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

/**
 * QR-UOVの各パラメータを集めた構造体。
 */
typedef struct QRUOV_params_struct{
  int parameter;
  int SL;
  int q;
  int v;
  int m;
  int l;
  int tau_lVM;
  int tau_lVV12;
  int tau_lV;
  int tau_m;
  int lambda;
  int q_bitlen;
  const uint8_t* Fq_inv_table;
  int n;
  int V;
  int M;
  int N;
  int Fql_degree;
  int Fql_accumulator_degree;
  int seed_len;
  int salt_len;
  int f_c;
  int f_e;
  int f_c0;
  size_t sk_len;
  size_t pk_len;
  size_t sigma_len;
  int prg_shake;
  void* prg_ctx;
  int SHAKE_len;
  int AES_len;
} QRUOV_params;

#endif /* PARAMETER_H_INCLUDED */
