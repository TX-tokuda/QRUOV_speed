/**
 * @file  qruov.c
 * @brief QR-UOV署名ライブラリの外部インタフェースのソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#ifdef DEBUG
  #include <stdio.h>
#endif

#include "qruov_signature.h"
#include "der.h"
#include "parameter.h"

const uint8_t Fq_inv_table_q7[7] = {0,1,4,5,2,3,6};
const uint8_t Fq_inv_table_q31[31] = {
     0, 1,16,21, 8,25,26, 9,
     4, 7,28,17,13,12,20,29,
     2,11,19,18,14, 3,24,27,
    22, 5, 6,23,10,15,30
  };
const uint8_t Fq_inv_table_q127[127] = {
      0,  1, 64, 85, 32, 51,106,109, 16,113, 89,104, 53, 88,118, 17,
      8, 15,120,107,108,121, 52,116, 90, 61, 44, 80, 59, 92, 72, 41,
      4, 77, 71, 98, 60,103,117,114, 54, 31,124, 65, 26, 48, 58,100,
     45, 70, 94,  5, 22, 12, 40, 97, 93, 78, 46, 28, 36, 25, 84,125,

      2, 43,102, 91, 99, 81, 49, 34, 30, 87,115,105,122, 33, 57, 82,
     27, 69, 79,101, 62,  3, 96, 73, 13, 10, 24, 67, 29, 56, 50,123,
     86, 55, 35, 68, 47, 83, 66, 37, 11, 75,  6, 19, 20,  7,112,119,
    110,  9, 39, 74, 23, 38, 14,111, 18, 21, 76, 95, 42, 63,126
  };

int QRUOV_paramgen(char* param_name,char* prg, QRUOV_params* para){

  if ((strcmp(param_name, "qruov1q127L3v156m54")   != 0) &&
      (strcmp(param_name, "qruov1q31L3v165m60")    != 0) &&
      (strcmp(param_name, "qruov1q31L10v600m70")   != 0) &&
      (strcmp(param_name, "qruov1q7L10v740m100")   != 0) &&

      (strcmp(param_name, "qruov3q127L3v228m78")   != 0) &&
      (strcmp(param_name, "qruov3q31L3v246m87")    != 0) &&
      (strcmp(param_name, "qruov3q31L10v890m100")  != 0) &&
      (strcmp(param_name, "qruov3q7L10v1100m140")  != 0) &&

      (strcmp(param_name, "qruov5q127L3v306m105")  != 0) &&
      (strcmp(param_name, "qruov5q31L3v324m114")   != 0) &&
      (strcmp(param_name, "qruov5q31L10v1120m120") != 0) &&
      (strcmp(param_name, "qruov5q7L10v1490m190")  != 0)){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_paramgen] param_name must be "
                    "\"qruov1q127L3v156m54\", \"qruov1q31L3v165m60\", "
                    "\"qruov1q31L10v600m70\", \"qruov1q7L10v740m100\", "
                    "\"qruov3q127L3v228m78\", \"qruov3q31L3v246m87\", "
                    "\"qruov3q31L10v890m100\", \"qruov3q7L10v1100m140\", "
                    "\"qruov5q127L3v306m105\", \"qruov5q31L3v324m114\", "
                    "\"qruov5q31L10v1120m120\" or \"qruov5q7L10v1490m190\".\n");
#endif
    return -1;
  }

  if (strcmp(prg, "SHAKE")!=0 && strcmp(prg, "AES")!=0){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_paramgen] prg must be \"SHAKE\" or \"AES\".\n");
#endif
    return -2;
  }

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_paramgen] para must not be NULL.\n");
#endif
    return -3;
  }

  if (strcmp(param_name, "qruov1q127L3v156m54") == 0){
    para->parameter = 1;
    para->SL        = 1;
    para->q         = 127;
    para->v         = 156;
    para->m         = 54;
    para->l         = 3;
    para->tau_lVM   = 2916;
    para->tau_lVV12 = 4267;
    para->tau_lV    = 192;
    para->tau_m     = 82;

  }else if (strcmp(param_name, "qruov1q31L3v165m60") == 0){
    para->parameter = 2;
    para->SL        = 1;
    para->q         = 31;
    para->v         = 165;
    para->m         = 60;
    para->l         = 3;
    para->tau_lVM   = 3571;
    para->tau_lVV12 = 4959;
    para->tau_lV    = 226;
    para->tau_m     = 104;

  }else if (strcmp(param_name, "qruov1q31L10v600m70") == 0){
    para->parameter = 3;
    para->SL        = 1;
    para->q         = 31;
    para->v         = 600;
    para->m         = 70;
    para->l         = 10;
    para->tau_lVM   = 4518;
    para->tau_lVV12 = 19242;
    para->tau_lV    = 704;
    para->tau_m     = 116;

  }else if (strcmp(param_name, "qruov1q7L10v740m100") == 0){
    para->parameter = 4;
    para->SL        = 1;
    para->q         = 7;
    para->v         = 740;
    para->m         = 100;
    para->l         = 10;
    para->tau_lVM   = 8947;
    para->tau_lVV12 = 32629;
    para->tau_lV    = 1024;
    para->tau_m     = 201;

  }else if (strcmp(param_name, "qruov3q127L3v228m78") == 0){
    para->parameter = 5;
    para->SL        = 3;
    para->q         = 127;
    para->v         = 228;
    para->m         = 78;
    para->l         = 3;
    para->tau_lVM   = 6123;
    para->tau_lVV12 = 9020;
    para->tau_lV    = 283;
    para->tau_m     = 120;

  }else if (strcmp(param_name, "qruov3q31L3v246m87") == 0){
    para->parameter = 6;
    para->SL        = 3;
    para->q         = 31;
    para->v         = 246;
    para->m         = 87;
    para->l         = 3;
    para->tau_lVM   = 7655;
    para->tau_lVV12 = 10878;
    para->tau_lV    = 338;
    para->tau_m     = 154;

  }else if (strcmp(param_name, "qruov3q31L10v890m100") == 0){
    para->parameter = 7;
    para->SL        = 3;
    para->q         = 31;
    para->v         = 890;
    para->m         = 100;
    para->l         = 10;
    para->tau_lVM   = 9507;
    para->tau_lVV12 = 41974;
    para->tau_lV    = 1046;
    para->tau_m     = 169;

  }else if (strcmp(param_name, "qruov3q7L10v1100m140") == 0){
    para->parameter = 8;
    para->SL        = 3;
    para->q         = 7;
    para->v         = 1100;
    para->m         = 140;
    para->l         = 10;
    para->tau_lVM   = 18461;
    para->tau_lVV12 = 71432;
    para->tau_lV    = 1526;
    para->tau_m     = 289;

  }else if (strcmp(param_name, "qruov5q127L3v306m105") == 0){
    para->parameter = 9;
    para->SL        = 5;
    para->q         = 127;
    para->v         = 306;
    para->m         = 105;
    para->l         = 3;
    para->tau_lVM   = 11018;
    para->tau_lVV12 = 16144;
    para->tau_lV    = 380;
    para->tau_m     = 162;

  }else if (strcmp(param_name, "qruov5q31L3v324m114") == 0){
    para->parameter = 10;
    para->SL        = 5;
    para->q         = 31;
    para->v         = 324;
    para->m         = 114;
    para->l         = 3;
    para->tau_lVM   = 13145;
    para->tau_lVV12 = 18738;
    para->tau_lV    = 447;
    para->tau_m     = 203;

  }else if (strcmp(param_name, "qruov5q31L10v1120m120") == 0){
    para->parameter = 11;
    para->SL        = 5;
    para->q         = 31;
    para->v         = 1120;
    para->m         = 120;
    para->l         = 10;
    para->tau_lVM   = 14326;
    para->tau_lVV12 = 66236;
    para->tau_lV    = 1324;
    para->tau_m     = 210;

  }else if (strcmp(param_name, "qruov5q7L10v1490m190") == 0){
    para->parameter = 12;
    para->SL        = 5;
    para->q         = 7;
    para->v         = 1490;
    para->m         = 190;
    para->l         = 10;
    para->tau_lVM   = 33694;
    para->tau_lVV12 = 130305;
    para->tau_lV    = 2065;
    para->tau_m     = 391;
  }

  if (para->SL==1){
    para->lambda = 128;
  }else if (para->SL==3){
    para->lambda = 192;
  }else if (para->SL==5){
    para->lambda = 256;
  }

  if (para->q==127){
    para->q_bitlen = 7;
    para->Fq_inv_table      = Fq_inv_table_q127;
  }else if (para->q==31){
    para->q_bitlen = 5;
    para->Fq_inv_table      = Fq_inv_table_q31;
  }else if (para->q==7){
    para->q_bitlen = 3;
    para->Fq_inv_table      = Fq_inv_table_q7;
  }

  para->n = para->v + para->m;
  para->V = para->v / para->l;
  para->M = para->m / para->l;
  para->N = para->n / para->l;
  para->Fql_degree = para->l - 1;
  para->Fql_accumulator_degree = para->Fql_degree * 2;
  para->seed_len = para->lambda / 8;
  para->salt_len = para->lambda / 8;

  /**
   * 既約多項式の定義
   * para->l == 3の場合
   *    x^3 - x - 1
   * para->l == 10かつpara->q == 7の場合
   *    x^10 - 2x - 1
   * para->l == 10かつpara->q == 31の場合
   *    x^10 - 5x^3 - 1
   *
   * para->f_c  :中間項の係数のマイナス以外の部分
   * para->f_e  :中間項の次数
   * para->f_c0 :定数項のマイナス以外の部分
   */
  if ((para->q==127) && (para->l==3)){
    para->f_c = 1;
    para->f_e = 1;
    para->f_c0 = 1;
  }else if ((para->q==7) && (para->l==10)){
    para->f_c = 2;
    para->f_e = 1;
    para->f_c0 = 1;
  }else if ((para->q==31) && (para->l==3)){
    para->f_c = 1;
    para->f_e = 1;
    para->f_c0 = 1;
  }else if ((para->q==31) && (para->l==10)){
    para->f_c = 5;
    para->f_e = 3;
    para->f_c0 = 1;
  }

  // QRUOVPrivateKey(SEQUENCE)の長さは128Byte未満
  para->sk_len = 2 * (para->seed_len) + 9;

  if ((para->SL==1) || (para->parameter==7)){
    // QRUOVPublicKey(SEQUENCE)の長さが256Byte以上65536Byte未満
    para->pk_len = para->seed_len \
                         + (para->l * para->m * para->M * (para->M+1)) \
                           / 2 \
                         + 4 * para->m + 10;
  }else{
    // QRUOVPublicKey(SEQUENCE)の長さが65536Byte以上
    para->pk_len = para->seed_len \
                         + (para->l * para->m * para->M * (para->M+1)) \
                           / 2 \
                         + 4 * para->m + 12;
  }

  if ((para->parameter==1) || (para->parameter==2)){
    // QRUOVSignature(SEQUENCE)の長さが128Byte以上256Byte未満
    para->sigma_len = para->salt_len + para->n + 8;
  }else{
    // QRUOVSignature(SEQUENCE)の長さが256Byte以上
    para->sigma_len = para->salt_len + para->n + 10;
  }

  if (strcmp(prg, "SHAKE")==0){
    para->prg_shake = 1;
  }else if (strcmp(prg, "AES")==0){
    para->prg_shake = 0;
  }

  para->prg_ctx = NULL;

  if (para->SL==1){
    para->SHAKE_len = 16;
  }else if ((para->SL==3) || (para->SL==5)){
    para->SHAKE_len = 32;
  }

  if (para->SL==1){
    para->AES_len = 16;
  }else if (para->SL==3){
    para->AES_len = 24;
  }else if (para->SL==5){
    para->AES_len = 32;
  }

  return 0;
}

