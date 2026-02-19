/**
 * @file  random.h
 * @brief 乱数生成に関連する関数を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#ifdef DEBUG
  #include <stdio.h>
#endif
#include <string.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#include "parameter.h"

/**
 * ランダムbit列を生成する関数。
 *
 * @param[out] r    ランダムなbit列。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in]  rlen 生成するbit列のサイズ(Byte)。
 * @return          0:成功。-1以下:エラー。
 */
int byte_random(unsigned char* r, const int rlen);

/**
 * プライベートなランダムbit列を生成する関数。
 *
 * @param[out] r    ランダムなbit列。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in]  rlen 生成するbit列のサイズ(Byte)。
 * @return          0:成功。-1以下:エラー。
 */
int byte_random_private(unsigned char* r, const int rlen);

/**
 * 整数をbit列に変換する関数。
 *
 * @param[out] out    整数を変換したbit列。関数の実行前にメモリを確保しておくこと。
 * @param[in]  i      整数。
 * @param[in]  bitlen 変換するbit列のサイズ(bit)。8の倍数を指定すること。
 */
void integer_to_bits(unsigned char* out, int i, int bitlen);

/**
 * SHAKEのためのコンテキスト。
 */
typedef struct SHAKE_ctx{
  EVP_MD_CTX* mdctx;
}SHAKE_CTX;

/**
 * SHAKEの初期化を行う関数。
 *
 * @param[in/out] ctx   SHAKEのためのコンテキスト。
 * @param[in]     param SHAKEのパラメータ選択。128:SHAKE128を用いる。256:SHAKE256を用いる。
 * @return              0:成功。-1以下:エラー。
 */
int SHAKE_init(SHAKE_CTX** ctx, int param);

/**
 * SHAKEのコンテキストの解放を行う関数。
 *
 * @param[in/out] ctx SHAKEのためのコンテキスト。関数内でctx->mdctxが解放される。
 */
void SHAKE_free(SHAKE_CTX** ctx);

/**
 * SHAKEのコンテキストのコピーを行う関数。
 *
 * @param[out] out コピー先のSHAKEのコンテキスト。
 * @param[in]  in  コピー元のSHAKEのコンテキスト。
 * @return         0:成功。-1以下:エラー。
 */
int SHAKE_CTX_copy(SHAKE_CTX* out, SHAKE_CTX* in);

/**
 * SHAKEに入力を追加する関数。
 *
 * @param[in/out] ctx  SHAKEのためのコンテキスト。
 * @param[in]     m    SHAKEに入力する文字列。
 * @param[in]     mlen mのサイズ(Byte)。
 * @return             0:成功。-1以下:エラー。
 */
int SHAKE_update(SHAKE_CTX* ctx, const unsigned char* m, const int mlen);

/**
 * SHAKEを用いてハッシュ値を生成する関数。
 *
 * @param[in/out] ctx  SHAKEのためのコンテキスト。
 * @param[out]    d    ハッシュ値。
 * @param[in]     dlen ハッシュサイズ(Byte)。
 * @return             0:成功。-1以下:エラー。
 */
int SHAKE_gen(SHAKE_CTX* ctx, unsigned char* d, int dlen);

/**
 * SHAKEを用いて擬似乱数生成を行う関数。
 *
 * @param[in/out] ctx    SHAKEのためのコンテキスト。
 * @param[out]    out    生成した擬似乱数列。
 * @param[in]     i      整数。関数内で文字列に変換され、SHAKEの入力に追加される。
 * @param[in]     bitlen 擬似乱数列のサイズ(bit)。8の倍数を指定すること。
 * @return               0:成功。-1以下:エラー。
 */
int PRG_SHAKE(SHAKE_CTX* ctx, unsigned char* out, int i, uint64_t bitlen);

/**
 * AESのためのコンテキスト。
 */
typedef struct AES_ctx{
  EVP_CIPHER_CTX* cctx;
}AES_CTX;

/**
 * AESの初期化を行う関数。
 *
 * @param[in/out] ctx AESのためのコンテキスト。
 * @return            0:成功。-1以下:エラー。
 */
int AES_init(AES_CTX** ctx);

/**
 * AESのコンテキストの解放を行う関数。
 *
 * @param[in/out] ctx AESのためのコンテキスト。関数内でctx->cctxが解放される。
 */
void AES_free(AES_CTX** ctx);

/**
 * AESのコンテキストのコピーを行う関数。
 *
 * @param[out] out コピー先のAESのコンテキスト。
 * @param[in]  in  コピー元のAESのコンテキスト。
 * @return         0:成功。-1以下:エラー。
 */
int AES_CTX_copy(AES_CTX* out, AES_CTX* in);

/**
 * AESの鍵と初期化ベクトルを設定する関数。
 *
 * @param[in/out] ctx   AESのためのコンテキスト。
 * @param[in]     key   AESの鍵。サイズはpara->AES_len Byte。AESの初期化ベクトルには0が設定される。
 * @param[in]     param AESのパラメータ選択。
 *                      128:AES128CTRを用いる。192:AES192CTRを用いる。256:AES256CTRを用いる。
 * @return              0:成功。-1以下:エラー。
 */
int AES_update(AES_CTX* ctx, const unsigned char* key, const int param);

/**
 * AESの暗号化を行う関数。
 *
 * @param[in/out] ctx    AESのためのコンテキスト。
 * @param[out]    outbuf AESの暗号文。サイズは128bit。
 * @param[in]     intext AESの平文。サイズは128bit。
 * @return               0:成功。-1以下:エラー。
 */
int AES_gen(AES_CTX* ctx, unsigned char* outbuf, const unsigned char* intext);

/**
 * AESを用いて擬似乱数生成を行う関数。
 *
 * @param[in/out] ctx    AESのためのコンテキスト。
 * @param[out]    out    生成した擬似乱数列。
 * @param[in]     i      整数。関数内で文字列に変換され、AESの平文に追加される。
 * @param[in]     bitlen 擬似乱数列のサイズ(bit)。8の倍数を指定すること。
 * @return               0:成功。-1以下:エラー。
 */
int PRG_AES(AES_CTX* ctx, unsigned char* out, int i, uint64_t bitlen);

#endif /* RANDOM_H_INCLUDED */
