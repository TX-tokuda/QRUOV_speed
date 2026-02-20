/**
 * @file  qruov_signature.h
 * @brief QR-UOVの鍵生成、署名生成、署名検証を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef QRUOV_SIGNATURE_H_INCLUDED
#define QRUOV_SIGNATURE_H_INCLUDED

#include "qruov_matrix.h"
#include "expand.h"

/**
 * stringを初期化する関数。
 *
 * @param[out] string unsigned charの配列。関数内でメモリが確保される。
 * @param[in]  len    配列の長さ。
 * @return            0:成功。-1以下:エラー。
 */
int string_init(unsigned char** string, const int len);

/**
 * stringを解放する関数。
 *
 * @param[in/out] string 任意のunsigned charの配列。関数内でメモリが解放される。
 */
void string_free(unsigned char** string);

/**
 * SdTを生成する関数。
 *
 * @param[out] SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_SdT(FQL_MATRIX* SdT, const unsigned char* sk_seed, QRUOV_params* para);

/**
 * SdTを生成する関数のq=127における最適化関数。
 *
 * @param[out] SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_SdT_op(FQL_MATRIX_OP* SdT, const unsigned char* sk_seed, QRUOV_params* para);

/**
 * P1, P2T, P3を生成する関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_P3(FQL_MATRIX* P1, FQL_MATRIX* P2T, FQL_MATRIX* P3,
                     FQL_MATRIX* SdT, const unsigned char * pk_seed, QRUOV_params* para);

/**
 * P1, P2T, P3を生成する関数のq=127における最適化関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_P3_op(FQL_MATRIX_OP* P1, FQL_MATRIX_OP* P2T, FQL_MATRIX_OP* P3,
                         FQL_MATRIX_OP* SdT, const unsigned char * pk_seed, QRUOV_params* para);

/**
 * P1, P2T, P3を生成する関数のparameter==2における最適化関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_P3_op2(FQL_MATRIX* P1, FQL_MATRIX* P2T, FQL_MATRIX* P3,
                     FQL_MATRIX* SdT, const unsigned char * pk_seed, QRUOV_params* para);

/**
 * yを生成する関数。
 *
 * @param[out] y      Fqlのpara->V*1行列。
 * @param[in]  y_seed yのシード。サイズはpara->seed_len-Byte。
 * @param[in]  para   パラメータの構造体。
 * @return            0:成功。-1以下:エラー。
 */
int compute_y(FQL_MATRIX* y, const unsigned char* y_seed, QRUOV_params* para);

/**
 * yを生成する関数のq=127における最適化関数。
 *
 * @param[out] y      Fqlのpara->V*1行列。
 * @param[in]  y_seed yのシード。サイズはpara->seed_len-Byte。
 * @param[in]  para   パラメータの構造体。
 * @return            0:成功。-1以下:エラー。
 */
int compute_y_op(FQL_MATRIX_OP* y, const unsigned char* y_seed, QRUOV_params* para);

