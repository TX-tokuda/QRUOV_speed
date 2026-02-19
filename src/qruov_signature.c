/**
 * @file  qruov_signature.c
 * @brief QR-UOVの鍵生成、署名生成、署名検証を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include "qruov_signature.h"

int string_init(unsigned char** string, const int len){

  if (*string!=NULL){
    free(*string);
    *string = NULL;
  }

  *string = (unsigned char*)malloc(sizeof(unsigned char) * len);

  if (*string == NULL){
#ifdef DEBUG
    fprintf(stderr, "[string_init] malloc failed.\n");
#endif
    return -1;
  }

  return 0;
}

void string_free(unsigned char** string){

  if (string!=NULL){
    if (*string!=NULL){
      free(*string);
      *string = NULL;
    }
  }

  return;
}

int compute_SdT(FQL_MATRIX* SdT, const unsigned char* sk_seed, QRUOV_params* para){

  int ret;
  void** ctx = &(para->prg_ctx);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, sk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, sk_seed, para->AES_len*8);
  }
  if (ret!=0) { ret-=10; goto end; }

  ret = expand_sk(SdT, para);
  if (ret!=0){ ret-=20; goto end; }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

int compute_SdT_op(FQL_MATRIX_OP* SdT, const unsigned char* sk_seed, QRUOV_params* para){

  int ret;
  void** ctx = &(para->prg_ctx);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, sk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, sk_seed, para->AES_len*8);
  }
  if (ret!=0) { ret-=10; goto end; }

  ret = expand_sk_op(SdT, para);
  if (ret!=0){ ret-=20; goto end; }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

int compute_P1_P2T_P3(FQL_MATRIX* P1, FQL_MATRIX* P2T, FQL_MATRIX* P3,
                      FQL_MATRIX* SdT, const unsigned char * pk_seed, QRUOV_params* para){

  int ret_code = 0, ret = 0;
  void** ctx = &(para->prg_ctx);

  FQL_MATRIX Sd = {.data = NULL};
  ret = Fql_matrix_init(&Sd, para->V, para->M, 0, 1, para);
  if (ret!=0){ ret_code = ret-0; goto end; }
  ret = Fql_matrix_copy(&Sd, SdT, para);
  if (ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_transpose(&Sd);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0){ ret_code = ret-20; goto end; }

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, pk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, pk_seed, para->AES_len*8);
  }
  if (ret!=0){ ret_code = ret-30; goto end; }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(ret_code, P1, P2T, P3, SdT, Sd, para)
#endif
  for(i=0;i<para->m;i++){

    int ret_for;

    ret_for = expand_pk(&(P1[i]), &(P2T[i]), i, para);
    if (ret_for!=0){
      ret_code = ret_for-40;
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
      goto end_for;
    }

    ret_for = compute_Pi3(&(P3[i]), &(P1[i]), &(P2T[i]), SdT, &Sd, para);
    if (ret_for!=0){
      ret_code = ret_for-130;
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
      goto end_for;
    }

    end_for:
    i=i;  // dummy
  }

end:
  Fql_matrix_clear(&Sd, para);
  Fql_matrix_free(&Sd);
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret_code;
}

int compute_P1_P2T_P3_op(FQL_MATRIX_OP* P1, FQL_MATRIX_OP* P2T, FQL_MATRIX_OP* P3,
                         FQL_MATRIX_OP* SdT, const unsigned char * pk_seed, QRUOV_params* para){

  int ret_code = 0, ret = 0;
  void** ctx = &(para->prg_ctx);

  FQL_MATRIX_OP Sd = {.data = NULL};
  ret = Fql_matrix_init_op(&Sd, para->V, para->M, 0, 1, para);
  if (ret!=0){ ret_code = ret-0; goto end; }
  ret = Fql_matrix_copy_op(&Sd, SdT);
  if (ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_transpose_op(&Sd);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0){ ret_code = ret-20; goto end; }

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, pk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, pk_seed, para->AES_len*8);
  }
  if (ret!=0){ ret_code = ret-30; goto end; }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(ret_code, P1, P2T, P3, SdT, Sd, para)
#endif
  for(i=0;i<para->m;i++){

    int ret_for;

    ret_for = expand_pk_op(&(P1[i]), &(P2T[i]), i, para);
    if (ret_for!=0){
      ret_code = ret_for-40;
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
      goto end_for;
    }

    ret_for = compute_Pi3_op(&(P3[i]), &(P1[i]), &(P2T[i]), SdT, &Sd, para);
    if (ret_for!=0){
      ret_code = ret_for-130;
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
      goto end_for;
    }

    end_for:
    i=i;  // dummy
  }

end:
  Fql_matrix_clear_op(&Sd, para);
  Fql_matrix_free_op(&Sd);
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret_code;
}

int compute_y(FQL_MATRIX* y, const unsigned char* y_seed, QRUOV_params* para){

  int ret;
  void** ctx = &(para->prg_ctx);

  if(para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if(para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, y_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, y_seed, para->AES_len*8);
  }
  if (ret!=0) { ret-=10; goto end; }

  ret = expand_y(y, para);
  if (ret!=0){ ret-=20; goto end; }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

int compute_y_op(FQL_MATRIX_OP* y, const unsigned char* y_seed, QRUOV_params* para){

  int ret;
  void** ctx = &(para->prg_ctx);

  if(para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if(para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, y_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, y_seed, para->AES_len*8);
  }
  if (ret!=0) { ret-=10; goto end; }

  ret = expand_y_op(y, para);
  if (ret!=0){ ret-=20; goto end; }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

int compute_P1_P2T_L_u(FQL_MATRIX* P1, FQL_MATRIX* P2T, FQ_MATRIX* L, FQ_MATRIX* u,
                       const FQL_MATRIX* SdT, const FQL_MATRIX* y, const unsigned char* pk_seed,
                       QRUOV_params* para){

  int ret = 0;
  void** ctx = &(para->prg_ctx);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, pk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, pk_seed, para->AES_len*8);
  }
  if (ret!=0){ ret-=10; goto end; }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(ret, P1, P2T, SdT, y, para)
#endif
  for(i=0;i<para->m;i++){

    int ret_for;
    FQ_MATRIX Li = {.data = NULL};
    FQ_MATRIX ui = {.data = NULL};

    ret_for = Fq_matrix_init(&Li, para->m, 1);
    if (ret_for!=0){ ret = ret_for-20; goto end_for; }
    Fq_matrix_clear(&Li);

    ret_for = Fq_matrix_init(&ui, 1, 1);
    if (ret_for!=0){ ret = ret_for-30; goto end_for; }
    Fq_matrix_clear(&ui);

    ret_for = expand_pk(&(P1[i]), &(P2T[i]), i ,para);
    if (ret_for!=0){ ret = ret_for-40; goto end_for; }

    ret_for = compute_Li_ui(&Li, &ui, &(P1[i]), &(P2T[i]), SdT, y, para);
    if (ret_for!=0){ ret = ret_for-130; goto end_for; }

    for(int j=0; j<para->m; j++){
      L->data[INDEX(i, j, L->col)] = Li.data[INDEX(j, 0, Li.col)];
    }
    u->data[INDEX(i, 0, u->col)] = ui.data[INDEX(0, 0, ui.col)];

    end_for:
    Fq_matrix_clear(&ui);
    Fq_matrix_free(&ui);
    Fq_matrix_clear(&Li);
    Fq_matrix_free(&Li);
    if(ret!=0){
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
    }
  }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

int compute_P1_P2T_L_u_op(FQL_MATRIX_OP* P1, FQL_MATRIX_OP* P2T, FQ_MATRIX* L, FQ_MATRIX* u,
                          const FQL_MATRIX_OP* SdT, const FQL_MATRIX_OP* y,
			  const unsigned char* pk_seed, QRUOV_params* para){

  int ret = 0;
  void** ctx = &(para->prg_ctx);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, pk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, pk_seed, para->AES_len*8);
  }
  if (ret!=0){ ret-=10; goto end; }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(ret, P1, P2T, SdT, y, para)
#endif
  for(i=0;i<para->m;i++){

    int ret_for;
    FQ_MATRIX Li = {.data = NULL};
    FQ_MATRIX ui = {.data = NULL};

    ret_for = Fq_matrix_init(&Li, para->m, 1);
    if (ret_for!=0){ ret = ret_for-20; goto end_for; }
    Fq_matrix_clear(&Li);

    ret_for = Fq_matrix_init(&ui, 1, 1);
    if (ret_for!=0){ ret = ret_for-30; goto end_for; }
    Fq_matrix_clear(&ui);

    ret_for = expand_pk_op(&(P1[i]), &(P2T[i]), i ,para);
    if (ret_for!=0){ ret = ret_for-40; goto end_for; }

    ret_for = compute_Li_ui_op(&Li, &ui, &(P1[i]), &(P2T[i]), SdT, y, para);
    if (ret_for!=0){ ret = ret_for-130; goto end_for; }

    for(int j=0; j<para->m; j++){
      L->data[INDEX(i, j, L->col)] = Li.data[INDEX(j, 0, Li.col)];
    }
    u->data[INDEX(i, 0, u->col)] = ui.data[INDEX(0, 0, ui.col)];

    end_for:
    Fq_matrix_clear(&ui);
    Fq_matrix_free(&ui);
    Fq_matrix_clear(&Li);
    Fq_matrix_free(&Li);
    if(ret!=0){
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
    }
  }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}


int compute_mu(unsigned char* mu, const unsigned char* M, const unsigned long long Mlen,
               const unsigned char* pk_seed, const QRUOV_params* para){

  int ret;
  SHAKE_CTX* ctx = NULL;

  ret = SHAKE_init(&ctx, 256);
  if (ret!=0) return ret;

  ret = SHAKE_update(ctx, pk_seed, para->seed_len);
  if (ret!=0){ SHAKE_free(&ctx); return ret-10; }

  ret = SHAKE_update(ctx, M, Mlen);
  if (ret!=0){ SHAKE_free(&ctx); return ret-20; }

  ret = SHAKE_gen(ctx, mu, 64);
  if (ret!=0){ SHAKE_free(&ctx); return ret-30; }

  SHAKE_free(&ctx);

  return 0;
}

int r_ctx_init(void** r_ctx, const unsigned char* r_seed, const QRUOV_params* para){

  int ret;

  if(para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)r_ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)r_ctx);
  }
  if (ret!=0) return ret;

  if(para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*r_ctx, r_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*r_ctx, r_seed, para->AES_len*8);
  }
  if (ret!=0) return ret-10;

  return 0;
}

int compute_r(void* r_ctx, unsigned char* r, const QRUOV_params* para){

  int ret = 0;

  if(para->prg_shake==1){
    SHAKE_CTX* tmp_ctx = NULL;
    tmp_ctx = (SHAKE_CTX*)malloc(sizeof(SHAKE_CTX));
    if (tmp_ctx == NULL){
#ifdef DEBUG
      fprintf(stderr, "[compute_r] malloc failed.\n");
#endif
      return -1;
    }

    unsigned char x[2];
    memset(x, 0, 2);

    ret = SHAKE_update(r_ctx, x, 2);
    if (ret!=0) { ret-=10; goto end_shake; }

    ret = SHAKE_CTX_copy(tmp_ctx, (SHAKE_CTX*)r_ctx);
    if (ret!=0) { ret-=20; goto end_shake; }

    ret = SHAKE_gen(tmp_ctx, r, para->salt_len);
    if (ret!=0) { ret-=30; goto end_shake; }

    end_shake:
    SHAKE_free(&tmp_ctx);

  }else{
    unsigned char intext[16];  // AES-CTR plaintext(16Byte)
    memset(intext, 0, 16);
    uint64_t loopnum =  para->salt_len / 16;
    if (para->salt_len % 16 != 0) loopnum += 1;

    for(uint64_t j=0; j<loopnum; j++){
      unsigned char outbuf[16];
      memset(outbuf, 0, 16);
      ret = AES_gen((AES_CTX*)r_ctx, outbuf, intext);
      if (ret!=0) { goto end_aes; }

      int cpylen = ((j+1)*16 <= para->salt_len) ? 16 : (para->salt_len - j*16);
      memcpy(r+j*16, outbuf, cpylen);
    }

    end_aes:
    ret=ret;  // dummy
  }

  return ret;
}

int compute_P1_P2T(FQL_MATRIX* P1, FQL_MATRIX* P2T, const unsigned char * pk_seed,
                   QRUOV_params* para){

  int ret = 0;
  void** ctx = &(para->prg_ctx);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, pk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, pk_seed, para->AES_len*8);
  }
  if (ret!=0){ ret-=10; goto end; }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(ret, P1, P2T, para)
#endif
  for(i=0;i<para->m;i++){
    int ret_for = expand_pk(&(P1[i]), &(P2T[i]), i, para);
    if (ret_for!=0){
      ret = ret_for-20;
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
    }
  }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

int compute_P1_P2T_op(FQL_MATRIX_OP* P1, FQL_MATRIX_OP* P2T, const unsigned char * pk_seed,
                      QRUOV_params* para){

  int ret = 0;
  void** ctx = &(para->prg_ctx);

  if (para->prg_shake==1){
    ret = SHAKE_init((SHAKE_CTX**)ctx, para->SHAKE_len*8);
  }else{
    ret = AES_init((AES_CTX**)ctx);
  }
  if (ret!=0) goto end;

  if (para->prg_shake==1){
    ret = SHAKE_update((SHAKE_CTX*)*ctx, pk_seed, para->seed_len);
  }else{
    ret = AES_update((AES_CTX*)*ctx, pk_seed, para->AES_len*8);
  }
  if (ret!=0){ ret-=10; goto end; }

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(ret, P1, P2T, para)
#endif
  for(i=0;i<para->m;i++){
    int ret_for = expand_pk_op(&(P1[i]), &(P2T[i]), i, para);
    if (ret_for!=0){
      ret = ret_for-20;
#ifndef QRUOV_USE_MULTI_THREAD
      goto end;
#endif
    }
  }

  end:
  if (para->prg_shake==1){
    SHAKE_free((SHAKE_CTX**)ctx);
  }else{
    AES_free((AES_CTX**)ctx);
  }

  return ret;
}

#define FREE_KEYGEN()               \
  do{                               \
    for (int i=0; i<para->m; i++){  \
      Fql_matrix_free(&(P2T[i]));   \
      Fql_matrix_free(&(P1[i]));    \
    }                               \
    Fql_matrix_clear(&SdT, para);   \
    Fql_matrix_free(&SdT);          \
  }while (0)                        \

int keygen(FQL_MATRIX* P3, const unsigned char* sk_seed, const unsigned char* pk_seed,
           QRUOV_params* para){

  int ret = 0;

  // init
  FQL_MATRIX SdT = {.data = NULL};
  FQL_MATRIX P1[para->m];
  for (int i=0; i<para->m; i++){
    P1[i].data = NULL;
  }
  FQL_MATRIX P2T[para->m];
  for (int i=0; i<para->m; i++){
    P2T[i].data = NULL;
  }

  ret = Fql_matrix_init(&SdT, para->V, para->M, 0, 1, para);
  if (ret!=0){ FREE_KEYGEN(); return ret-0; }

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P1[i]), para->V, para->V, 1, 0, para);
    if (ret!=0){ FREE_KEYGEN(); return ret-10; }

    ret = Fql_matrix_init(&(P2T[i]), para->V, para->M, 0, 1, para);
    if (ret!=0){ FREE_KEYGEN(); return ret-20; }
  }

  // compute SdT, P1, P2T, P3
  ret = compute_SdT(&SdT, sk_seed, para);
  if (ret!=0){ FREE_KEYGEN(); return ret-30; }

  ret = compute_P1_P2T_P3(P1, P2T, P3, &SdT, pk_seed, para);
  if (ret!=0){ FREE_KEYGEN(); return ret-80; }

  // free
  FREE_KEYGEN();

  return 0;
}

  
#define FREE_KEYGEN_OP()            \
  do{                               \
    for (int i=0; i<para->m; i++){  \
      Fql_matrix_free_op(&(P2T[i]));\
      Fql_matrix_free_op(&(P1[i])); \
    }                               \
    Fql_matrix_clear_op(&SdT, para);\
    Fql_matrix_free_op(&SdT);       \
  }while (0)                        \

int keygen_op(FQL_MATRIX_OP* P3, const unsigned char* sk_seed, const unsigned char* pk_seed,
              QRUOV_params* para){
  int ret = 0;

  // init
  FQL_MATRIX_OP SdT = {.data = NULL};
  FQL_MATRIX_OP P1[para->m];
  for (int i=0; i<para->m; i++){
    P1[i].data = NULL;
  }
  FQL_MATRIX_OP P2T[para->m];
  for (int i=0; i<para->m; i++){
    P2T[i].data = NULL;
  }

  ret = Fql_matrix_init_op(&SdT, para->V, para->M, 0, 1, para);
  if (ret!=0){ FREE_KEYGEN_OP(); return ret-0; }

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init_op(&(P1[i]), para->V, para->V, 1, 0, para);
    if (ret!=0){ FREE_KEYGEN_OP(); return ret-10; }

    ret = Fql_matrix_init_op(&(P2T[i]), para->V, para->M, 0, 1, para);
    if (ret!=0){ FREE_KEYGEN_OP(); return ret-20; }
  }

  // compute SdT, P1, P2T, P3
  ret = compute_SdT_op(&SdT, sk_seed, para);
  if (ret!=0){ FREE_KEYGEN_OP(); return ret-30; }

  ret = compute_P1_P2T_P3_op(P1, P2T, P3, &SdT, pk_seed, para);
  if (ret!=0){ FREE_KEYGEN_OP(); return ret-80; }

  // free
  FREE_KEYGEN_OP();

  return 0;
}

#define FREE_SIGN()                                                 \
  do{                                                               \
    if(P!=NULL) memset(P, 0, para->m);                              \
    if(first_col_index!=NULL) memset(first_col_index, 0, para->m);  \
    Fql_matrix_clear(&y, para);                                     \
    Fq_matrix_clear(&u);                                            \
    Fq_matrix_clear(&L);                                            \
    Fql_matrix_clear(&SdT, para);                                   \
    Fq_matrix_clear(&b);                                            \
    Fq_matrix_clear(&L_inv);                                        \
    Fq_matrix_clear(&LU);                                           \
    Fq_matrix_clear(&LL);                                           \
                                                                    \
    if (para->prg_shake==1){                                        \
      SHAKE_free((SHAKE_CTX**)&r_ctx);                              \
    }else{                                                          \
      AES_free((AES_CTX**)&r_ctx);                                  \
    }                                                               \
    if(P!=NULL) free(P);                                            \
    if(first_col_index!=NULL) free(first_col_index);                \
    Fql_matrix_free(&y);                                            \
    Fq_matrix_free(&u);                                             \
    Fq_matrix_free(&L);                                             \
    for (int i=0; i<para->m; i++){                                  \
      Fql_matrix_free(&(P2T[i]));                                   \
      Fql_matrix_free(&(P1[i]));                                    \
    }                                                               \
    Fql_matrix_free(&SdT);                                          \
    Fql_matrix_free(&x_Fql);                                        \
    Fq_matrix_free(&x_Fq);                                          \
    Fq_matrix_free(&b);                                             \
    Fq_matrix_free(&L_inv);                                         \
    Fq_matrix_free(&LU);                                            \
    Fq_matrix_free(&LL);                                            \
    Fq_matrix_free(&t);                                             \
  }while (0)                                                        \

int sign(unsigned char* r, FQL_MATRIX* s, const unsigned char* M, const unsigned long long Mlen,
         const unsigned char* sk_seed, const unsigned char* pk_seed, const unsigned char* r_seed,
         const unsigned char* y_seed, const unsigned char* x_seed, QRUOV_params* para){

  int ret = 0;

  void* r_ctx = NULL;

  FQL_MATRIX SdT = {.data = NULL};
  FQL_MATRIX P1[para->m];
  for (int i=0; i<para->m; i++){
    P1[i].data = NULL;
  }
  FQL_MATRIX P2T[para->m];
  for (int i=0; i<para->m; i++){
    P2T[i].data = NULL;
  }
  FQ_MATRIX L = {.data = NULL};
  FQ_MATRIX u = {.data = NULL};
  FQL_MATRIX y = {.data = NULL};

  unsigned char mu[64] = {0};
  FQ_MATRIX t = {.data = NULL};
  int* P = NULL;
  int rank = 0;
  int* first_col_index = NULL;

  FQ_MATRIX LL = {.data = NULL};
  FQ_MATRIX LU = {.data = NULL};
  FQ_MATRIX L_inv = {.data = NULL};
  int has_L_inv = 0;
  FQ_MATRIX b = {.data = NULL};
  FQ_MATRIX x_Fq = {.data = NULL};
  FQL_MATRIX x_Fql = {.data = NULL};

  P = (int*)malloc(sizeof(int) * para->m);
  if (P == NULL){
#ifdef DEBUG
    fprintf(stderr, "[sign] P malloc failed.\n");
#endif
    FREE_SIGN(); return -1;
  }
  memset(P, 0, para->m);

  first_col_index = (int*)malloc(sizeof(int) * para->m);
  if (first_col_index == NULL){
#ifdef DEBUG
    fprintf(stderr, "[sign] first_col_index malloc failed.\n");
#endif
    FREE_SIGN(); return -2;
  }
  memset(first_col_index, 0, para->m);

  // init
  ret = Fql_matrix_init(&SdT, para->V, para->M, 0, 1, para);
  if (ret!=0) return ret-10;
  Fql_matrix_clear(&SdT, para);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P1[i]), para->V, para->V, 1, 0, para);
    if (ret!=0){ FREE_SIGN(); return ret-20; }

    ret = Fql_matrix_init(&(P2T[i]), para->V, para->M, 0, 1, para);
    if (ret!=0){ FREE_SIGN(); return ret-30; }

    Fql_matrix_clear(&(P1[i]), para);
    Fql_matrix_clear(&(P2T[i]), para);
  }

  ret = Fq_matrix_init(&L, para->m, para->m);
  if (ret!=0){ FREE_SIGN(); return ret-40; }
  Fq_matrix_clear(&L);

  ret = Fq_matrix_init(&u, para->m, 1);
  if (ret!=0){ FREE_SIGN(); return ret-50; }
  Fq_matrix_clear(&u);

  ret = Fql_matrix_init(&y, para->V, 1, 0, 0, para);
  if (ret!=0){ FREE_SIGN(); return ret-60; }
  Fql_matrix_clear(&y, para);

  // compute SdT, y
  ret = compute_SdT(&SdT, sk_seed, para);
  if (ret!=0){ FREE_SIGN();  return ret-70; }

  ret = compute_y(&y, y_seed, para);
  if (ret!=0){ FREE_SIGN();  return ret-120; }

  // compute P1, P2T, L, u
  ret = compute_P1_P2T_L_u(P1, P2T, &L, &u, &SdT, &y, pk_seed, para);
  if (ret!=0){ FREE_SIGN(); return ret-170; }

  // init
  ret = Fq_matrix_init(&t, para->m, 1);
  if (ret!=0){ FREE_SIGN(); return ret-570; }
  Fq_matrix_clear(&t);

  ret = Fq_matrix_init(&LL, para->m, para->m);
  if (ret!=0){ FREE_SIGN(); return ret-580; }
  Fq_matrix_clear(&LL);

  ret = Fq_matrix_init(&LU, para->m, para->m);
  if (ret!=0){ FREE_SIGN(); return ret-590; }
  Fq_matrix_clear(&LU);

  ret = Fq_matrix_init(&L_inv, para->m, para->m);
  if (ret!=0){ FREE_SIGN(); return ret-600; }
  Fq_matrix_clear(&L_inv);

  ret = Fq_matrix_init(&b, para->m, 1);
  if (ret!=0){ FREE_SIGN(); return ret-610; }
  Fq_matrix_clear(&b);

  ret = Fq_matrix_init(&x_Fq, para->m, 1);
  if (ret!=0){ FREE_SIGN(); return ret-620; }
  Fq_matrix_clear(&x_Fq);

  ret = Fql_matrix_init(&x_Fql, para->M, 1, 0, 0, para);
  if (ret!=0){ FREE_SIGN(); return ret-630; }
  Fql_matrix_clear(&x_Fql, para);

  // compute mu
  ret = compute_mu(mu, M, Mlen, pk_seed, para);
  if (ret!=0){ FREE_SIGN(); return ret-640; }

  // init r_ctx
  ret = r_ctx_init(&r_ctx, r_seed, para);
  if (ret!=0){ FREE_SIGN(); return ret-680; }

  int is_consistent = 0;
  do{
    // compute r
    ret = compute_r(r_ctx, r, para);
    if (ret!=0){ FREE_SIGN(); return ret-700; }

    // compute t, b
    ret = Hash(&t, mu, r, para);
    if (ret!=0){ FREE_SIGN(); return ret-740; }

    ret = Fq_matrix_sub(&b, &t, &u, para);
    if (ret!=0){ FREE_SIGN(); return ret-780; }

    // solve
    ret = LU_decompose(P, &LL, &LU, &rank, first_col_index, &L, para);
    if (ret!=0){ FREE_SIGN(); return ret-790; }

    ret = consistent(&is_consistent, &L_inv, &has_L_inv, P, &LL, rank, &b, para);
    if (ret!=0){ FREE_SIGN(); return ret-810; }
  }while (!is_consistent);

  // sample a solution
  ret = sample_a_solution(&x_Fq, P, &LL, &LU, rank, first_col_index, &b, x_seed, para);
  if (ret!=0){ FREE_SIGN(); return ret-820; }

  for (int i=0; i<(x_Fql.size); i++){
    for (int j=0; j<para->l; j++){
      x_Fql.data[i][j] = x_Fq.data[i*para->l+j];
    }
  }

  Fql_matrix_transpose(&SdT);

  // compute s
  ret = compute_s(s, &y, &x_Fql, &SdT, para);  // 変数SdTは上で転置されているため、中身はSdに等しい
  if (ret!=0){ FREE_SIGN(); return ret-860; }

  // free
  FREE_SIGN();

  return 0;
}


#define FREE_SIGN_OP()                                              \
  do{                                                               \
    if(P!=NULL) memset(P, 0, para->m);                              \
    if(first_col_index!=NULL) memset(first_col_index, 0, para->m);  \
    Fql_matrix_clear_op(&y, para);                                  \
    Fq_matrix_clear(&u);                                            \
    Fq_matrix_clear(&L);                                            \
    Fql_matrix_clear_op(&SdT, para);                                \
    Fq_matrix_clear(&b);                                            \
    Fq_matrix_clear(&L_inv);                                        \
    Fq_matrix_clear(&LU);                                           \
    Fq_matrix_clear(&LL);                                           \
                                                                    \
    if (para->prg_shake==1){                                        \
      SHAKE_free((SHAKE_CTX**)&r_ctx);                              \
    }else{                                                          \
      AES_free((AES_CTX**)&r_ctx);                                  \
    }                                                               \
    if(P!=NULL) free(P);                                            \
    if(first_col_index!=NULL) free(first_col_index);                \
    Fql_matrix_free_op(&y);                                         \
    Fq_matrix_free(&u);                                             \
    Fq_matrix_free(&L);                                             \
    for (int i=0; i<para->m; i++){                                  \
      Fql_matrix_free_op(&(P2T[i]));                                \
      Fql_matrix_free_op(&(P1[i]));                                 \
    }                                                               \
    Fql_matrix_free_op(&SdT);                                       \
    Fql_matrix_free_op(&x_Fql);                                     \
    Fq_matrix_free(&x_Fq);                                          \
    Fq_matrix_free(&b);                                             \
    Fq_matrix_free(&L_inv);                                         \
    Fq_matrix_free(&LU);                                            \
    Fq_matrix_free(&LL);                                            \
    Fq_matrix_free(&t);                                             \
  }while (0)                                                        \

int sign_op(unsigned char* r, FQL_MATRIX_OP* s, const unsigned char* M,
            const unsigned long long Mlen, const unsigned char* sk_seed,
            const unsigned char* pk_seed, const unsigned char* r_seed,
            const unsigned char* y_seed, const unsigned char* x_seed, QRUOV_params* para){

  int ret = 0;

  void* r_ctx = NULL;

  FQL_MATRIX_OP SdT = {.data = NULL};
  FQL_MATRIX_OP P1[para->m];
  for (int i=0; i<para->m; i++){
    P1[i].data = NULL;
  }
  FQL_MATRIX_OP P2T[para->m];
  for (int i=0; i<para->m; i++){
    P2T[i].data = NULL;
  }
  FQ_MATRIX L = {.data = NULL};
  FQ_MATRIX u = {.data = NULL};
  FQL_MATRIX_OP y = {.data = NULL};

  unsigned char mu[64] = {0};
  FQ_MATRIX t = {.data = NULL};
  int* P = NULL;
  int rank = 0;
  int* first_col_index = NULL;

  FQ_MATRIX LL = {.data = NULL};
  FQ_MATRIX LU = {.data = NULL};
  FQ_MATRIX L_inv = {.data = NULL};
  int has_L_inv = 0;
  FQ_MATRIX b = {.data = NULL};
  FQ_MATRIX x_Fq = {.data = NULL};
  FQL_MATRIX_OP x_Fql = {.data = NULL};

  P = (int*)malloc(sizeof(int) * para->m);
  if (P == NULL){
#ifdef DEBUG
    fprintf(stderr, "[sign_op] P malloc failed.\n");
#endif
    FREE_SIGN_OP(); return -1;
  }
  memset(P, 0, para->m);

  first_col_index = (int*)malloc(sizeof(int) * para->m);
  if (first_col_index == NULL){
#ifdef DEBUG
    fprintf(stderr, "[sign_op] first_col_index malloc failed.\n");
#endif
    FREE_SIGN_OP(); return -2;
  }
  memset(first_col_index, 0, para->m);

  // init
  ret = Fql_matrix_init_op(&SdT, para->V, para->M, 0, 1, para);
  if (ret!=0) return ret-10;
  Fql_matrix_clear_op(&SdT, para);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init_op(&(P1[i]), para->V, para->V, 1, 0, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-20; }

    ret = Fql_matrix_init_op(&(P2T[i]), para->V, para->M, 0, 1, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-30; }

    Fql_matrix_clear_op(&(P1[i]), para);
    Fql_matrix_clear_op(&(P2T[i]), para);
  }

  ret = Fq_matrix_init(&L, para->m, para->m);
  if (ret!=0){ FREE_SIGN_OP(); return ret-40; }
  Fq_matrix_clear(&L);

  ret = Fq_matrix_init(&u, para->m, 1);
  if (ret!=0){ FREE_SIGN_OP(); return ret-50; }
  Fq_matrix_clear(&u);

  ret = Fql_matrix_init_op(&y, para->V, 1, 0, 0, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-60; }
  Fql_matrix_clear_op(&y, para);

  // compute SdT, y
  ret = compute_SdT_op(&SdT, sk_seed, para);
  if (ret!=0){ FREE_SIGN_OP();  return ret-70; }

  ret = compute_y_op(&y, y_seed, para);
  if (ret!=0){ FREE_SIGN_OP();  return ret-120; }

  // compute P1, P2T, L, u
  ret = compute_P1_P2T_L_u_op(P1, P2T, &L, &u, &SdT, &y, pk_seed, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-170; }

  // init
  ret = Fq_matrix_init(&t, para->m, 1);
  if (ret!=0){ FREE_SIGN_OP(); return ret-570; }
  Fq_matrix_clear(&t);

  ret = Fq_matrix_init(&LL, para->m, para->m);
  if (ret!=0){ FREE_SIGN_OP(); return ret-580; }
  Fq_matrix_clear(&LL);

  ret = Fq_matrix_init(&LU, para->m, para->m);
  if (ret!=0){ FREE_SIGN_OP(); return ret-590; }
  Fq_matrix_clear(&LU);

  ret = Fq_matrix_init(&L_inv, para->m, para->m);
  if (ret!=0){ FREE_SIGN_OP(); return ret-600; }
  Fq_matrix_clear(&L_inv);

  ret = Fq_matrix_init(&b, para->m, 1);
  if (ret!=0){ FREE_SIGN_OP(); return ret-610; }
  Fq_matrix_clear(&b);

  ret = Fq_matrix_init(&x_Fq, para->m, 1);
  if (ret!=0){ FREE_SIGN_OP(); return ret-620; }
  Fq_matrix_clear(&x_Fq);

  ret = Fql_matrix_init_op(&x_Fql, para->M, 1, 0, 0, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-630; }
  Fql_matrix_clear_op(&x_Fql, para);

  // compute mu
  ret = compute_mu(mu, M, Mlen, pk_seed, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-640; }

  // init r_ctx
  ret = r_ctx_init(&r_ctx, r_seed, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-680; }

  int is_consistent = 0;
  do{
    // compute r
    ret = compute_r(r_ctx, r, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-700; }

    // compute t, b
    ret = Hash(&t, mu, r, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-740; }

    ret = Fq_matrix_sub(&b, &t, &u, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-780; }

    // solve
    ret = LU_decompose(P, &LL, &LU, &rank, first_col_index, &L, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-790; }

    ret = consistent(&is_consistent, &L_inv, &has_L_inv, P, &LL, rank, &b, para);
    if (ret!=0){ FREE_SIGN_OP(); return ret-810; }
  }while (!is_consistent);

  // sample a solution
  ret = sample_a_solution(&x_Fq, P, &LL, &LU, rank, first_col_index, &b, x_seed, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-820; }

  for (int i=0; i<(x_Fql.size); i++){
    //for (int j=0; j<para->l; j++){
      //x_Fql.data[i][j] = x_Fq.data[i*para->l+j];
      x_Fql.data[i] = (uint64_t)x_Fq.data[i*para->l+0];         // a[0]
      x_Fql.data[i] |= ((uint64_t)x_Fq.data[i*para->l+1])<<16;  // a[1]
      x_Fql.data[i] |= ((uint64_t)x_Fq.data[i*para->l+2])<<32;  // a[2]

    //}
  }

  Fql_matrix_transpose_op(&SdT);

  // compute s
  // 変数SdTは上で転置されているため、中身はSdに等しい
  ret = compute_s_op(s, &y, &x_Fql, &SdT, para);
  if (ret!=0){ FREE_SIGN_OP(); return ret-860; }

  // free
  FREE_SIGN_OP();

  return 0;
}

#define FREE_VERIFY()               \
  do{                               \
    Fq_matrix_free(&td);            \
    Fq_matrix_free(&t);             \
    for (int i=0; i<para->m; i++){  \
      Fql_matrix_free(&(P2T[i]));   \
      Fql_matrix_free(&(P1[i]));    \
    }                               \
  }while (0)                        \

int verify(const unsigned char* M, const unsigned long long Mlen, const unsigned char* pk_seed,
           const FQL_MATRIX* P3, const unsigned char* r, const FQL_MATRIX* s, QRUOV_params* para){

  int ret = 0;

  // init
  FQL_MATRIX P1[para->m];
  FQL_MATRIX P2T[para->m];
  for(int i=0; i<para->m; i++){
    P1[i].data = NULL;
    P2T[i].data = NULL;
  }
  unsigned char mu[64] = {0};
  FQ_MATRIX t = {.data = NULL};
  FQ_MATRIX td = {.data = NULL};

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P1[i]), para->V, para->V, 1, 0, para);
    if (ret!=0){ FREE_VERIFY(); return ret-0; }

    ret = Fql_matrix_init(&(P2T[i]), para->V, para->M, 0, 1, para);
    if (ret!=0){ FREE_VERIFY(); return ret-10; }

    Fql_matrix_clear(&(P1[i]), para);
    Fql_matrix_clear(&(P2T[i]), para);
  }

  ret = Fq_matrix_init(&t, para->m, 1);
  if (ret!=0){ FREE_VERIFY(); return ret-20; }
  Fq_matrix_clear(&t);

  ret = Fq_matrix_init(&td, para->m, 1);
  if (ret!=0){ FREE_VERIFY(); return ret-30; }
  Fq_matrix_clear(&td);

  // compute P1, P2T
  ret = compute_P1_P2T(P1, P2T, pk_seed, para);
  if (ret!=0){ FREE_VERIFY(); return ret-40; }

  // compute mu, t, td
  ret = compute_mu(mu, M, Mlen, pk_seed, para);
  if (ret!=0){ FREE_VERIFY(); return ret-150; }

  ret = Hash(&t, mu, r, para);
  if (ret!=0){ FREE_VERIFY(); return ret-190; }

  ret = compute_td(&td, s, P1, P2T, P3, para);
  if (ret!=0){ FREE_VERIFY(); return ret-230; }

  for (int i=0; i<(t.size); i++){
    if (t.data[i] != td.data[i]){
#ifdef DEBUG
      fprintf(stderr, "[verify] signature rejected.\n");
#endif
      FREE_VERIFY();
      return 0;
    }
  }

  // free
  FREE_VERIFY();

#ifdef DEBUG
  fprintf(stderr, "[verify] signature accepted.\n");
#endif
  return 1;
}


#define FREE_VERIFY_OP()            \
  do{                               \
    Fq_matrix_free(&td);            \
    Fq_matrix_free(&t);             \
    for (int i=0; i<para->m; i++){  \
      Fql_matrix_free_op(&(P2T[i]));\
      Fql_matrix_free_op(&(P1[i])); \
    }                               \
  }while (0)                        \

int verify_op(const unsigned char* M, const unsigned long long Mlen, const unsigned char* pk_seed,
              const FQL_MATRIX_OP* P3, const unsigned char* r, const FQL_MATRIX_OP* s,
              QRUOV_params* para){

  int ret = 0;

  // init
  FQL_MATRIX_OP P1[para->m];
  FQL_MATRIX_OP P2T[para->m];
  for(int i=0; i<para->m; i++){
    P1[i].data = NULL;
    P2T[i].data = NULL;
  }
  unsigned char mu[64] = {0};
  FQ_MATRIX t = {.data = NULL};
  FQ_MATRIX td = {.data = NULL};

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init_op(&(P1[i]), para->V, para->V, 1, 0, para);
    if (ret!=0){ FREE_VERIFY_OP(); return ret-0; }

    ret = Fql_matrix_init_op(&(P2T[i]), para->V, para->M, 0, 1, para);
    if (ret!=0){ FREE_VERIFY_OP(); return ret-10; }

    Fql_matrix_clear_op(&(P1[i]), para);
    Fql_matrix_clear_op(&(P2T[i]), para);
  }

  ret = Fq_matrix_init(&t, para->m, 1);
  if (ret!=0){ FREE_VERIFY_OP(); return ret-20; }
  Fq_matrix_clear(&t);

  ret = Fq_matrix_init(&td, para->m, 1);
  if (ret!=0){ FREE_VERIFY_OP(); return ret-30; }
  Fq_matrix_clear(&td);

  // compute P1, P2T
  ret = compute_P1_P2T_op(P1, P2T, pk_seed, para);
  if (ret!=0){ FREE_VERIFY_OP(); return ret-40; }

  // compute mu, t, td
  ret = compute_mu(mu, M, Mlen, pk_seed, para);
  if (ret!=0){ FREE_VERIFY_OP(); return ret-150; }

  ret = Hash(&t, mu, r, para);
  if (ret!=0){ FREE_VERIFY_OP(); return ret-190; }

  ret = compute_td_op(&td, s, P1, P2T, P3, para);
  if (ret!=0){ FREE_VERIFY_OP(); return ret-230; }

  for (int i=0; i<(t.size); i++){
    if (t.data[i] != td.data[i]){
#ifdef DEBUG
      fprintf(stderr, "[verify_op] signature rejected.\n");
#endif
      FREE_VERIFY_OP();
      return 0;
    }
  }

  // free
  FREE_VERIFY_OP();

#ifdef DEBUG
  fprintf(stderr, "[verify_op] signature accepted.\n");
#endif
  return 1;
}