/**
 * QR-UOVの鍵生成を行う関数の汎用関数。
 *
 * @param[out]    pk    公開鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] pklen pkのサイズ(Byte)。関数内でpkに書き込まれたサイズが格納される。
 * @param[out]    sk    秘密鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] sklen skのサイズ(Byte)。関数内でskに書き込まれたサイズが格納される。
 * @param[in]     para  パラメータの構造体。
 * @return              0:成功。-1以下:エラー。
 */
int QRUOV_keygen_gn(unsigned char* pk, size_t* pklen, unsigned char* sk,
                    size_t* sklen, QRUOV_params* para){
                  
  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -5;
  }
                  
  if (pk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] pk must not be NULL.\n");
#endif
    return -1;
  }

  if (*pklen < para->pk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] pklen must be >= para->pk_len.\n");
#endif
    return -2;
  }

  if (sk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] sk must not be NULL.\n");
#endif
    return -3;
  }

  if (*sklen < para->sk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] sklen must be >= para->sk_len.\n");
#endif
    return -4;
  }

  // init
  int ret_code = 0, ret = 0;

  unsigned char* sk_seed = NULL;
  unsigned char* pk_seed = NULL;
  unsigned char* tmp_sk = NULL;
  unsigned char* tmp_pk = NULL;
  FQL_MATRIX P3[para->m];
  for (int i=0; i<para->m; i++){
    P3[i].data = NULL;
  }

  ret = string_init(&sk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; }
  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-110; goto end; }
  ret = string_init(&tmp_sk, para->sk_len);
  if (ret!=0){ ret_code = ret-120; goto end; }
  ret = string_init(&tmp_pk, para->pk_len);
  if (ret!=0){ ret_code = ret-130; goto end; }

  byte_random_private(sk_seed, para->seed_len);
  byte_random(pk_seed, para->seed_len);
  memset(tmp_sk, 0, para->sk_len);
  memset(tmp_pk, 0, para->pk_len);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P3[i]), para->M, para->M, 1, 0, para);
    if (ret!=0){ ret_code = ret-140; goto end; }
    Fql_matrix_clear(&(P3[i]), para);
  }

  // keygen
  ret = keygen(P3, sk_seed, pk_seed, para);
  if (ret!=0){ ret_code = ret-150; goto end; }

  // DER encode
  ret = sk2der(tmp_sk, sk_seed, pk_seed, para);
  if (ret!=0){ ret_code = ret-540; goto end; }
  ret = pk2der(tmp_pk, pk_seed, P3, para);
  if (ret!=0){ ret_code = ret-600; goto end; }

  memcpy(sk, tmp_sk, para->sk_len);
  memcpy(pk, tmp_pk, para->pk_len);
  *sklen = para->sk_len;
  *pklen = para->pk_len;

