/**
 * @file  main.c
 * @brief main関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parameter.h"
#include "qruov.h"
#include <time.h>

#define BENCH_ITERS 3000
#define WORMUP_ITERS 10

static double timespec_diff_sec(const struct timespec *start, const struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec)
         + (double)(end->tv_nsec - start->tv_nsec) / 1e9;
}

int main(){
  struct timespec start, end;
  double keygen_time, sign_time, verify_time;

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

  // keygen
  size_t pklen = para.pk_len;
  size_t sklen = para.sk_len;
  unsigned char* pk = NULL;
  unsigned char* sk = NULL;
  pk = (unsigned char*)malloc(sizeof(unsigned char) * para.pk_len);
  sk = (unsigned char*)malloc(sizeof(unsigned char) * para.sk_len);
  memset(pk, 0, para.pk_len);
  memset(sk, 0, para.sk_len);

    // ウォームアップ
    for (int i = 0; i < WORMUP_ITERS; i++) {
        ret = QRUOV_keygen(pk, &pklen, sk, &sklen, &para);
    }

    // keygen 計測
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < BENCH_ITERS; i++) {
        ret = QRUOV_keygen(pk, &pklen, sk, &sklen, &para);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    keygen_time = timespec_diff_sec(&start, &end);
    printf("QRUOV_keygen %d回の合計時間: %.9f 秒\n", BENCH_ITERS, keygen_time);
    printf("QRUOV_keygen 1回あたり: %.9f 秒\n", keygen_time / BENCH_ITERS);


  // sign
  size_t siglen = para.sigma_len;
  unsigned char* sig = NULL;
  sig = (unsigned char*)malloc(sizeof(unsigned char) * para.sigma_len);
  memset(sig, 0, para.sigma_len);
  unsigned char* m = (unsigned char*)"hello world!";
  size_t mlen = 12;

    // ウォームアップ
    for (int i = 0; i < WORMUP_ITERS; i++) {
        ret = QRUOV_sign(sk, para.sk_len, sig, &siglen, m, mlen, &para);
    }

    //sign 計測
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < BENCH_ITERS; i++) {
        ret = QRUOV_sign(sk, para.sk_len, sig, &siglen, m, mlen, &para);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    sign_time = timespec_diff_sec(&start, &end);
    printf("QRUOV_sign %d回の合計時間: %.9f 秒\n", BENCH_ITERS, sign_time);
    printf("QRUOV_sign 1回あたり: %.9f 秒\n", sign_time / BENCH_ITERS);


    // ウォームアップ
    for (int i = 0; i < WORMUP_ITERS; i++) {
        ret = QRUOV_verify(pk, para.pk_len, sig, para.sigma_len, m, mlen, &para);
    }

  // verify 計測
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < BENCH_ITERS; i++) {
        ret = QRUOV_verify(pk, para.pk_len, sig, para.sigma_len, m, mlen, &para);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    verify_time = timespec_diff_sec(&start, &end);
    printf("QRUOV_verify %d回の合計時間: %.9f 秒\n", BENCH_ITERS, verify_time);
    printf("QRUOV_verify 1回あたり: %.9f 秒\n", verify_time / BENCH_ITERS);


  if (ret==0) printf("signature is rejected.\n");
  else if (ret==1) printf("signature is accepted.\n");

  if (pk!=NULL) free(pk);
  if (sk!=NULL) free(sk);
  if (sig!=NULL) free(sig);

  return 0;
}
