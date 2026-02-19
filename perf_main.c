#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/parameter.h"
#include "include/qruov.h"

#define LOOP 100

// compile
// libqruov.aをDEBUG=falseでビルドしておくこと
// $ gcc -o perf_org -Wall -O3 -std=c17 -fopenmp -I./include -I/usr/local/include perf_main.c -L./src -lqruov -L/usr/local/lib64 -lcrypto
// $ gcc -o perf_org -Wall -O3 -std=c17 -DQRUOV_USE_MULTI_THREAD -fopenmp -I./include -I/usr/local/include perf_main.c -L./src -lqruov -L/usr/local/lib64 -lcrypto

// micro sec
inline static double diff_time(struct timespec start, struct timespec stop) {
  double diff = (stop.tv_sec * 1000000 + (double)stop.tv_nsec / 1000) 
              - (start.tv_sec * 1000000 + (double)start.tv_nsec / 1000);
  return diff;
}

int main(int argc, char* argv[]){

  if (argc != 4){
    printf("USAGE: ./perf_org <num_threads> <param_name> <prg>\n");
    return -1;
  }

  // 0ならマルチスレッドなし、1以上ならマルチスレッドあり
  int num_threads = atoi(argv[1]);

  // "qruov1q127L3v156m54", "qruov3q127L3v228m78"のどちらかを想定
  char* param_name = argv[2];

  // "SHAKE", "AES"のどちらかを想定
  char* prg = argv[3];

  int ret = 0;
  struct timespec start_time, stop_time;
  double sum_k = 0, sum_s = 0, sum_v = 0;  // keygen, sign, verify

  // paramgen
  QRUOV_params para;

  ret = QRUOV_paramgen(param_name, prg, &para);

  if (ret!=0){
    printf("QRUOV_paramgen returned %d.\n", ret);
    return ret;
  }

  // init
  unsigned char* pk = NULL;
  unsigned char* sk = NULL;
  unsigned char* sig = NULL;

  pk = (unsigned char*)malloc(sizeof(unsigned char) * para.pk_len);
  sk = (unsigned char*)malloc(sizeof(unsigned char) * para.sk_len);
  sig = (unsigned char*)malloc(sizeof(unsigned char) * para.sigma_len);

  memset(pk, 0, para.pk_len);
  memset(sk, 0, para.sk_len);
  memset(sig, 0, para.sigma_len);

  size_t pklen = para.pk_len;
  size_t sklen = para.sk_len;
  size_t siglen = para.sigma_len;

  unsigned char* m = (unsigned char*)"helloworld";
  size_t mlen = 10;

  for(int i=0; i<LOOP; i++){

    ret = 0;

    // keygen
    clock_gettime(CLOCK_MONOTONIC, &start_time);    
    ret = QRUOV_keygen(pk, &pklen, sk, &sklen, &para);
    clock_gettime(CLOCK_MONOTONIC, &stop_time);
  
    if (ret!=0){
      printf("QRUOV_keygen returned %d.\n", ret);
      return ret;
    }

    printf("raw,%d,%d,%s,%s,QRUOV_keygen,%.06f,usec\n",
           i, num_threads, param_name, prg, diff_time(start_time, stop_time));
    fflush(stdout);
    sum_k += diff_time(start_time, stop_time);
  
    // sign
    clock_gettime(CLOCK_MONOTONIC, &start_time);    
    ret = QRUOV_sign(sk, para.sk_len, sig, &siglen, m, mlen, &para);
    clock_gettime(CLOCK_MONOTONIC, &stop_time);
  
    if (ret!=0){
      printf("QRUOV_sign returned %d.\n", ret);
      return ret;
    }

    printf("raw,%d,%d,%s,%s,QRUOV_sign,%.06f,usec\n",
           i, num_threads, param_name, prg, diff_time(start_time, stop_time));
    fflush(stdout);
    sum_s += diff_time(start_time, stop_time);
  
    // verify
    clock_gettime(CLOCK_MONOTONIC, &start_time);    
    ret = QRUOV_verify(pk, para.pk_len, sig, para.sigma_len, m, mlen, &para);
    clock_gettime(CLOCK_MONOTONIC, &stop_time);
  
    if (ret!=0 && ret!=1){
      printf("QRUOV_verify returned %d.\n", ret);
      return ret;
    }else if (ret==0){
      printf("signature is rejected.\n");
      return ret;
    }

    printf("raw,%d,%d,%s,%s,QRUOV_verify,%.06f,usec\n",
           i, num_threads, param_name, prg, diff_time(start_time, stop_time));
    fflush(stdout);
    sum_v += diff_time(start_time, stop_time);
  }

  // final
  if (pk!=NULL) free(pk);
  if (sk!=NULL) free(sk);
  if (sig!=NULL) free(sig);

  printf("avg,,%d,%s,%s,QRUOV_keygen,%.06f,usec\n", num_threads, param_name, prg, sum_k/LOOP);
  printf("avg,,%d,%s,%s,QRUOV_sign,%.06f,usec\n", num_threads, param_name, prg, sum_s/LOOP);
  printf("avg,,%d,%s,%s,QRUOV_verify,%.06f,usec\n", num_threads, param_name, prg, sum_v/LOOP);

  return 0;
}