end:
  // free
  for (int i=0; i<para->m; i++){
    Fql_matrix_free(&(P3[i]));
  }
  if(sk_seed!=NULL){
    memset(sk_seed, 0, para->seed_len);
  }
  if(tmp_sk!=NULL){
    memset(tmp_sk, 0, para->sk_len);
  }
  string_free(&sk_seed);
  string_free(&pk_seed);
  string_free(&tmp_sk);
  string_free(&tmp_pk);

  return ret_code;
}

/**
 * QR-UOVの鍵生成を行う関数の汎用関数のq=127における最適化関数。
 *
 * @param[out]    pk    公開鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] pklen pkのサイズ(Byte)。関数内でpkに書き込まれたサイズが格納される。
 * @param[out]    sk    秘密鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] sklen skのサイズ(Byte)。関数内でskに書き込まれたサイズが格納される。
 * @param[in]     para  パラメータの構造体。
 * @return              0:成功。-1以下:エラー。
 */
int QRUOV_keygen_op(unsigned char* pk, size_t* pklen, unsigned char* sk,
                 size_t* sklen, QRUOV_params* para){
                  
  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_op] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -5;
  }
                  
  if (pk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_op] pk must not be NULL.\n");
#endif
    return -1;
  }

  if (*pklen < para->pk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_op] pklen must be >= para->pk_len.\n");
#endif
    return -2;
  }

  if (sk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_op] sk must not be NULL.\n");
#endif
    return -3;
  }

  if (*sklen < para->sk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_op] sklen must be >= para->sk_len.\n");
