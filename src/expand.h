/**
 * @file  expand.h
 * @brief Expand関数とその関連関数を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef EXPAND_H_INCLUDED
#define EXPAND_H_INCLUDED

#include "matrix.h"

/**
 * 棄却サンプリングを行う関数。
 *
 * @param[out] vec  棄却サンプリングによって生成されるFqの列。
 * @param[in]  str  棄却サンプリングの入力文字列。
 * @param[in]  tau  strのサイズ(Byte)。
 * @param[in]  nd   vecのサイズ。
 * @param[in]  para パラメータの構造体。
 *
 */
void rej_samp(Fq* vec, const unsigned char* str, const int tau, const int nd,
              const QRUOV_params* para);

/**
 * SHAKEを用いて擬似乱数生成と棄却サンプリングを行う関数。
 *
 * @param[in/out] ctx  SHAKEのためのコンテキスト。
 * @param[out]    vec  棄却サンプリングによって生成されるFqの列。
 * @param[in]     i    整数。関数内で文字列に変換され、SHAKEの入力に追加される。
 * @param[in]     tau  棄却サンプリングの入力文字列のサイズ(Byte)。
 * @param[in]     nd   vecのサイズ。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int rej_samp_PRG_SHAKE(SHAKE_CTX* ctx, Fq* vec, const int i, const int tau, const int nd,
                       const QRUOV_params* para);

/**
 * AESを用いて擬似乱数生成と棄却サンプリングを行う関数。
 *
 * @param[in/out] ctx  AESのためのコンテキスト。
 * @param[out]    vec  棄却サンプリングによって生成されるFqの列。
 * @param[in]     i    整数。関数内で文字列に変換され、AESの入力に追加される。
 * @param[in]     tau  棄却サンプリングの入力文字列のサイズ(Byte)。
 * @param[in]     nd   vecのサイズ。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int rej_samp_PRG_AES(AES_CTX* ctx, Fq* vec, const int i, const int tau, const int nd,
                     const QRUOV_params* para);

/**
 * Fqの列をFqlの行列に変換する関数。
 *
 * @param[out] R      Fqlの行列。
 * @param[in]  vec    Fqの列。
 * @param[in]  veclen vecのサイズ。
 * @param[in]  para   パラメータの構造体。
 * @retun             0:成功。-1以下:エラー。
 */
int expand_matrix(FQL_MATRIX* R, const Fq* vec, const int veclen, const QRUOV_params* para);

/**
 * Fqの列をFqlの行列に変換する関数のq=127における最適化関数。
 *
 * @param[out] R      Fqlの行列。
 * @param[in]  vec    Fqの列。
 * @param[in]  veclen vecのサイズ。
 * @param[in]  para   パラメータの構造体。
 * @retun             0:成功。-1以下:エラー。
 */
int expand_matrix_op(FQL_MATRIX_OP* R, const Fq* vec, const int veclen, const QRUOV_params* para);

/**
 * SdTをランダムに生成する関数。
 *
 * @param[out]    SdT  Fqlのpara->V*para->M行列の転置行列。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int expand_sk(FQL_MATRIX* SdT, QRUOV_params* para);

/**
 * SdTをランダムに生成する関数のq=127における最適化関数。
 *
 * @param[out]    SdT  Fqlのpara->V*para->M行列の転置行列。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int expand_sk_op(FQL_MATRIX_OP* SdT, QRUOV_params* para);

/**
 * Pi1, Pi2Tをランダムに生成する関数。
 *
 * @param[out]    Pi1  Fqlのpara->V*para->V対称行列。
 * @param[out]    Pi2T Fqlのpara->V*para->M行列の転置行列。
 * @param[in]     i    Pi1やPi2Tのインデックス。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int expand_pk(FQL_MATRIX* Pi1, FQL_MATRIX* Pi2T, const int i, QRUOV_params* para);

/**
 * Pi1, Pi2Tをランダムに生成する関数のq=127における最適化関数。
 *
 * @param[out]    Pi1  Fqlのpara->V*para->V対称行列。
 * @param[out]    Pi2T Fqlのpara->V*para->M行列の転置行列。
 * @param[in]     i    Pi1やPi2Tのインデックス。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int expand_pk_op(FQL_MATRIX_OP* Pi1, FQL_MATRIX_OP* Pi2T, const int i, QRUOV_params* para);

/**
 * yをランダムに生成する関数。
 *
 * @param[out]    y    Fqlのpara->V*1行列。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int expand_y(FQL_MATRIX* y, const QRUOV_params* para);

/**
 * yをランダムに生成する関数のq=127における最適化関数。
 *
 * @param[out]    y    Fqlのpara->V*1行列。
 * @param[in]     para パラメータの構造体。
 * @return             0:成功。-1以下:エラー。
 */
int expand_y_op(FQL_MATRIX_OP* y, const QRUOV_params* para);

/**
 * SHAKE256を用いてmuからハッシュ値を生成する関数。
 *
 * @param[out] t    ハッシュ値。Fqのpara->m*1行列。
 * @param[in]  mu   SHAKE256の入力文字列。サイズは64Byte。
 * @param[in]  r    ランダムソルト。サイズはpara->salt_len-Byte。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Hash(FQ_MATRIX* t, const unsigned char* mu, const unsigned char* r, const QRUOV_params* para);

#endif /* EXPAND_H_INCLUDED */
