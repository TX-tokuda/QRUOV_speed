/**
 * @file  expand.c
 * @brief Expand関数とその関連関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include "expand.h"

void rej_samp(Fq* vec, const unsigned char* str, const int tau, const int nd,
              const QRUOV_params* para){

  Fq vectmp[tau];

  for(int j=0; j<tau; j++){
    vectmp[j] = str[j] & ((uint8_t)para->q);
  }

  int k = nd;
  while((vectmp[k]==para->q) && (k<tau)) k++;

  for(int j=0; j<nd; j++){
    if(vectmp[j]==para->q){
      if(k<tau){
        vectmp[j] = vectmp[k];
        k++;
        while((vectmp[k]==para->q) && (k<tau)) k++;
      }else{
        vectmp[j] = 0;
      }
    }
  }

  for(int j=0; j<nd; j++){
    vec[j] = vectmp[j];
  }

  // clear
  memset(vectmp, 0, tau);

  return;
}

int rej_samp_PRG_SHAKE(SHAKE_CTX* ctx, Fq* vec, const int i, const int tau, const int nd,
                       const QRUOV_params* para){

  unsigned char str[tau];

  int ret = PRG_SHAKE(ctx, str, i, 8*tau);
  if (ret!=0) return ret;

  rej_samp(vec, str, tau, nd, para);

  // clear
  memset(str, 0, tau);

  return 0;
}

int rej_samp_PRG_AES(AES_CTX* ctx, Fq* vec, const int i, const int tau, const int nd,
                     const QRUOV_params* para){

  unsigned char str[tau];

  int ret = PRG_AES(ctx, str, i, 8*tau);
  if (ret!=0) return ret;

  rej_samp(vec, str, tau, nd, para);

  // clear
  memset(str, 0, tau);

  return 0;
}

int expand_matrix(FQL_MATRIX* R, const Fq* vec, const int veclen, const QRUOV_params* para){

  if (((R->size)*para->l)!=veclen){
#ifdef DEBUG
    fprintf(stderr, "[expand_matrix] (R->size)*para->l must be == veclen.\n");
#endif
    return -1;
  }

  for(int i=0; i<(R->size); i++){
    for(int j=0; j<para->l; j++){
      R->data[i][j] = vec[i*para->l+j];
    }
  }

  return 0;
}

int expand_matrix_op(FQL_MATRIX_OP* R, const Fq* vec, const int veclen, const QRUOV_params* para){

  if (((R->size)*para->l)!=veclen){
#ifdef DEBUG
    fprintf(stderr, "[expand_matrix_op] (R->size)*para->l must be == veclen.\n");
#endif
    return -1;
  }

  // a = a[2]*x^2 + a[1]*x + a[0]
  //   64bit   25bit  7bit  9bit  7bit  9bit  7bit
  // data[i] =     0  a[2]     0  a[1]     0  a[0] 
  for(int i=0; i<(R->size); i++){
    R->data[i] = (uint64_t)vec[i*para->l+0];         // a[0]
    R->data[i] |= ((uint64_t)vec[i*para->l+1])<<16;  // a[1]
    R->data[i] |= ((uint64_t)vec[i*para->l+2])<<32;  // a[2]
  }

  return 0;
}

int expand_sk(FQL_MATRIX* SdT, QRUOV_params* para){

  int ret;

  int len = para->l * para->V * para->M;
  int tau = para->tau_lVM;
  Fq vec[len] ;

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE(para->prg_ctx, vec, 0, tau, len, para);
  }else{
    ret = rej_samp_PRG_AES(para->prg_ctx, vec, 0, tau, len, para);
  }
  if (ret!=0) return ret;

  ret = expand_matrix(SdT, vec, len, para);
  if (ret!=0) return ret-20;

  // clear
  memset(vec, 0, len);

  return 0;
}

int expand_sk_op(FQL_MATRIX_OP* SdT, QRUOV_params* para){

  int ret;

  int len = para->l * para->V * para->M;
  int tau = para->tau_lVM;
  Fq vec[len] ;

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE(para->prg_ctx, vec, 0, tau, len, para);
  }else{
    ret = rej_samp_PRG_AES(para->prg_ctx, vec, 0, tau, len, para);
  }
  if (ret!=0) return ret;

  ret = expand_matrix_op(SdT, vec, len, para);
  if (ret!=0) return ret-20;

  // clear
  memset(vec, 0, len);

  return 0;
}

int expand_pk(FQL_MATRIX* Pi1, FQL_MATRIX* Pi2T, const int i, QRUOV_params* para){

  int ret;
  void* Pi1_ctx = NULL;

  if(para->prg_shake==1){
    Pi1_ctx = (void*)malloc(sizeof(SHAKE_CTX));
  }else{
    Pi1_ctx = (void*)malloc(sizeof(AES_CTX));
  }
  if (Pi1_ctx == NULL){
#ifdef DEBUG
    fprintf(stderr, "[expand_pk] Pi1_ctx malloc failed.\n");
#endif
    return -1;
  }

  int Pi1_len = (para->l * para->V * (para->V+1)) / 2;
  int Pi1_tau = para->tau_lVV12;
  Fq Pi1_vec[Pi1_len];

  if(para->prg_shake==1){
    ret = SHAKE_CTX_copy((SHAKE_CTX*)Pi1_ctx, para->prg_ctx);
  }else{
    ret = AES_CTX_copy((AES_CTX*)Pi1_ctx, para->prg_ctx);
  }
  if (ret!=0){
    ret-=10;
    goto end1;
  }

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE((SHAKE_CTX*)Pi1_ctx, Pi1_vec, 2*i, Pi1_tau, Pi1_len, para);
  }else{
    ret = rej_samp_PRG_AES((AES_CTX*)Pi1_ctx, Pi1_vec, 2*i, Pi1_tau, Pi1_len, para);
  }
  if (ret!=0){
    ret-=20;
    goto end1;
  }

  ret = expand_matrix(Pi1, Pi1_vec, Pi1_len, para);
  if (ret!=0){
    ret-=40;
    goto end1;
  }

  end1:
  if(para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)&Pi1_ctx);
  }else{
    AES_free((AES_CTX**)&Pi1_ctx);
  }
  if (Pi1_ctx!=NULL) free(Pi1_ctx);

  if (ret!=0) return ret;

  void* Pi2_ctx = NULL;

  if(para->prg_shake==1){
    Pi2_ctx = (void*)malloc(sizeof(SHAKE_CTX));
  }else{
    Pi2_ctx = (void*)malloc(sizeof(AES_CTX));
  }
  if (Pi2_ctx == NULL){
#ifdef DEBUG
    fprintf(stderr, "[expand_pk] Pi2_ctx malloc failed.\n");
#endif
    return -2;
  }

  int Pi2_len = para->l * para->V * para->M;
  int Pi2_tau = para->tau_lVM;
  Fq Pi2_vec[Pi2_len];

  if(para->prg_shake==1){
    ret = SHAKE_CTX_copy((SHAKE_CTX*)Pi2_ctx, para->prg_ctx);
  }else{
    ret = AES_CTX_copy((AES_CTX*)Pi2_ctx, para->prg_ctx);
  }
  if (ret!=0){
    ret-=50;
    goto end2;
  }

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE((SHAKE_CTX*)Pi2_ctx, Pi2_vec, 2*i+1, Pi2_tau, Pi2_len, para);
  }else{
    ret = rej_samp_PRG_AES((AES_CTX*)Pi2_ctx, Pi2_vec, 2*i+1, Pi2_tau, Pi2_len, para);
  }
  if (ret!=0){
    ret-=60;
    goto end2;
  }

  ret = expand_matrix(Pi2T, Pi2_vec, Pi2_len, para);
  if (ret!=0){
    ret-=80;
    goto end2;
  }

  end2:
  if(para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)&Pi2_ctx);
  }else{
    AES_free((AES_CTX**)&Pi2_ctx);
  }
  if (Pi2_ctx!=NULL) free(Pi2_ctx);

  return ret;
}

int expand_pk_op(FQL_MATRIX_OP* Pi1, FQL_MATRIX_OP* Pi2T, const int i, QRUOV_params* para){

  int ret;
  void* Pi1_ctx = NULL;

  if(para->prg_shake==1){
    Pi1_ctx = (void*)malloc(sizeof(SHAKE_CTX));
  }else{
    Pi1_ctx = (void*)malloc(sizeof(AES_CTX));
  }
  if (Pi1_ctx == NULL){
#ifdef DEBUG
    fprintf(stderr, "[expand_pk] Pi1_ctx malloc failed.\n");
#endif
    return -1;
  }

  int Pi1_len = (para->l * para->V * (para->V+1)) / 2;
  int Pi1_tau = para->tau_lVV12;
  Fq Pi1_vec[Pi1_len];

  if(para->prg_shake==1){
    ret = SHAKE_CTX_copy((SHAKE_CTX*)Pi1_ctx, para->prg_ctx);
  }else{
    ret = AES_CTX_copy((AES_CTX*)Pi1_ctx, para->prg_ctx);
  }
  if (ret!=0){
    ret-=10;
    goto end1;
  }

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE((SHAKE_CTX*)Pi1_ctx, Pi1_vec, 2*i, Pi1_tau, Pi1_len, para);
  }else{
    ret = rej_samp_PRG_AES((AES_CTX*)Pi1_ctx, Pi1_vec, 2*i, Pi1_tau, Pi1_len, para);
  }
  if (ret!=0){
    ret-=20;
    goto end1;
  }

  ret = expand_matrix_op(Pi1, Pi1_vec, Pi1_len, para);
  if (ret!=0){
    ret-=40;
    goto end1;
  }

  end1:
  if(para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)&Pi1_ctx);
  }else{
    AES_free((AES_CTX**)&Pi1_ctx);
  }
  if (Pi1_ctx!=NULL) free(Pi1_ctx);

  if (ret!=0) return ret;

  void* Pi2_ctx = NULL;

  if(para->prg_shake==1){
    Pi2_ctx = (void*)malloc(sizeof(SHAKE_CTX));
  }else{
    Pi2_ctx = (void*)malloc(sizeof(AES_CTX));
  }
  if (Pi2_ctx == NULL){
#ifdef DEBUG
    fprintf(stderr, "[expand_pk] Pi2_ctx malloc failed.\n");
#endif
    return -2;
  }

  int Pi2_len = para->l * para->V * para->M;
  int Pi2_tau = para->tau_lVM;
  Fq Pi2_vec[Pi2_len];

  if(para->prg_shake==1){
    ret = SHAKE_CTX_copy((SHAKE_CTX*)Pi2_ctx, para->prg_ctx);
  }else{
    ret = AES_CTX_copy((AES_CTX*)Pi2_ctx, para->prg_ctx);
  }
  if (ret!=0){
    ret-=50;
    goto end2;
  }

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE((SHAKE_CTX*)Pi2_ctx, Pi2_vec, 2*i+1, Pi2_tau, Pi2_len, para);
  }else{
    ret = rej_samp_PRG_AES((AES_CTX*)Pi2_ctx, Pi2_vec, 2*i+1, Pi2_tau, Pi2_len, para);
  }
  if (ret!=0){
    ret-=60;
    goto end2;
  }

  ret = expand_matrix_op(Pi2T, Pi2_vec, Pi2_len, para);
  if (ret!=0){
    ret-=80;
    goto end2;
  }

  end2:
  if(para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)&Pi2_ctx);
  }else{
    AES_free((AES_CTX**)&Pi2_ctx);
  }
  if (Pi2_ctx!=NULL) free(Pi2_ctx);

  return ret;
}

int expand_y(FQL_MATRIX* y, const QRUOV_params* para){

  int ret;

  int len = para->l * para->V;
  int tau = para->tau_lV;
  Fq vec[len] ;

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE(para->prg_ctx, vec, 0, tau, len, para);
  }else{
    ret = rej_samp_PRG_AES(para->prg_ctx, vec, 0, tau, len, para);
  }
  if (ret!=0) return ret;

  ret = expand_matrix(y, vec, len, para);
  if (ret!=0) return ret-20;

  // clear
  memset(vec, 0, len);

  return 0;
}

int expand_y_op(FQL_MATRIX_OP* y, const QRUOV_params* para){

  int ret;

  int len = para->l * para->V;
  int tau = para->tau_lV;
  Fq vec[len] ;

  if(para->prg_shake==1){
    ret = rej_samp_PRG_SHAKE(para->prg_ctx, vec, 0, tau, len, para);
  }else{
    ret = rej_samp_PRG_AES(para->prg_ctx, vec, 0, tau, len, para);
  }
  if (ret!=0) return ret;

  ret = expand_matrix_op(y, vec, len, para);
  if (ret!=0) return ret-20;

  // clear
  memset(vec, 0, len);

  return 0;
}

int Hash(FQ_MATRIX* t, const unsigned char* mu, const unsigned char* r, const QRUOV_params* para){

  int ret;

  SHAKE_CTX* ctx = NULL;
  int tau = para->tau_m;
  unsigned char str[tau];
  Fq vec[para->m];

  ret = SHAKE_init(&ctx, 256);  // SHAKE256
  if (ret!=0){
    return ret;
  }

  ret = SHAKE_update(ctx, mu, 64);
  if (ret!=0){
    SHAKE_free(&ctx);
    return ret-10;
  }

  ret = SHAKE_update(ctx, r, para->salt_len);
  if (ret!=0){
    SHAKE_free(&ctx);
    return ret-20;
  }

  ret = SHAKE_gen(ctx, str, tau);
  if (ret!=0){
    SHAKE_free(&ctx);
    return ret-30;
  }

  SHAKE_free(&ctx);

  rej_samp(vec, str, tau, para->m, para);

  for(int i=0; i<(t->size); i++){
    t->data[i] = vec[i];
  }

  return 0;
}