/**
 * P1, P2T, L, uを生成する関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[out] L       Fqのpara->m*para->m行列。
 * @param[out] u       Fqのpara->m*1行列。
 * @param[in]  SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  y       Fqlのpara->V*1行列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_L_u(FQL_MATRIX* P1, FQL_MATRIX* P2T, FQ_MATRIX* L, FQ_MATRIX* u,
                       const FQL_MATRIX* SdT, const FQL_MATRIX* y, const unsigned char* pk_seed,
                       QRUOV_params* para);

/**
 * P1, P2T, L, uを生成する関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[out] L       Fqのpara->m*para->m行列。
 * @param[out] u       Fqのpara->m*1行列。
 * @param[in]  SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  y       Fqlのpara->V*1行列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_L_u_op2(FQL_MATRIX* P1, FQL_MATRIX* P2T, FQ_MATRIX* L, FQ_MATRIX* u,
                       const FQL_MATRIX* SdT, const FQL_MATRIX* y, const unsigned char* pk_seed,
                       QRUOV_params* para);

/**
 * P1, P2T, L, uを生成する関数のq=127における最適化関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[out] L       Fqのpara->m*para->m行列。
 * @param[out] u       Fqのpara->m*1行列。
 * @param[in]  SdT     Fqlのpara->V*para->M行列の転置行列。
 * @param[in]  y       Fqlのpara->V*1行列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_L_u_op(FQL_MATRIX_OP* P1, FQL_MATRIX_OP* P2T, FQ_MATRIX* L, FQ_MATRIX* u,
                          const FQL_MATRIX_OP* SdT, const FQL_MATRIX_OP* y,
                          const unsigned char* pk_seed, QRUOV_params* para);

/**
 * SHAKE256を用いてメッセージからmuを生成する関数。
 *
 * @param[out] mu      文字列。サイズは64Byte。
 * @param[in]  M       メッセージ。
 * @param[in]  Mlen    メッセージのサイズ(Byte)。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_mu(unsigned char* mu, const unsigned char* M, const unsigned long long Mlen,
               const unsigned char* pk_seed, const QRUOV_params* para);

/**
 * rを生成するためのコンテキストを初期化する関数。
 *
 * @param[in/out] r_ctx  rを生成するためのコンテキスト。
 * @param[in]     r_seed rのシード。サイズはpara->seed_len-Byte。
 * @param[in]     para   パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int r_ctx_init(void** r_ctx, const unsigned char* r_seed, const QRUOV_params* para);

/**
 * rを生成する関数。
 *
 * @param[in/out] r_ctx rを生成するためのコンテキスト。
 * @param[in]     r     ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in]     para  パラメータの構造体。
 * @return        0:成功。-1以下:エラー。
 */
int compute_r(void* r_ctx, unsigned char* r, const QRUOV_params* para);