#endif
    return -4;
  }

  // init
  int ret_code = 0, ret = 0;

  unsigned char* sk_seed = NULL;
  unsigned char* pk_seed = NULL;
  unsigned char* tmp_sk = NULL;
  unsigned char* tmp_pk = NULL;
  FQL_MATRIX_OP P3[para->m];
  for (int i=0; i<para->m; i++){
    P3[i].data = NULL;
  }

  ret = string_init(&sk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; }
  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-110; goto end; }
  ret = string_init(&tmp_sk, para->sk_len);
  if (ret!=0){ ret_code = ret-120; goto end; }
  ret = string_init(&tmp_pk, para->pk_len);
  if (ret!=0){ ret_code = ret-130; goto end; }

  byte_random_private(sk_seed, para->seed_len);
  byte_random(pk_seed, para->seed_len);
  memset(tmp_sk, 0, para->sk_len);
  memset(tmp_pk, 0, para->pk_len);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init_op(&(P3[i]), para->M, para->M, 1, 0, para);
    if (ret!=0){ ret_code = ret-140; goto end; }
    Fql_matrix_clear_op(&(P3[i]), para);
  }

  // keygen
  ret = keygen_op(P3, sk_seed, pk_seed, para);
  if (ret!=0){ ret_code = ret-150; goto end; }

  // DER encode
  ret = sk2der(tmp_sk, sk_seed, pk_seed, para);
  if (ret!=0){ ret_code = ret-540; goto end; }
  ret = pk2der_op(tmp_pk, pk_seed, P3, para);
  if (ret!=0){ ret_code = ret-600; goto end; }

  memcpy(sk, tmp_sk, para->sk_len);
  memcpy(pk, tmp_pk, para->pk_len);
  *sklen = para->sk_len;
  *pklen = para->pk_len;

end:
  // free
  for (int i=0; i<para->m; i++){
    Fql_matrix_free_op(&(P3[i]));
  }
  if(sk_seed!=NULL){
    memset(sk_seed, 0, para->seed_len);
  }
  if(tmp_sk!=NULL){
    memset(tmp_sk, 0, para->sk_len);
  }
  string_free(&sk_seed);
  string_free(&pk_seed);
  string_free(&tmp_sk);
  string_free(&tmp_pk);

  return ret_code;
}

/**
 * QR-UOVの鍵生成を行う関数のparameter==2における最適化関数。
 *
 * @param[out]    pk    公開鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] pklen pkのサイズ(Byte)。関数内でpkに書き込まれたサイズが格納される。
 * @param[out]    sk    秘密鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] sklen skのサイズ(Byte)。関数内でskに書き込まれたサイズが格納される。
 * @param[in]     para  パラメータの構造体。
 * @return              0:成功。-1以下:エラー。
 */
int QRUOV_keygen_op2(unsigned char* pk, size_t* pklen, unsigned char* sk,
                    size_t* sklen, QRUOV_params* para){
                  
  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -5;
  }
                  
  if (pk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] pk must not be NULL.\n");
#endif
    return -1;
  }

  if (*pklen < para->pk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] pklen must be >= para->pk_len.\n");
#endif
    return -2;
  }

  if (sk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] sk must not be NULL.\n");
#endif
    return -3;
  }

  if (*sklen < para->sk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_keygen_gn] sklen must be >= para->sk_len.\n");
#endif
    return -4;
  }

  // init
  int ret_code = 0, ret = 0;

  unsigned char* sk_seed = NULL;
  unsigned char* pk_seed = NULL;
  unsigned char* tmp_sk = NULL;
  unsigned char* tmp_pk = NULL;
  FQL_MATRIX P3[para->m];
  for (int i=0; i<para->m; i++){
    P3[i].data = NULL;
  }

  ret = string_init(&sk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; }
  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-110; goto end; }
  ret = string_init(&tmp_sk, para->sk_len);
  if (ret!=0){ ret_code = ret-120; goto end; }
  ret = string_init(&tmp_pk, para->pk_len);
  if (ret!=0){ ret_code = ret-130; goto end; }

  byte_random_private(sk_seed, para->seed_len);
  byte_random(pk_seed, para->seed_len);
  memset(tmp_sk, 0, para->sk_len);
  memset(tmp_pk, 0, para->pk_len);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P3[i]), para->M, para->M, 1, 0, para);
    if (ret!=0){ ret_code = ret-140; goto end; }
    Fql_matrix_clear(&(P3[i]), para);
  }

  // keygen
  ret = keygen_op2(P3, sk_seed, pk_seed, para);
  if (ret!=0){ ret_code = ret-150; goto end; }

  // DER encode
  ret = sk2der(tmp_sk, sk_seed, pk_seed, para);
  if (ret!=0){ ret_code = ret-540; goto end; }
  ret = pk2der(tmp_pk, pk_seed, P3, para);
  if (ret!=0){ ret_code = ret-600; goto end; }

  memcpy(sk, tmp_sk, para->sk_len);
  memcpy(pk, tmp_pk, para->pk_len);
  *sklen = para->sk_len;
  *pklen = para->pk_len;

end:
  // free
  for (int i=0; i<para->m; i++){
    Fql_matrix_free(&(P3[i]));
  }
  if(sk_seed!=NULL){
    memset(sk_seed, 0, para->seed_len);
  }
  if(tmp_sk!=NULL){
    memset(tmp_sk, 0, para->sk_len);
  }
  string_free(&sk_seed);
  string_free(&pk_seed);
  string_free(&tmp_sk);
  string_free(&tmp_pk);

  return ret_code;
}

