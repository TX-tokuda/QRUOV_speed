/**
 * @file  der.h
 * @brief QR-UOVの公開鍵、秘密鍵、署名のDERエンコード/デコードを定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef DER_H_INCLUDED
#define DER_H_INCLUDED

#ifdef DEBUG
  #include <stdio.h>
#endif
#include <string.h>

#include "parameter.h"
#include "matrix.h"

/**
 * lengthをDERエンコードする関数。
 *
 * @param[in/out] der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @param[in]     length 符号なし整数。SEQUENCEあるいはOCTET_STRINGのlengthを表す。
 * @return               0:成功。-1以下:エラー。
 */
int length2der(unsigned char* der, const unsigned int derlen, unsigned int* index,
               unsigned int length);

/**
 * lengthをDERデコードする関数。
 *
 * @param[out]    length 符号なし整数。SEQUENCEあるいはOCTET_STRINGのlengthを表す。
 * @param[in]     der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @return               0:成功。-1以下:エラー。
 */
int der2length(unsigned int* length, const unsigned char* der, const unsigned int derlen,
               unsigned int* index);

/**
 * 文字列をOCTET STRINGにDERエンコードする関数。
 *
 * @param[in/out] der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @param[in]     str    文字列。
 * @param[in]     strlen strのサイズ(Byte)。
 * @return               0:成功。-1以下:エラー。
 */
int str2octet_string(unsigned char* der, const unsigned int derlen, unsigned int* index,
                     const unsigned char* str, const unsigned int strlen);

/**
 * OCTET STRINGを文字列にDERデコードする関数。
 *
 * @param[out]    str    文字列。
 * @param[in]     strlen strのサイズ(Byte)。
 * @param[in]     der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @return               0:成功。-1以下:エラー。
 */
int octet_string2str(unsigned char* str, const unsigned int strlen, const unsigned char* der,
                     const unsigned int derlen, unsigned int* index);

/**
 * Fqlの行列をOCTET STRINGにDERエンコードする関数。
 *
 * @param[in/out] der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @param[in]     mat    Fqlの行列。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int Fql_mat2octet_string(unsigned char*der, const unsigned int derlen, unsigned int* index,
                         const FQL_MATRIX* mat, const QRUOV_params* para);

/**
 * Fqlの行列をOCTET STRINGにDERエンコードする関数のq=127における最適化関数。
 *
 * @param[in/out] der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @param[in]     mat    Fqlの行列。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int Fql_mat2octet_string_op(unsigned char*der, const unsigned int derlen, unsigned int* index,
                            const FQL_MATRIX_OP* mat, const QRUOV_params* para);

/**
 * OCTET STRINGをFqlの行列にDERデコードする関数。
 *
 * @param[out]    mat    Fqlの行列。
 * @param[in]     der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int octet_string2Fql_mat(FQL_MATRIX* mat, const unsigned char* der, const unsigned int derlen,
                         unsigned int* index, const QRUOV_params* para);

/**
 * OCTET STRINGをFqlの行列にDERデコードする関数のq=127における最適化関数。
 *
 * @param[out]    mat    Fqlの行列。
 * @param[in]     der    DER形式の文字列。
 * @param[in]     derlen derのサイズ(Byte)。
 * @param[in/out] index  derのインデックス。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int octet_string2Fql_mat_op(FQL_MATRIX_OP* mat, const unsigned char* der,
                            const unsigned int derlen, unsigned int* index,
                            const QRUOV_params* para);

/**
 * QR-UOVの公開鍵をDERエンコードする関数。
 *
 * @param[out] der     DER形式の文字列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int pk2der(unsigned char* der, const unsigned char* pk_seed, const FQL_MATRIX* P3,
           const QRUOV_params* para);

/**
 * QR-UOVの公開鍵をDERエンコードする関数のq=127における最適化関数。
 *
 * @param[out] der     DER形式の文字列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int pk2der_op(unsigned char* der, const unsigned char* pk_seed, const FQL_MATRIX_OP* P3,
              const QRUOV_params* para);

/**
 * QR-UOVの公開鍵をDERデコードする関数。
 *
 * @param[out] pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  der     DER形式の文字列。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int der2pk(unsigned char* pk_seed, FQL_MATRIX* P3, const unsigned char* der,
           const QRUOV_params* para);

/**
 * QR-UOVの公開鍵をDERデコードする関数のq=127における最適化関数。
 *
 * @param[out] pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  der     DER形式の文字列。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int der2pk_op(unsigned char* pk_seed, FQL_MATRIX_OP* P3, const unsigned char* der,
              const QRUOV_params* para);

/**
 * QR-UOVの秘密鍵をDERエンコードする関数。
 *
 * @param[out] der     DER形式の文字列。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int sk2der(unsigned char* der, const unsigned char* sk_seed, const unsigned char* pk_seed,
           const QRUOV_params* para);

/**
 * QR-UOVの秘密鍵をDERデコードする関数。
 *
 * @param[out] sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[out] pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  der     DER形式の文字列。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int der2sk(unsigned char* sk_seed, unsigned char* pk_seed, const unsigned char* der,
           const QRUOV_params* para);

/**
 * QR-UOVの署名をDERエンコードする関数。
 *
 * @param[out] der  DER形式の文字列。
 * @param[in]  r    ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in]  s    Fqlのpara->N*1行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int sigma2der(unsigned char* der, const unsigned char* r, const FQL_MATRIX* s,
              const QRUOV_params* para);

/**
 * QR-UOVの署名をDERエンコードする関数のq=127における最適化関数。
 *
 * @param[out] der  DER形式の文字列。
 * @param[in]  r    ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in]  s    Fqlのpara->N*1行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int sigma2der_op(unsigned char* der, const unsigned char* r, const FQL_MATRIX_OP* s,
                 const QRUOV_params* para);

/**
 * QR-UOVの署名をDERデコードする関数。
 *
 * @param[out] r    ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[out] s    Fqlのpara->N*1行列。
 * @param[in]  der  DER形式の文字列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int der2sigma(unsigned char* r, FQL_MATRIX* s, const unsigned char* der, const QRUOV_params* para);

/**
 * QR-UOVの署名をDERデコードする関数のq=127における最適化関数。
 *
 * @param[out] r    ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[out] s    Fqlのpara->N*1行列。
 * @param[in]  der  DER形式の文字列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int der2sigma_op(unsigned char* r, FQL_MATRIX_OP* s, const unsigned char* der,
                 const QRUOV_params* para);

#endif /* DER_H_INCLUDED */