/**
 * P1, P2Tを生成する関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T(FQL_MATRIX* P1, FQL_MATRIX* P2T, const unsigned char * pk_seed,
                   QRUOV_params* para);

/**
 * P1, P2Tを生成する関数のq=127における最適化関数。
 *
 * @param[out] P1      Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[out] P2T     Fqlのpara->V*para->M行列の転置行列のpara->m個の配列。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int compute_P1_P2T_op(FQL_MATRIX_OP* P1, FQL_MATRIX_OP* P2T, const unsigned char * pk_seed,
                      QRUOV_params* para);

/**
 * QR-UOVの鍵生成を行う関数。
 *
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int keygen(FQL_MATRIX* P3, const unsigned char* sk_seed, const unsigned char* pk_seed,
           QRUOV_params* para);

/**
 * QR-UOVの鍵生成を行う関数のq=127における最適化関数。
 *
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int keygen_op(FQL_MATRIX_OP* P3, const unsigned char* sk_seed, const unsigned char* pk_seed,
              QRUOV_params* para);

/**
 * QR-UOVの鍵生成を行う関数のparameter = 2における最適化関数。
 *
 * @param[out] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int keygen_op2(FQL_MATRIX* P3, const unsigned char* sk_seed, const unsigned char* pk_seed,
           QRUOV_params* para);

/**
 * QR-UOVの署名生成を行う関数。
 *
 * @param[out] r       ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[out] s       Fqlのpara->N*1行列。
 * @param[in]  M       メッセージ。
 * @param[in]  Mlen    メッセージのサイズ(Byte)。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  r_seed  rのシード。サイズはpara->seed_len-Byte。
 * @param[in]  y_seed  yのシード。サイズはpara->seed_len-Byte。
 * @param[in]  x_seed  xのシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int sign(unsigned char* r, FQL_MATRIX* s, const unsigned char* M, const unsigned long long Mlen,
         const unsigned char* sk_seed, const unsigned char* pk_seed, const unsigned char* r_seed,
         const unsigned char* y_seed, const unsigned char* x_seed, QRUOV_params* para);

/**
 * QR-UOVの署名生成を行う関数のq=127における最適化関数。
 *
 * @param[out] r       ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[out] s       Fqlのpara->N*1行列。
 * @param[in]  M       メッセージ。
 * @param[in]  Mlen    メッセージのサイズ(Byte)。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  r_seed  rのシード。サイズはpara->seed_len-Byte。
 * @param[in]  y_seed  yのシード。サイズはpara->seed_len-Byte。
 * @param[in]  x_seed  xのシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int sign_op(unsigned char* r, FQL_MATRIX_OP* s, const unsigned char* M,
            const unsigned long long Mlen, const unsigned char* sk_seed,
            const unsigned char* pk_seed, const unsigned char* r_seed,
            const unsigned char* y_seed, const unsigned char* x_seed, QRUOV_params* para);

/**
 * QR-UOVの署名生成を行う関数のparameter = 2における最適化関数。
 *
 * @param[out] r       ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[out] s       Fqlのpara->N*1行列。
 * @param[in]  M       メッセージ。
 * @param[in]  Mlen    メッセージのサイズ(Byte)。
 * @param[in]  sk_seed 秘密鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in]  r_seed  rのシード。サイズはpara->seed_len-Byte。
 * @param[in]  y_seed  yのシード。サイズはpara->seed_len-Byte。
 * @param[in]  x_seed  xのシード。サイズはpara->seed_len-Byte。
 * @param[in]  para    パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int sign_op2(unsigned char* r, FQL_MATRIX* s, const unsigned char* M, const unsigned long long Mlen,
         const unsigned char* sk_seed, const unsigned char* pk_seed, const unsigned char* r_seed,
         const unsigned char* y_seed, const unsigned char* x_seed, QRUOV_params* para);
         
/**
 * QR-UOVの署名検証を行う関数。
 *
 * @param[in] M       メッセージ。
 * @param[in] Mlen    メッセージのサイズ(Byte)。
 * @param[in] pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in] r       ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in] s       Fqlのpara->N*1行列。
 * @param[in] para    パラメータの構造体。
 * @return            0:署名を棄却。1:署名を受理。-1以下:エラー。
 */
int verify(const unsigned char* M, const unsigned long long Mlen, const unsigned char* pk_seed,
           const FQL_MATRIX* P3, const unsigned char* r, const FQL_MATRIX* s, QRUOV_params* para);

/**
 * QR-UOVの署名検証を行う関数のq=127における最適化関数。
 *
 * @param[in] M       メッセージ。
 * @param[in] Mlen    メッセージのサイズ(Byte)。
 * @param[in] pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in] r       ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in] s       Fqlのpara->N*1行列。
 * @param[in] para    パラメータの構造体。
 * @return            0:署名を棄却。1:署名を受理。-1以下:エラー。
 */
int verify_op(const unsigned char* M, const unsigned long long Mlen, const unsigned char* pk_seed,
              const FQL_MATRIX_OP* P3, const unsigned char* r, const FQL_MATRIX_OP* s,
              QRUOV_params* para);

/**
 * QR-UOVの署名検証を行う関数のparameter-"における最適化関数。
 *
 * @param[in] M       メッセージ。
 * @param[in] Mlen    メッセージのサイズ(Byte)。
 * @param[in] pk_seed 公開鍵のシード。サイズはpara->seed_len-Byte。
 * @param[in] P3      Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in] r       ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in] s       Fqlのpara->N*1行列。
 * @param[in] para    パラメータの構造体。
 * @return            0:署名を棄却。1:署名を受理。-1以下:エラー。
 */
int verify_op2(const unsigned char* M, const unsigned long long Mlen, const unsigned char* pk_seed,
           const FQL_MATRIX* P3, const unsigned char* r, const FQL_MATRIX* s, QRUOV_params* para);
#endif /* QRUOV_SIGNATURE_H_INCLUDED */