/**
 * QR-UOVの署名生成を行う関数の汎用関数。
 *
 * @param[in]     sk     秘密鍵。
 * @param[in]     sklen  skのサイズ(Byte)。
 * @param[out]    sig    署名。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] siglen sigのサイズ(Byte)。関数内でsigに書き込まれたサイズが格納される。
 * @param[in]     m      メッセージ。
 * @param[in]     mlen   メッセージサイズ(Byte)。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int QRUOV_sign_gn(unsigned char* sk, size_t sklen, unsigned char* sig,
                  size_t* siglen, unsigned char* m, size_t mlen,
                  QRUOV_params* para){

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -6;
  }

  if (sk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] sk must not be NULL.\n");
#endif
    return -1;
  }

  if (sklen != para->sk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] sklen must be == para->sk_len.\n");
#endif
    return -2;
  }

  if (sig==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] sig must not be NULL.\n");
#endif
    return -3;
  }

  if (*siglen < para->sigma_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] siglen must be >= para->sigma_len.\n");
#endif
    return -4;
  }

  if (m==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] m must not be NULL.\n");
#endif
    return -5;
  }

  // init
  int ret_code = 0, ret = 0;
  unsigned char* sk_seed = NULL;
  unsigned char* pk_seed = NULL;
  unsigned char* r_seed = NULL;
  unsigned char* y_seed = NULL;
  unsigned char* x_seed = NULL;
  unsigned char* r = NULL;
  unsigned char* tmp_sig = NULL;
  FQL_MATRIX s = {.data = NULL};

  ret = string_init(&sk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; };
  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-110; goto end; };
  ret = string_init(&r_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-120; goto end; };
  ret = string_init(&y_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-130; goto end; };
  ret = string_init(&x_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-140; goto end; };
  ret = string_init(&r, para->salt_len);
  if (ret!=0){ ret_code = ret-150; goto end; };
  ret = string_init(&tmp_sig, para->sigma_len);
  if (ret!=0){ ret_code = ret-160; goto end; };

  memset(sk_seed, 0, para->seed_len);
  memset(pk_seed, 0, para->seed_len);
  byte_random(r_seed, para->seed_len);
  byte_random_private(y_seed, para->seed_len);
  byte_random_private(x_seed, para->seed_len);
  memset(r, 0, para->salt_len);
  memset(tmp_sig, 0, para->sigma_len);

  ret = Fql_matrix_init(&s, para->N, 1, 0, 0, para);
  if (ret!=0){ ret_code = ret-170; goto end; };
  Fql_matrix_clear(&s, para);

  // DER decode
  ret = der2sk(sk_seed, pk_seed, sk, para);
  if (ret!=0){ ret_code = ret-290; goto end; };

  // sign
  ret = sign(r, &s, m, mlen, sk_seed, pk_seed, r_seed, y_seed, x_seed, para);
  if (ret!=0){ ret_code = ret-360; goto end; };

  // DER encode
  ret = sigma2der(tmp_sig, r, &s, para);
  if (ret!=0){ ret_code = ret-1290; goto end; };

  memcpy(sig, tmp_sig, para->sigma_len);
  *siglen = para->sigma_len;

end:
  // free
  Fql_matrix_clear(&s, para);
  Fql_matrix_free(&s);
  if(sk_seed!=NULL){
    memset(sk_seed, 0, para->seed_len);
  }
  string_free(&sk_seed);
  string_free(&pk_seed);
  string_free(&r_seed);
  if(y_seed!=NULL){
    memset(y_seed, 0, para->seed_len);
  }
  if(x_seed!=NULL){
    memset(x_seed, 0, para->seed_len);
  }
  string_free(&y_seed);
  string_free(&x_seed);
  string_free(&r);
  string_free(&tmp_sig);

  return ret_code;
}

/**
 * QR-UOVの署名生成を行う関数のq=127における最適化関数。
 *
 * @param[in]     sk     秘密鍵。
 * @param[in]     sklen  skのサイズ(Byte)。
 * @param[out]    sig    署名。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] siglen sigのサイズ(Byte)。関数内でsigに書き込まれたサイズが格納される。
 * @param[in]     m      メッセージ。
 * @param[in]     mlen   メッセージサイズ(Byte)。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int QRUOV_sign_op(unsigned char* sk, size_t sklen, unsigned char* sig,
               size_t* siglen, unsigned char* m, size_t mlen,
               QRUOV_params* para){

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_op] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -6;
  }

  if (sk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_op] sk must not be NULL.\n");
#endif
    return -1;
  }

  if (sklen != para->sk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_op] sklen must be == para->sk_len.\n");
#endif
    return -2;
  }

  if (sig==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_op] sig must not be NULL.\n");
#endif
    return -3;
  }

  if (*siglen < para->sigma_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_op] siglen must be >= para->sigma_len.\n");
#endif
    return -4;
  }

  if (m==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_op] m must not be NULL.\n");
#endif
    return -5;
  }

  // init
  int ret_code = 0, ret = 0;
  unsigned char* sk_seed = NULL;
  unsigned char* pk_seed = NULL;
  unsigned char* r_seed = NULL;
  unsigned char* y_seed = NULL;
  unsigned char* x_seed = NULL;
  unsigned char* r = NULL;
  unsigned char* tmp_sig = NULL;
  FQL_MATRIX_OP s = {.data = NULL};

  ret = string_init(&sk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; };
  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-110; goto end; };
  ret = string_init(&r_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-120; goto end; };
  ret = string_init(&y_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-130; goto end; };
  ret = string_init(&x_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-140; goto end; };
  ret = string_init(&r, para->salt_len);
  if (ret!=0){ ret_code = ret-150; goto end; };
  ret = string_init(&tmp_sig, para->sigma_len);
  if (ret!=0){ ret_code = ret-160; goto end; };

  memset(sk_seed, 0, para->seed_len);
  memset(pk_seed, 0, para->seed_len);
  byte_random(r_seed, para->seed_len);
  byte_random_private(y_seed, para->seed_len);
  byte_random_private(x_seed, para->seed_len);
  memset(r, 0, para->salt_len);
  memset(tmp_sig, 0, para->sigma_len);

  ret = Fql_matrix_init_op(&s, para->N, 1, 0, 0, para);
  if (ret!=0){ ret_code = ret-170; goto end; };
  Fql_matrix_clear_op(&s, para);

  // DER decode
  ret = der2sk(sk_seed, pk_seed, sk, para);
  if (ret!=0){ ret_code = ret-290; goto end; };

  // sign
  ret = sign_op(r, &s, m, mlen, sk_seed, pk_seed, r_seed, y_seed, x_seed, para);
  if (ret!=0){ ret_code = ret-360; goto end; };

  // DER encode
  ret = sigma2der_op(tmp_sig, r, &s, para);
  if (ret!=0){ ret_code = ret-1290; goto end; };

  memcpy(sig, tmp_sig, para->sigma_len);
  *siglen = para->sigma_len;

end:
  // free
  Fql_matrix_clear_op(&s, para);
  Fql_matrix_free_op(&s);
  if(sk_seed!=NULL){
    memset(sk_seed, 0, para->seed_len);
  }
  string_free(&sk_seed);
  string_free(&pk_seed);
  string_free(&r_seed);
  if(y_seed!=NULL){
    memset(y_seed, 0, para->seed_len);
  }
  if(x_seed!=NULL){
    memset(x_seed, 0, para->seed_len);
  }
  string_free(&y_seed);
  string_free(&x_seed);
  string_free(&r);
  string_free(&tmp_sig);

  return ret_code;
}

/**
 * QR-UOVの署名生成を行う関数のparameter==2における最適化関数。
 *
 * @param[in]     sk     秘密鍵。
 * @param[in]     sklen  skのサイズ(Byte)。
 * @param[out]    sig    署名。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] siglen sigのサイズ(Byte)。関数内でsigに書き込まれたサイズが格納される。
 * @param[in]     m      メッセージ。
 * @param[in]     mlen   メッセージサイズ(Byte)。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int QRUOV_sign_op2(unsigned char* sk, size_t sklen, unsigned char* sig,
                  size_t* siglen, unsigned char* m, size_t mlen,
                  QRUOV_params* para){

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -6;
  }

  if (sk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] sk must not be NULL.\n");
#endif
    return -1;
  }

  if (sklen != para->sk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] sklen must be == para->sk_len.\n");
#endif
    return -2;
  }

  if (sig==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] sig must not be NULL.\n");
#endif
    return -3;
  }

  if (*siglen < para->sigma_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] siglen must be >= para->sigma_len.\n");
#endif
    return -4;
  }

  if (m==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_sign_gn] m must not be NULL.\n");
#endif
    return -5;
  }

  // init
  int ret_code = 0, ret = 0;
  unsigned char* sk_seed = NULL;
  unsigned char* pk_seed = NULL;
  unsigned char* r_seed = NULL;
  unsigned char* y_seed = NULL;
  unsigned char* x_seed = NULL;
  unsigned char* r = NULL;
  unsigned char* tmp_sig = NULL;
  FQL_MATRIX s = {.data = NULL};

  ret = string_init(&sk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; };
  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-110; goto end; };
  ret = string_init(&r_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-120; goto end; };
  ret = string_init(&y_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-130; goto end; };
  ret = string_init(&x_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-140; goto end; };
  ret = string_init(&r, para->salt_len);
  if (ret!=0){ ret_code = ret-150; goto end; };
  ret = string_init(&tmp_sig, para->sigma_len);
  if (ret!=0){ ret_code = ret-160; goto end; };

  memset(sk_seed, 0, para->seed_len);
  memset(pk_seed, 0, para->seed_len);
  byte_random(r_seed, para->seed_len);
  byte_random_private(y_seed, para->seed_len);
  byte_random_private(x_seed, para->seed_len);
  memset(r, 0, para->salt_len);
  memset(tmp_sig, 0, para->sigma_len);

  ret = Fql_matrix_init(&s, para->N, 1, 0, 0, para);
  if (ret!=0){ ret_code = ret-170; goto end; };
  Fql_matrix_clear(&s, para);

  // DER decode
  ret = der2sk(sk_seed, pk_seed, sk, para);
  if (ret!=0){ ret_code = ret-290; goto end; };

  // sign
  ret = sign_op2(r, &s, m, mlen, sk_seed, pk_seed, r_seed, y_seed, x_seed, para);
  if (ret!=0){ ret_code = ret-360; goto end; };

  // DER encode
  ret = sigma2der(tmp_sig, r, &s, para);
  if (ret!=0){ ret_code = ret-1290; goto end; };

  memcpy(sig, tmp_sig, para->sigma_len);
  *siglen = para->sigma_len;

end:
  // free
  Fql_matrix_clear(&s, para);
  Fql_matrix_free(&s);
  if(sk_seed!=NULL){
    memset(sk_seed, 0, para->seed_len);
  }
  string_free(&sk_seed);
  string_free(&pk_seed);
  string_free(&r_seed);
  if(y_seed!=NULL){
    memset(y_seed, 0, para->seed_len);
  }
  if(x_seed!=NULL){
    memset(x_seed, 0, para->seed_len);
  }
  string_free(&y_seed);
  string_free(&x_seed);
  string_free(&r);
  string_free(&tmp_sig);

  return ret_code;
}


/**
 * QR-UOVの署名検証を行う関数の汎用関数。
 *
 * @param[in] pk     公開鍵。
 * @param[in] pklen  pkのサイズ(Byte)。
 * @param[in] sig    署名。
 * @param[in] siglen sigのサイズ(Byte)。
 * @param[in] m      メッセージ。
 * @param[in] mlen   メッセージサイズ(Byte)。
 * @param[in] para   パラメータの構造体。
 * @return           0:署名を棄却。1:署名を受理。-1以下:エラー。
 */
