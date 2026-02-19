/**
 * @file  der.c
 * @brief QR-UOVの公開鍵、秘密鍵、署名のDERエンコード/デコードを実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include "der.h"

int length2der(unsigned char* der, const unsigned int derlen, unsigned int* index,
               unsigned int length){

  if (length < 128){
    if ((*index)+1 >= derlen+1) goto error;
    der[(*index)++] = (unsigned char)(length & 0xFF);

  }else{
    uint8_t len_of_length = 0;
    unsigned int tmp = length;

    // len_of_length(lengthが何Byteで表せるか)を求める
    // len_of_lengthが8bit以上になる場合は考慮していない(QR-UOVでは起こり得ない)
    while (tmp>0){
      len_of_length++;
      tmp >>= 8;
    }

    // len_of_lengthをderに格納
    if ((*index)+1 >= derlen+1) goto error;
    der[(*index)++] = (unsigned char)((len_of_length & 0xFF) | 0x80);

    // lengthをbig-endianでderに格納
    if ((*index)+len_of_length >= derlen+1) goto error;
    for (int i=((int)len_of_length-1); i>=0; i--){
      der[(*index)+i] = (unsigned char)(length & 0xFF);
      length >>= 8;
    }
    *index += len_of_length;

  }

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[length2der] no space to write in der.\n");
#endif
  return -1;
}

int der2length(unsigned int* length, const unsigned char* der, const unsigned int derlen,
               unsigned int* index){

  // der[*index]の最上位bitが0のとき
  if (((der[*index]>>7)&1)==0){
    if ((*index)+1 >= derlen+1) goto error;
    *length = (unsigned int)der[(*index)++];

  }else{
    // len_of_length(lengthが何Byteで表せるか)を読み込む
    if ((*index)+1 >= derlen+1) goto error;
    uint8_t len_of_length = (uint8_t)(der[(*index)++] ^ 0x80);
    *length = 0;

    // derからbig-endianでlengthを読み込む
    if ((*index)+len_of_length >= derlen+1) goto error;
    for (int i=0; i<(int)len_of_length; i++){
      unsigned char tmp = der[(*index)++];
      *length <<= 8;
      *length += (unsigned int)tmp;
    }

  }

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[der2length] no data to read in der.\n");
#endif
  return -1;
}

int str2octet_string(unsigned char* der, const unsigned int derlen, unsigned int* index,
                     const unsigned char* str, const unsigned int strlen){

  int ret;

  if ((*index)+1 >= derlen+1) goto error;
  der[(*index)++] = 0x04; 

  ret = length2der(der, derlen, index, strlen);
  if (ret!=0) return ret-10;

  if ((*index)+strlen >= derlen+1) goto error;
  memcpy(der+(*index), str, strlen);
  *index += strlen;
  
  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[str2octet_string] no space to write in der.\n");
#endif
  return -1;
}

int octet_string2str(unsigned char* str, const unsigned int strlen, const unsigned char* der,
                     const unsigned int derlen, unsigned int* index){

  int ret;
  unsigned int len;

  if ((*index)+1 >= derlen+1) goto error;
  if (der[(*index)++] != 0x04){
#ifdef DEBUG
    fprintf(stderr, "[octet_string2str] input must be OCTET STRING.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, derlen, index);
  if (ret!=0) return ret-10;

  if (len!=strlen){
#ifdef DEBUG
    fprintf(stderr, "[octet_string2str] OCTET STRING length must be == strlen.\n");
#endif
    return -2;
  }

  if ((*index)+strlen >= derlen+1) goto error;
  memcpy(str, der+(*index), strlen);
  *index += strlen;

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[octet_string2str] no data to read in der.\n");
#endif
  return -3;
}

int Fql_mat2octet_string(unsigned char*der, const unsigned int derlen, unsigned int* index,
                          const FQL_MATRIX* mat, const QRUOV_params* para){

  int ret;
  unsigned int matlen = (mat->size) * para->l;


  if ((*index)+1 >= derlen+1) goto error;
  der[(*index)++] = 0x04; 

  ret = length2der(der, derlen, index, matlen);
  if (ret!=0) return ret-10;

  if ((*index)+(mat->size)*para->l >= derlen+1) goto error;
  for (int i=0; i<(mat->size); i++){
    for (int j=0; j<para->l; j++){
      der[(*index)++] = (unsigned char)(mat->data[i][j]);
    }
  }

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[Fql_mat2octet_string] no space to write in der.\n");
#endif
  return -1;
}

int Fql_mat2octet_string_op(unsigned char*der, const unsigned int derlen, unsigned int* index,
                            const FQL_MATRIX_OP* mat, const QRUOV_params* para){

  int ret;
  unsigned int matlen = (mat->size) * para->l;


  if ((*index)+1 >= derlen+1) goto error;
  der[(*index)++] = 0x04; 

  ret = length2der(der, derlen, index, matlen);
  if (ret!=0) return ret-10;

  if ((*index)+(mat->size)*para->l >= derlen+1) goto error;
  for (int i=0; i<(mat->size); i++){
    for (int j=0; j<para->l; j++){
      der[(*index)++] = (unsigned char)(((mat->data[i])>>(j*16)) & 0x7F);
    }
  }

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[Fql_mat2octet_string_op] no space to write in der.\n");
#endif
  return -1;
}

int octet_string2Fql_mat(FQL_MATRIX* mat, const unsigned char* der, const unsigned int derlen,
                         unsigned int* index, const QRUOV_params* para){

  int ret;
  unsigned int len;
  unsigned int matlen = (mat->size) * para->l;

  if ((*index)+1 >= derlen+1) goto error;
  if (der[(*index)++] != 0x04){
#ifdef DEBUG
    fprintf(stderr, "[octet_string2Fql_mat] input must be OCTET STRING.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, derlen, index);
  if (ret!=0) return ret-10;

  if (len!=matlen){
#ifdef DEBUG
    fprintf(stderr, "[octet_string2Fql_mat] OCTET STRING length must be == matlen.\n");
#endif
    return -2;
  }

  if ((*index)+(mat->size)*para->l >= derlen+1) goto error;
  for (int i=0; i<(mat->size); i++){
    for (int j=0; j<para->l; j++){
      mat->data[i][j] = (Fq)(der[(*index)++]);
    }
  }

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[octet_string2Fql_mat] no data to read in der.\n");
#endif
  return -3;
}

int octet_string2Fql_mat_op(FQL_MATRIX_OP* mat, const unsigned char* der,
                            const unsigned int derlen, unsigned int* index,
                            const QRUOV_params* para){

  int ret;
  unsigned int len;
  unsigned int matlen = (mat->size) * para->l;

  if ((*index)+1 >= derlen+1) goto error;
  if (der[(*index)++] != 0x04){
#ifdef DEBUG
    fprintf(stderr, "[octet_string2Fql_mat_op] input must be OCTET STRING.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, derlen, index);
  if (ret!=0) return ret-10;

  if (len!=matlen){
#ifdef DEBUG
    fprintf(stderr, "[octet_string2Fql_mat_op] OCTET STRING length must be == matlen.\n");
#endif
    return -2;
  }

  if ((*index)+(mat->size)*para->l >= derlen+1) goto error;
  for (int i=0; i<(mat->size); i++){
    mat->data[i] = 0;
    for (int j=0; j<para->l; j++){
      mat->data[i] |= ((uint64_t)der[(*index)++])<<(j*16);
    }
  }

  return 0;

  error:
#ifdef DEBUG
  fprintf(stderr, "[octet_string2Fql_mat_op] no data to read in der.\n");
#endif
  return -3;
}

int pk2der(unsigned char* der, const unsigned char* pk_seed, const FQL_MATRIX* P3,
            const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  // Pi3(OCTET STRING)の長さは256Byte以上65536Byte未満
  unsigned int p3len = para->m*((P3[0].size)*para->l+4);
  unsigned int pklen;

  if (para->SL==1 || (para->parameter==7)){
    // P3(SEQUENCE)の長さが256Byte以上65536Byte未満
    pklen = (para->seed_len+2) + (p3len+4);
  }else{
    // P3(SEQUENCE)の長さが65536Byte以上
    pklen = (para->seed_len+2) + (p3len+5);
  }

  // SEQUENCE{seed_pk, P3}
  der[index++] = 0x30; 
  ret = length2der(der, para->pk_len, &index, pklen);
  if (ret!=0) return ret;

  // seed_pk
  ret = str2octet_string(der, para->pk_len, &index, pk_seed, para->seed_len);
  if (ret!=0) return ret-10;

  // P3
  der[index++] = 0x30; 
  ret = length2der(der, para->pk_len, &index, p3len);
  if (ret!=0) return ret-30;

  for (int i=0; i<para->m; i++){
    ret = Fql_mat2octet_string(der, para->pk_len, &index, &(P3[i]), para);
    if (ret!=0) return ret-40;
  }

  return 0;
}

int pk2der_op(unsigned char* der, const unsigned char* pk_seed, const FQL_MATRIX_OP* P3,
              const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  // Pi3(OCTET STRING)の長さは256Byte以上65536Byte未満
  unsigned int p3len = para->m*((P3[0].size)*para->l+4);
  unsigned int pklen;

  if (para->SL==1 || (para->parameter==7)){
    // P3(SEQUENCE)の長さが256Byte以上65536Byte未満
    pklen = (para->seed_len+2) + (p3len+4);
  }else{
    // P3(SEQUENCE)の長さが65536Byte以上
    pklen = (para->seed_len+2) + (p3len+5);
  }

  // SEQUENCE{seed_pk, P3}
  der[index++] = 0x30; 
  ret = length2der(der, para->pk_len, &index, pklen);
  if (ret!=0) return ret;

  // seed_pk
  ret = str2octet_string(der, para->pk_len, &index, pk_seed, para->seed_len);
  if (ret!=0) return ret-10;

  // P3
  der[index++] = 0x30; 
  ret = length2der(der, para->pk_len, &index, p3len);
  if (ret!=0) return ret-30;

  for (int i=0; i<para->m; i++){
    ret = Fql_mat2octet_string_op(der, para->pk_len, &index, &(P3[i]), para);
    if (ret!=0) return ret-40;
  }

  return 0;
}

int der2pk(unsigned char* pk_seed, FQL_MATRIX* P3, const unsigned char* der,
           const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int len;
  // Pi3(OCTET STRING)の長さは256Byte以上65536Byte未満
  unsigned int p3len = para->m*((P3[0].size)*para->l+4);
  unsigned int pklen;

  if (para->SL==1 || (para->parameter==7)){
    // P3(SEQUENCE)の長さが256Byte以上65536Byte未満
    pklen = (para->seed_len+2) + (p3len+4);
  }else{
    // P3(SEQUENCE)の長さが65536Byte以上
    pklen = (para->seed_len+2) + (p3len+5);
  }

  // SEQUENCE{seed_pk, P3}
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2pk] input must be SEQUENCE.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, para->pk_len, &index);
  if (ret!=0) return ret-10;

  if (len!=pklen){
#ifdef DEBUG
    fprintf(stderr, "[der2pk] SEQUENCE length must be == pklen.\n");
#endif
    return -2;
  }

  // seed_pk
  ret = octet_string2str(pk_seed, para->seed_len, der, para->pk_len, &index);
  if (ret!=0) return ret-20;

  // P3
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2pk] P3 input must be SEQUENCE.\n");
#endif
    return -3;
  }

  ret = der2length(&len, der, para->pk_len, &index);
  if (ret!=0) return ret-40;

  if (len!=p3len){
#ifdef DEBUG
    fprintf(stderr, "[der2pk] P3 SEQUENCE length must be == p3len.\n");
#endif
    return -4;
  }

  for (int i=0; i<para->m; i++){
    ret = octet_string2Fql_mat(&(P3[i]), der, para->pk_len, &index, para);
    if (ret!=0) return ret-50;
  }

  return 0;
}

int der2pk_op(unsigned char* pk_seed, FQL_MATRIX_OP* P3, const unsigned char* der,
              const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int len;
  // Pi3(OCTET STRING)の長さは256Byte以上65536Byte未満
  unsigned int p3len = para->m*((P3[0].size)*para->l+4);
  unsigned int pklen;

  if (para->SL==1 || (para->parameter==7)){
    // P3(SEQUENCE)の長さが256Byte以上65536Byte未満
    pklen = (para->seed_len+2) + (p3len+4);
  }else{
    // P3(SEQUENCE)の長さが65536Byte以上
    pklen = (para->seed_len+2) + (p3len+5);
  }

  // SEQUENCE{seed_pk, P3}
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2pk_op] input must be SEQUENCE.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, para->pk_len, &index);
  if (ret!=0) return ret-10;

  if (len!=pklen){
#ifdef DEBUG
    fprintf(stderr, "[der2pk_op] SEQUENCE length must be == pklen.\n");
#endif
    return -2;
  }

  // seed_pk
  ret = octet_string2str(pk_seed, para->seed_len, der, para->pk_len, &index);
  if (ret!=0) return ret-20;

  // P3
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2pk_op] P3 input must be SEQUENCE.\n");
#endif
    return -3;
  }

  ret = der2length(&len, der, para->pk_len, &index);
  if (ret!=0) return ret-40;

  if (len!=p3len){
#ifdef DEBUG
    fprintf(stderr, "[der2pk_op] P3 SEQUENCE length must be == p3len.\n");
#endif
    return -4;
  }

  for (int i=0; i<para->m; i++){
    ret = octet_string2Fql_mat_op(&(P3[i]), der, para->pk_len, &index, para);
    if (ret!=0) return ret-50;
  }

  return 0;
}

int sk2der(unsigned char* der, const unsigned char* sk_seed, const unsigned char* pk_seed,
            const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  // sk_seed(OCTET STRING), pk_seed(OCTET STRING)の長さはどちらも128Byte未満
  unsigned int sklen = 3+2*(para->seed_len+2);

  // SEQUENCE{version, sk_seed, pk_seed}
  der[index++] = 0x30; 
  ret = length2der(der, para->sk_len, &index, sklen);
  if (ret!=0) return ret;

  // version
  der[index++] = 0x02;
  ret = length2der(der, para->sk_len, &index, 1);
  if (ret!=0) return ret-10;
  der[index++] = 0x02;  // version 2

  // sk_seed
  ret = str2octet_string(der, para->sk_len, &index, sk_seed, para->seed_len);
  if (ret!=0) return ret-20;

  // pk_seed
  ret = str2octet_string(der, para->sk_len, &index, pk_seed, para->seed_len);
  if (ret!=0) return ret-40;

  return 0;
}

int der2sk(unsigned char* sk_seed, unsigned char* pk_seed, const unsigned char* der,
           const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int len;
  // sk_seed(OCTET STRING), pk_seed(OCTET STRING)の長さはどちらも128Byte未満
  unsigned int sklen = 3+2*(para->seed_len+2);

  // SEQUENCE{version, sk_seed, pk_seed}
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2sk] input must be SEQUENCE.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, para->sk_len, &index);
  if (ret!=0) return ret-10;

  if (len!=sklen){
#ifdef DEBUG
    fprintf(stderr, "[der2sk] SEQUENCE length must be == sklen.\n");
#endif
    return -2;
  }

  // version
  if (der[index++] != 0x02){
#ifdef DEBUG
    fprintf(stderr, "[der2sk] version input must be INTEGER.\n");
#endif
    return -3;
  }

  ret = der2length(&len, der, para->sk_len, &index);
  if (ret!=0) return ret-20;

  if (len!=1){
#ifdef DEBUG
    fprintf(stderr, "[der2sk] version INTEGER length must be 1.\n");
#endif
    return -4;
  }

  if (der[index++] != 0x02){
#ifdef DEBUG
    fprintf(stderr, "[der2sk] version must be 2.\n");
#endif
    return -5;
  }

  // sk_seed
  ret = octet_string2str(sk_seed, para->seed_len, der, para->sk_len, &index);
  if (ret!=0) return ret-30;

  // pk_seed
  ret = octet_string2str(pk_seed, para->seed_len, der, para->sk_len, &index);
  if (ret!=0) return ret-50;

  return 0;
}

int sigma2der(unsigned char* der, const unsigned char* r, const FQL_MATRIX* s,
               const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int slen = (s->size) * para->l;
  unsigned int sigmalen;

  if (para->parameter==1 || para->parameter==2){
    // s(OCTET STRING)の長さが128Byte以上256Byte未満
    sigmalen = (para->salt_len+2) + (slen+3);
  }else{
    // s(OCTET STRING)の長さが256Byte以上65536Byte未満
    sigmalen = (para->salt_len+2) + (slen+4);
  }

  // SEQUENCE{r, s}
  der[index++] = 0x30; 
  ret = length2der(der, para->sigma_len, &index, sigmalen);
  if (ret!=0) return ret;

  // r
  ret = str2octet_string(der, para->sigma_len, &index, r, para->salt_len);
  if (ret!=0) return ret-10;

  // s
  ret = Fql_mat2octet_string(der, para->sigma_len, &index, s, para);
  if (ret!=0) return ret-30;

  return 0;
}

int sigma2der_op(unsigned char* der, const unsigned char* r, const FQL_MATRIX_OP* s,
                 const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int slen = (s->size) * para->l;
  unsigned int sigmalen;

  if (para->parameter==1 || para->parameter==2){
    // s(OCTET STRING)の長さが128Byte以上256Byte未満
    sigmalen = (para->salt_len+2) + (slen+3);
  }else{
    // s(OCTET STRING)の長さが256Byte以上65536Byte未満
    sigmalen = (para->salt_len+2) + (slen+4);
  }

  // SEQUENCE{r, s}
  der[index++] = 0x30; 
  ret = length2der(der, para->sigma_len, &index, sigmalen);
  if (ret!=0) return ret;

  // r
  ret = str2octet_string(der, para->sigma_len, &index, r, para->salt_len);
  if (ret!=0) return ret-10;

  // s
  ret = Fql_mat2octet_string_op(der, para->sigma_len, &index, s, para);
  if (ret!=0) return ret-30;

  return 0;
}

int der2sigma(unsigned char* r, FQL_MATRIX* s, const unsigned char* der, const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int len;
  unsigned int slen = (s->size) * para->l;
  unsigned int sigmalen;

  if (para->parameter==1 || para->parameter==2){
    // s(OCTET STRING)の長さが128Byte以上256Byte未満
    sigmalen = (para->salt_len+2) + (slen+3);
  }else{
    // s(OCTET STRING)の長さが256Byte以上65536Byte未満
    sigmalen = (para->salt_len+2) + (slen+4);
  }

  // SEQUENCE{r, s}
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2sigma] input must be SEQUENCE.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, para->sigma_len, &index);
  if (ret!=0) return ret-10;

  if (len!=sigmalen){
#ifdef DEBUG
    fprintf(stderr, "[der2sigma] SEQUENCE length must be == sigmalen.\n");
#endif
    return -2;
  }

  // r
  ret = octet_string2str(r, para->salt_len, der, para->sigma_len, &index);
  if (ret!=0) return ret-20;

  // s
  ret = octet_string2Fql_mat(s, der, para->sigma_len, &index, para);
  if (ret!=0) return ret-40;

  return 0;
}

int der2sigma_op(unsigned char* r, FQL_MATRIX_OP* s, const unsigned char* der,
                 const QRUOV_params* para){

  int ret;
  unsigned int index = 0;
  unsigned int len;
  unsigned int slen = (s->size) * para->l;
  unsigned int sigmalen;

  if (para->parameter==1 || para->parameter==2){
    // s(OCTET STRING)の長さが128Byte以上256Byte未満
    sigmalen = (para->salt_len+2) + (slen+3);
  }else{
    // s(OCTET STRING)の長さが256Byte以上65536Byte未満
    sigmalen = (para->salt_len+2) + (slen+4);
  }

  // SEQUENCE{r, s}
  if (der[index++] != 0x30){
#ifdef DEBUG
    fprintf(stderr, "[der2sigma_op] input must be SEQUENCE.\n");
#endif
    return -1;
  }

  ret = der2length(&len, der, para->sigma_len, &index);
  if (ret!=0) return ret-10;

  if (len!=sigmalen){
#ifdef DEBUG
    fprintf(stderr, "[der2sigma_op] SEQUENCE length must be == sigmalen.\n");
#endif
    return -2;
  }

  // r
  ret = octet_string2str(r, para->salt_len, der, para->sigma_len, &index);
  if (ret!=0) return ret-20;

  // s
  ret = octet_string2Fql_mat_op(s, der, para->sigma_len, &index, para);
  if (ret!=0) return ret-40;

  return 0;
}
