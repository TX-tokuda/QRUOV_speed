/**
 * @file  random.c
 * @brief 乱数生成に関連する関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include "random.h"

int byte_random(unsigned char* r, const int rlen){
  if (r==NULL){
#ifdef DEBUG
    fprintf(stderr, "[byte_random] r must be != NULL.\n");
#endif
    return -1;
  }
  if (rlen<=0){
#ifdef DEBUG
    fprintf(stderr, "[byte_random] rlen must be > 0.\n");
#endif
    return -2;
  }

  RAND_bytes(r, rlen);

  return 0;
}

int byte_random_private(unsigned char* r, const int rlen){

  if (r==NULL){
#ifdef DEBUG
    fprintf(stderr, "[byte_random_private] r must be != NULL.\n");
#endif
    return -1;
  }
  if (rlen<=0){
#ifdef DEBUG
    fprintf(stderr, "[byte_random_private] rlen must be > 0.\n");
#endif
    return -2;
  }

  RAND_priv_bytes(r, rlen);

  return 0;
}

void integer_to_bits(unsigned char* out, int i, int bitlen){

  int bytelen = bitlen/8;

  // big-endian
  for(int j=bytelen-1; j>=0; j--){
    out[j] = i&0xFF;
    i>>=8;
  }

  return;
}

int SHAKE_init(SHAKE_CTX** ctx, const int param){

  int ret;

  if (*ctx!=NULL){
    free(*ctx);
    *ctx = NULL;
  }

  *ctx = (SHAKE_CTX*)malloc(sizeof(SHAKE_CTX));
  if (*ctx == NULL){
#ifdef DEBUG
    fprintf(stderr, "[SHAKE_init] malloc failed.\n");
#endif
    return -1;
  }

  (*ctx)->mdctx = EVP_MD_CTX_new();

  if(param==128){
    ret = EVP_DigestInit_ex2((*ctx)->mdctx, EVP_shake128(), NULL);
  }else if(param==256){
    ret = EVP_DigestInit_ex2((*ctx)->mdctx, EVP_shake256(), NULL);
  }else{
#ifdef DEBUG
    fprintf(stderr, "[SHAKE_init] param must be 128 or 256.\n");
#endif
    return -2;
  }

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[SHAKE_init] SHAKE init function failed.\n");
#endif
    return -3;
  }

  return 0;
}

void SHAKE_free(SHAKE_CTX** ctx){

  if (ctx!=NULL){
    if (*ctx!=NULL){
      EVP_MD_CTX_free((*ctx)->mdctx);
      free(*ctx);
      *ctx = NULL;
    }
  }

  return;
}

int SHAKE_CTX_copy(SHAKE_CTX* out, SHAKE_CTX* in) {

  out->mdctx = EVP_MD_CTX_new();

  int ret = EVP_MD_CTX_copy_ex(out->mdctx, in->mdctx);

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[SHAKE_CTX_copy] SHAKE_CTX copy function failed.\n");
#endif
    return -1;
  }

  return 0;
}

int SHAKE_update(SHAKE_CTX* ctx, const unsigned char* m, const int mlen){

  int ret = EVP_DigestUpdate(ctx->mdctx, m, mlen);

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[SHAKE_update] SHAKE update function failed.\n");
#endif
    return -1;
  }

  return 0;
}

int SHAKE_gen(SHAKE_CTX* ctx, unsigned char* d, int dlen){

  int ret = EVP_DigestFinalXOF(ctx->mdctx, d, dlen);

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[SHAKE_gen] SHAKE final function failed.\n");
#endif
    return -1;
  }

  return 0;
}

int PRG_SHAKE(SHAKE_CTX* ctx, unsigned char* out, int i, uint64_t bitlen){

  int ret;
  unsigned char x[2];
  int bytelen = bitlen/8;
  integer_to_bits(x, i, 16);

  ret = SHAKE_update(ctx, x, 2);
  if (ret!=0) return ret;

  ret = SHAKE_gen(ctx, out, bytelen);
  if (ret!=0) return ret-10;

  return 0;
}

int AES_init(AES_CTX** ctx){

  if (*ctx!=NULL){
    free(*ctx);
    *ctx = NULL;
  }

  *ctx = (AES_CTX*)malloc(sizeof(AES_CTX));
  if (*ctx == NULL){
#ifdef DEBUG
    fprintf(stderr, "[AES_init] malloc failed.\n");
#endif
    return -1;
  }

  (*ctx)->cctx = EVP_CIPHER_CTX_new();

  return 0;
}

void AES_free(AES_CTX** ctx){

  if (ctx!=NULL){
    if (*ctx!=NULL){
      EVP_CIPHER_CTX_free((*ctx)->cctx);
      free(*ctx);
      *ctx = NULL;
    }
  }

  return;
}

int AES_CTX_copy(AES_CTX* out, AES_CTX* in) {

  out->cctx = EVP_CIPHER_CTX_new();

  int ret = EVP_CIPHER_CTX_copy(out->cctx, in->cctx);

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[AES_CTX_copy] AES_CTX copy function failed.\n");
#endif
    return -1;
  }

  return 0;
}

int AES_update(AES_CTX* ctx, const unsigned char* key, const int param){

  EVP_CIPHER* cipher = NULL;

  if(param==128){
    cipher = EVP_CIPHER_fetch(NULL, "AES-128-CTR", NULL);
  }else if(param==192){
    cipher = EVP_CIPHER_fetch(NULL, "AES-192-CTR", NULL);
  }else if(param==256){
    cipher = EVP_CIPHER_fetch(NULL, "AES-256-CTR", NULL);
  }else{
#ifdef DEBUG
    fprintf(stderr, "[AES_update] param must be 128 or 192 or 256.\n");
#endif
    return -1;
  }

  if (cipher==NULL){
#ifdef DEBUG
    fprintf(stderr, "[AES_update] AES fetch function failed.\n");
#endif
    return -2;
  }

  unsigned char iv[16];  // nonce(4Byte)+iv(8Byte)+ctr(4Byte) AES128/192/256共通
  memset(iv, 0, 16);

  int ret = EVP_EncryptInit_ex2(ctx->cctx, cipher, key, iv, NULL);

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[AES_update] AES init function failed.\n");
#endif
    EVP_CIPHER_free(cipher);
    return -3;
  }

  EVP_CIPHER_free(cipher);

  return 0;
}

int AES_gen(AES_CTX* ctx, unsigned char* outbuf, const unsigned char* intext){

  int outlen;

  int ret = EVP_EncryptUpdate(ctx->cctx, outbuf, &outlen, intext, 16);
                                                              // intext, outbufは128bit固定
  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[AES_gen] AES encrypt update function failed.\n");
#endif
    return -1;
  }

  return 0;
}

int PRG_AES(AES_CTX* ctx, unsigned char* out, int i, uint64_t bitlen){

  unsigned char intext[16];  // AES-CTR plaintext(16Byte)
  memset(intext, 0, 16);

  unsigned char x[16];  // AES-CTR nonce(4Byte)+iv(8Byte)+ctr(4Byte) AES128/192/256共通
  integer_to_bits(x, i, 64);
  memset(x+8, 0, 8);

  int ret = EVP_EncryptInit_ex2(ctx->cctx, NULL, NULL, x, NULL);

  if (ret!=1){
#ifdef DEBUG
    fprintf(stderr, "[PRG_AES] AES init function failed.\n");
#endif
    return -1;
  }

  int bytelen = bitlen/8;
  unsigned char b[bytelen + 16];
  uint64_t loopnum;
  if (bytelen%16==0) loopnum = bytelen/16;
  else loopnum = bytelen/16 + 1;


  for(uint64_t j=0; j<loopnum; j++){
    int ret = AES_gen(ctx, b+j*16, intext);
    if (ret!=0) return ret-10;
  }

  for(int j=0; j<bytelen; j++){
    out[j] = b[j];
  }

  // clear
  memset(b, 0, bytelen+16);

  return 0;
}