int QRUOV_verify_gn(unsigned char* pk, size_t pklen, unsigned char* sig,
                    size_t siglen, unsigned char* m, size_t mlen,
                    QRUOV_params* para){

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -6;
  }

  if (pk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] pk must not be NULL.\n");
#endif
    return -1;
  }

  if (pklen != para->pk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] pklen must be == para->pk_len.\n");
#endif
    return -2;
  }

  if (sig==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] sig must not be NULL.\n");
#endif
    return -3;
  }

  if (siglen != para->sigma_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] siglen must be == para->sigma_len.\n");
#endif
    return -4;
  }

  if (m==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] m must not be NULL.\n");
#endif
    return -5;
  }

  // init
  int ret_code = 0, ret = 0;
  unsigned char* pk_seed = NULL;
  unsigned char* r = NULL;
  FQL_MATRIX P3[para->m];
  for (int i=0; i<para->m; i++){
    P3[i].data = NULL;
  }
  FQL_MATRIX s = {.data = NULL};

  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; };
  ret = string_init(&r, para->salt_len);
  if (ret!=0){ ret_code = ret-110; goto end; };

  memset(pk_seed, 0, para->seed_len);
  memset(r, 0, para->salt_len);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P3[i]), para->M, para->M, 1, 0, para);
    if (ret!=0){ ret_code = ret-120; goto end; };
    Fql_matrix_clear(&(P3[i]), para);
  }
  ret = Fql_matrix_init(&s, para->N, 1, 0, 0, para);
  if (ret!=0){ ret_code = ret-240; goto end; };
  Fql_matrix_clear(&s, para);

  // DER decode
  ret = der2pk(pk_seed, P3, pk, para);
  if (ret!=0){ ret_code = ret-380; goto end; };
  ret = der2sigma(r, &s, sig, para);
  if (ret!=0){ ret_code = ret-450; goto end; };

  ret = verify(m, mlen, pk_seed, P3, r, &s, para);
  if (ret!=0 && ret!=1){
    ret_code = ret-510; goto end;
  }else{
    ret_code = ret;
  }

end:
  // free
  Fql_matrix_clear(&s, para);
  Fql_matrix_free(&s);
  for (int i=0; i<para->m; i++){
    Fql_matrix_free(&(P3[i]));
  }
  string_free(&pk_seed);
  string_free(&r);

  return ret_code;
}

/**
 * QR-UOVの署名検証を行う関数のq=127における最適化関数。
 *
 * @param[in] pk     公開鍵。
 * @param[in] pklen  pkのサイズ(Byte)。
 * @param[in] sig    署名。
 * @param[in] siglen sigのサイズ(Byte)。
 * @param[in] m      メッセージ。
 * @param[in] mlen   メッセージサイズ(Byte)。
 * @param[in] para   パラメータの構造体。
 * @return           0:署名を棄却。1:署名を受理。-1以下:エラー。
 */
int QRUOV_verify_op(unsigned char* pk, size_t pklen, unsigned char* sig,
                 size_t siglen, unsigned char* m, size_t mlen,
                 QRUOV_params* para){

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_op] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -6;
  }

  if (pk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_op] pk must not be NULL.\n");
#endif
    return -1;
  }

  if (pklen != para->pk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_op] pklen must be == para->pk_len.\n");
#endif
    return -2;
  }

  if (sig==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_op] sig must not be NULL.\n");
#endif
    return -3;
  }

  if (siglen != para->sigma_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_op] siglen must be == para->sigma_len.\n");
#endif
    return -4;
  }

  if (m==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_op] m must not be NULL.\n");
#endif
    return -5;
  }

  // init
  int ret_code = 0, ret = 0;
  unsigned char* pk_seed = NULL;
  unsigned char* r = NULL;
  FQL_MATRIX_OP P3[para->m];
  for (int i=0; i<para->m; i++){
    P3[i].data = NULL;
  }
  FQL_MATRIX_OP s = {.data = NULL};

  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; };
  ret = string_init(&r, para->salt_len);
  if (ret!=0){ ret_code = ret-110; goto end; };

  memset(pk_seed, 0, para->seed_len);
  memset(r, 0, para->salt_len);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init_op(&(P3[i]), para->M, para->M, 1, 0, para);
    if (ret!=0){ ret_code = ret-120; goto end; };
    Fql_matrix_clear_op(&(P3[i]), para);
  }
  ret = Fql_matrix_init_op(&s, para->N, 1, 0, 0, para);
  if (ret!=0){ ret_code = ret-240; goto end; };
  Fql_matrix_clear_op(&s, para);

  // DER decode
  ret = der2pk_op(pk_seed, P3, pk, para);
  if (ret!=0){ ret_code = ret-380; goto end; };
  ret = der2sigma_op(r, &s, sig, para);
  if (ret!=0){ ret_code = ret-450; goto end; };

  ret = verify_op(m, mlen, pk_seed, P3, r, &s, para);
  if (ret!=0 && ret!=1){
    ret_code = ret-510; goto end;
  }else{
    ret_code = ret;
  }

end:
  // free
  Fql_matrix_clear_op(&s, para);
  Fql_matrix_free_op(&s);
  for (int i=0; i<para->m; i++){
    Fql_matrix_free_op(&(P3[i]));
  }
  string_free(&pk_seed);
  string_free(&r);

  return ret_code;
}

/**
 * QR-UOVの署名検証を行う関数のparameter==2における最適化関数。
 *
 * @param[in] pk     公開鍵。
 * @param[in] pklen  pkのサイズ(Byte)。
 * @param[in] sig    署名。
 * @param[in] siglen sigのサイズ(Byte)。
 * @param[in] m      メッセージ。
 * @param[in] mlen   メッセージサイズ(Byte)。
 * @param[in] para   パラメータの構造体。
 * @return           0:署名を棄却。1:署名を受理。-1以下:エラー。
 */
int QRUOV_verify_op2(unsigned char* pk, size_t pklen, unsigned char* sig,
                    size_t siglen, unsigned char* m, size_t mlen,
                    QRUOV_params* para){

  if (para==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] para must not be NULL.\n");
#endif
    // 他のチェックで使用するので先にチェックするがエラーコードは引数の順とする
    return -6;
  }

  if (pk==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] pk must not be NULL.\n");
#endif
    return -1;
  }

  if (pklen != para->pk_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] pklen must be == para->pk_len.\n");
#endif
    return -2;
  }

  if (sig==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] sig must not be NULL.\n");
#endif
    return -3;
  }

  if (siglen != para->sigma_len){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] siglen must be == para->sigma_len.\n");
#endif
    return -4;
  }

  if (m==NULL){
#ifdef DEBUG
    fprintf(stderr, "[QRUOV_verify_gn] m must not be NULL.\n");
#endif
    return -5;
  }

  // init
  int ret_code = 0, ret = 0;
  unsigned char* pk_seed = NULL;
  unsigned char* r = NULL;
  FQL_MATRIX P3[para->m];
  for (int i=0; i<para->m; i++){
    P3[i].data = NULL;
  }
  FQL_MATRIX s = {.data = NULL};

  ret = string_init(&pk_seed, para->seed_len);
  if (ret!=0){ ret_code = ret-100; goto end; };
  ret = string_init(&r, para->salt_len);
  if (ret!=0){ ret_code = ret-110; goto end; };

  memset(pk_seed, 0, para->seed_len);
  memset(r, 0, para->salt_len);

  for (int i=0; i<para->m; i++){
    ret = Fql_matrix_init(&(P3[i]), para->M, para->M, 1, 0, para);
    if (ret!=0){ ret_code = ret-120; goto end; };
    Fql_matrix_clear(&(P3[i]), para);
  }
  ret = Fql_matrix_init(&s, para->N, 1, 0, 0, para);
  if (ret!=0){ ret_code = ret-240; goto end; };
  Fql_matrix_clear(&s, para);

  // DER decode
  ret = der2pk(pk_seed, P3, pk, para);
  if (ret!=0){ ret_code = ret-380; goto end; };
  ret = der2sigma(r, &s, sig, para);
  if (ret!=0){ ret_code = ret-450; goto end; };

  ret = verify_op2(m, mlen, pk_seed, P3, r, &s, para);
  if (ret!=0 && ret!=1){
    ret_code = ret-510; goto end;
  }else{
    ret_code = ret;
  }

end:
  // free
  Fql_matrix_clear(&s, para);
  Fql_matrix_free(&s);
  for (int i=0; i<para->m; i++){
    Fql_matrix_free(&(P3[i]));
  }
  string_free(&pk_seed);
  string_free(&r);

  return ret_code;
}

int QRUOV_keygen(unsigned char* pk, size_t* pklen, unsigned char* sk,
                 size_t* sklen, QRUOV_params* para){

  int ret_code = 0;

  if (para->parameter == 1 || para->parameter == 5){
    ret_code = QRUOV_keygen_op(pk, pklen, sk, sklen, para);
  }else if (para->parameter == 2){
    ret_code = QRUOV_keygen_op2(pk, pklen, sk, sklen, para);
  }else{
    ret_code = QRUOV_keygen_gn(pk, pklen, sk, sklen, para);
  }

  return ret_code;
}

int QRUOV_sign(unsigned char* sk, size_t sklen, unsigned char* sig,
               size_t* siglen, unsigned char* m, size_t mlen,
               QRUOV_params* para){

  int ret_code = 0;

  if (para->parameter == 1 || para->parameter == 5){
    ret_code = QRUOV_sign_op(sk, sklen, sig, siglen, m, mlen, para);
  }else if (para->parameter == 2){
    ret_code = QRUOV_sign_op2(sk, sklen, sig, siglen, m, mlen, para);
  }else{
    ret_code = QRUOV_sign_gn(sk, sklen, sig, siglen, m, mlen, para);
  }

  return ret_code;
}

int QRUOV_verify(unsigned char* pk, size_t pklen, unsigned char* sig,
                 size_t siglen, unsigned char* m, size_t mlen,
                 QRUOV_params* para){

  int ret_code = 0;

  if (para->parameter == 1 || para->parameter == 5){
    ret_code = QRUOV_verify_op(pk, pklen, sig, siglen, m, mlen, para);
  }else if (para->parameter == 2){
    ret_code = QRUOV_verify_op2(pk, pklen, sig, siglen, m, mlen, para);
  }else{
    ret_code = QRUOV_verify_gn(pk, pklen, sig, siglen, m, mlen, para);
  }

  return ret_code;
}
