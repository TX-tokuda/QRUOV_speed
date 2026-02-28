/**
 * @file  qruov_matrix.h
 * @brief QR-UOVのアルゴリズム内の行列演算の関数を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef QRUOV_MATRIX_H_INCLUDED
#define QRUOV_MATRIX_H_INCLUDED

#include "parameter.h"
#include "matrix.h"

/**
 * Pi3を計算する関数。
 *
 * @param[out] Pi3  Fqlのpara->M*para->M対称行列。
 * @param[in]  Pi1  Fqlのpara->V*para->V行列。
 * @param[in]  Pi2T Fqlのpara->M*para->V行列。
 * @param[in]  SdT  Fqlのpara->M*para->V行列。
 * @param[in]  Sd   Fqlのpara->V*para->M行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int compute_Pi3(FQL_MATRIX* Pi3, const FQL_MATRIX* Pi1, FQL_MATRIX* Pi2T, FQL_MATRIX* SdT,
                FQL_MATRIX* Sd, const QRUOV_params* para);

/**
 * Pi3を計算する関数のq=127における最適化関数。
 *
 * @param[out] Pi3  Fqlのpara->M*para->M対称行列。
 * @param[in]  Pi1  Fqlのpara->V*para->V行列。
 * @param[in]  Pi2T Fqlのpara->M*para->V行列。
 * @param[in]  SdT  Fqlのpara->M*para->V行列。
 * @param[in]  Sd   Fqlのpara->V*para->M行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int compute_Pi3_op(FQL_MATRIX_OP* Pi3, const FQL_MATRIX_OP* Pi1, FQL_MATRIX_OP* Pi2T,
                   FQL_MATRIX_OP* SdT, FQL_MATRIX_OP* Sd, const QRUOV_params* para);

/**
 * Pi3を計算する関数のparameter==2における最適化関数。
 *
 * @param[out] Pi3  Fqlのpara->M*para->M対称行列。
 * @param[in]  Pi1  Fqlのpara->V*para->V行列。
 * @param[in]  Pi2T Fqlのpara->M*para->V行列。
 * @param[in]  SdT  Fqlのpara->M*para->V行列。
 * @param[in]  Sd   Fqlのpara->V*para->M行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int compute_Pi3_op2(FQL_MATRIX* Pi3, const FQL_MATRIX* Pi1, FQL_MATRIX* Pi2T, FQL_MATRIX* SdT,
                FQL_MATRIX* Sd, const QRUOV_params* para);

/**
 * L_iとu_iを計算する関数。
 *
 * @param[out] Li    Fqのpara->m*1行列。
 * @param[out] ui    Fqの1*1行列。
 * @param[in]  P1i   FQLのpara->V*para->V行列。
 * @param[in]  P2Ti  FQLのpara->V*para->M行列。
 * @param[in]  SdT   FQLのpara->V*para->M行列。
 * @param[in]  y     FQLのpara->V*1行列。
 * @param[in]  para  パラメータの構造体。
 * @return           0:成功。-1以下:エラー。
 */
int compute_Li_ui(FQ_MATRIX* Li, FQ_MATRIX* ui, const FQL_MATRIX* P1i, 
                  const FQL_MATRIX* P2Ti, const FQL_MATRIX* SdT, const FQL_MATRIX* y,
                  const QRUOV_params* para);

/**
 * L_iとu_iを計算する関数のq=127における最適化関数。
 *
 * @param[out] Li    Fqのpara->m*1行列。
 * @param[out] ui    Fqの1*1行列。
 * @param[in]  P1i   FQLのpara->V*para->V行列。
 * @param[in]  P2Ti  FQLのpara->V*para->M行列。
 * @param[in]  SdT   FQLのpara->V*para->M行列。
 * @param[in]  y     FQLのpara->V*1行列。
 * @param[in]  para  パラメータの構造体。
 * @return           0:成功。-1以下:エラー。
 */
int compute_Li_ui_op(FQ_MATRIX* Li, FQ_MATRIX* ui, const FQL_MATRIX_OP* P1i, 
                     const FQL_MATRIX_OP* P2Ti, const FQL_MATRIX_OP* SdT, const FQL_MATRIX_OP* y,
                     const QRUOV_params* para);

/**
 * L_iとu_iを計算する関数のparameter==2における最適化関数。
 *
 * @param[out] Li    Fqのpara->m*1行列。
 * @param[out] ui    Fqの1*1行列。
 * @param[in]  P1i   FQLのpara->V*para->V行列。
 * @param[in]  P2Ti  FQLのpara->V*para->M行列。
 * @param[in]  SdT   FQLのpara->V*para->M行列。
 * @param[in]  y     FQLのpara->V*1行列。
 * @param[in]  para  パラメータの構造体。
 * @return           0:成功。-1以下:エラー。
 */
int compute_Li_ui_op2(FQ_MATRIX* Li, FQ_MATRIX* ui, const FQL_MATRIX* P1i, 
                  const FQL_MATRIX* P2Ti, const FQL_MATRIX* SdT, const FQL_MATRIX* y,
                  const QRUOV_params* para);
                                      
/**
 * sを計算する関数。
 *
 * @param[out] s    Fqlのpara->N*1行列。
 * @param[in]  y    Fqlのpara->V*1行列。
 * @param[in]  x    Fqlのpara->M*1行列。
 * @param[in]  Sd   Fqlのpara->V*para->M行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int compute_s(FQL_MATRIX* s, const FQL_MATRIX* y, const FQL_MATRIX* x, const FQL_MATRIX* Sd,
              const QRUOV_params* para);


/**
 * sを計算する関数のq=127における最適化関数。
 *
 * @param[out] s    Fqlのpara->N*1行列。
 * @param[in]  y    Fqlのpara->V*1行列。
 * @param[in]  x    Fqlのpara->M*1行列。
 * @param[in]  Sd   Fqlのpara->V*para->M行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int compute_s_op(FQL_MATRIX_OP* s, const FQL_MATRIX_OP* y, const FQL_MATRIX_OP* x,
                 const FQL_MATRIX_OP* Sd, const QRUOV_params* para);

/**
 * sを計算する関数のparameter==2における最適化関数。
 *
 * @param[out] s    Fqlのpara->N*1行列。
 * @param[in]  y    Fqlのpara->V*1行列。
 * @param[in]  x    Fqlのpara->M*1行列。
 * @param[in]  Sd   Fqlのpara->V*para->M行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int compute_s_op2(FQL_MATRIX* s, const FQL_MATRIX* y, const FQL_MATRIX* x, const FQL_MATRIX* Sd,
              const QRUOV_params* para);

/**
 * tdを計算する関数。
 *
 * @param[out] td        Fqのpara->m*1行列。
 * @param[in]  s         Fqlのpara->N*1行列。
 * @param[in]  P1        Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[in]  P2T       Fqlのpara->M*para->V行列のpara->m個の配列。
 * @param[in]  P3        Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  para      パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int compute_td(FQ_MATRIX* td, const FQL_MATRIX* s, const FQL_MATRIX* P1,
               const FQL_MATRIX* P2T, const FQL_MATRIX* P3, const QRUOV_params* para); 


/**
 * tdを計算する関数のq=127における最適化関数。
 *
 * @param[out] td        Fqのpara->m*1行列。
 * @param[in]  s         Fqlのpara->N*1行列。
 * @param[in]  P1        Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[in]  P2T       Fqlのpara->M*para->V行列のpara->m個の配列。
 * @param[in]  P3        Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  para      パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int compute_td_op(FQ_MATRIX* td, const FQL_MATRIX_OP* s, const FQL_MATRIX_OP* P1,
                  const FQL_MATRIX_OP* P2T, const FQL_MATRIX_OP* P3, const QRUOV_params* para); 

/**
 * tdを計算する関数のparameter==2における最適化関数。
 *
 * @param[out] td        Fqのpara->m*1行列。
 * @param[in]  s         Fqlのpara->N*1行列。
 * @param[in]  P1        Fqlのpara->V*para->V対称行列のpara->m個の配列。
 * @param[in]  P2T       Fqlのpara->M*para->V行列のpara->m個の配列。
 * @param[in]  P3        Fqlのpara->M*para->M対称行列のpara->m個の配列。
 * @param[in]  para      パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int compute_td_op2(FQ_MATRIX* td, const FQL_MATRIX* s, const FQL_MATRIX* P1,
               const FQL_MATRIX* P2T, const FQL_MATRIX* P3, const QRUOV_params* para); 

/**
 * PAをLUに分解する関数。
 *
 * @param[out] P                置換配列。intのpara->m個の配列。
 * @param[out] L                Fqのpara->m*para->mの下三角行列。
 * @param[out] U                Fqのpara->m*para->mの階段行列。
 * @param[out] rank             ランク
 * @param[out] first_col_index  各行の先頭列番号を保持したpara->m個の配列。
 * @param[in]  A_org            LU分解する対象のFqのpara->m*para->mの行列。
 * @param[in]  para             パラメータの構造体。
 * @return                      0:成功。-1以下:エラー。
 */
int LU_decompose(int* P, FQ_MATRIX* L, FQ_MATRIX* U, int* rank, int* first_col_index,
                 FQ_MATRIX* A_org, const QRUOV_params* para);

/**
 * PAをLUに分解する関数のparameter==2における最適化関数。
 *
 * @param[out] P                置換配列。intのpara->m個の配列。
 * @param[out] L                Fqのpara->m*para->mの下三角行列。
 * @param[out] U                Fqのpara->m*para->mの階段行列。
 * @param[out] rank             ランク
 * @param[out] first_col_index  各行の先頭列番号を保持したpara->m個の配列。
 * @param[in]  A_org            LU分解する対象のFqのpara->m*para->mの行列。
 * @param[in]  para             パラメータの構造体。
 * @return                      0:成功。-1以下:エラー。
 */
int LU_decompose_op2(int* P, FQ_MATRIX* L, FQ_MATRIX* U, int* rank, int* first_col_index,
                 FQ_MATRIX* A_org, const QRUOV_params* para);

/**
 * 行列Aと置換配列Pそれぞれで、i行目とj行目を入れ替える関数。
 *
 * @param[in/out] A     Fqのpara->m*para->m行列。
 * @param[in/out] P     置換配列。intのpara->m個の配列。
 * @param[in]     i_row 入れ替える行。
 * @param[in]     j_row 入れ替える行。
 * @param[in]     para  パラメータの構造体。
 * @return              0:成功。-1以下:エラー。
 */
int row_swap(FQ_MATRIX* A, int* P, const int i_row, const int j_row, const QRUOV_params* para);

/**
 * Lの逆行列を計算する関数。
 *
 * @param[out] L_inv Lの逆行列。Fqのpara->m*para->m行列。
 * @param[in]  L     Fqのpara->m*para->mの下三角行列。
 * @param[in]  rank  ランク。
 * @param[in]  para  パラメータの構造体。
 * @return           0:成功。-1以下:エラー。。
 */
int L_inverse(FQ_MATRIX* L_inv, const FQ_MATRIX* L, const int rank, const QRUOV_params* para);

/**
 * Lの逆行列を計算する関数のparameter==2における最適化関数。
 *
 * @param[out] L_inv Lの逆行列。Fqのpara->m*para->m行列。
 * @param[in]  L     Fqのpara->m*para->mの下三角行列。
 * @param[in]  rank  ランク。
 * @param[in]  para  パラメータの構造体。
 * @return           0:成功。-1以下:エラー。。
 */
int L_inverse_op2(FQ_MATRIX* L_inv, const FQ_MATRIX* L, const int rank, const QRUOV_params* para);

/**
 * Ax=bが解を持つか判定する関数。（PA=LUに分解される）
 * has_L_invが1でない場合、Lの逆行列の計算を行いhas_L_invに1を設定する。
 *
 * @param[out]    result    解を持つ場合1、解を持たない場合0。
 * @param[in/out] L_inv     Lの逆行列。Fqのpara->m*para->m行列。
 * @param[in/out] has_L_inv L_invが計算済みの場合は1、未計算の場合は0。
 * @param[in]     P         置換配列。
 * @param[in]     L         Fqのpara->m*para->mの下三角行列。
 * @param[in]     rank      ランク。
 * @param[in]     B         配列bをデータにもつFqのpara->m*1行列。
 * @param[in]     para      パラメータの構造体。
 * @return                  0:成功。-1以下:エラー。
 */
int consistent(int* result, FQ_MATRIX* L_inv, int* has_L_inv, const int* P, const FQ_MATRIX* L,
               const int rank, const FQ_MATRIX* B, const QRUOV_params* para);

/**
 * Ax=bが解を持つか判定する関数のparameter==2における最適化関数。（PA=LUに分解される）
 * has_L_invが1でない場合、Lの逆行列の計算を行いhas_L_invに1を設定する。
 *
 * @param[out]    result    解を持つ場合1、解を持たない場合0。
 * @param[in/out] L_inv     Lの逆行列。Fqのpara->m*para->m行列。
 * @param[in/out] has_L_inv L_invが計算済みの場合は1、未計算の場合は0。
 * @param[in]     P         置換配列。
 * @param[in]     L         Fqのpara->m*para->mの下三角行列。
 * @param[in]     rank      ランク。
 * @param[in]     B         配列bをデータにもつFqのpara->m*1行列。
 * @param[in]     para      パラメータの構造体。
 * @return                  0:成功。-1以下:エラー。
 */
int consistent_op2(int* result, FQ_MATRIX* L_inv, int* has_L_inv, const int* P, const FQ_MATRIX* L,
               const int rank, const FQ_MATRIX* B, const QRUOV_params* para);

/**
 * Ax=bの解を求める関数。（PA=LUに分解される）
 *
 * @param[out]    X               解の配列をデータにもつFqのpara->m*1行列。
 * @param[in]     P               置換配列。
 * @param[in]     L               Fqのpara->m*para->mの下三角行列。
 * @param[in]     U               Fqのpara->m*para->m行列。
 * @param[in]     rank            ランク。
 * @param[in]     first_col_index 各行の先頭列番号を保持したpara->m個の配列。
 * @param[in]     B               配列bをデータにもつFqのpara->m*1行列。
 * @param[in]     x_seed          xのシード。サイズはpara->seed_len-Byte。
 * @param[in]     para            パラメータの構造体。
 * @return                        0:成功。-1以下:エラー。
 */
int sample_a_solution(FQ_MATRIX* X, const int* P, const FQ_MATRIX* L, const FQ_MATRIX* U, 
                      const int rank, const int* first_col_index, const FQ_MATRIX* B,
                      const unsigned char* x_seed, const QRUOV_params* para);

/**
 * Ax=bの解を求める関数のparameter==2における最適化関数。（PA=LUに分解される）
 *
 * @param[out]    X               解の配列をデータにもつFqのpara->m*1行列。
 * @param[in]     P               置換配列。
 * @param[in]     L               Fqのpara->m*para->mの下三角行列。
 * @param[in]     U               Fqのpara->m*para->m行列。
 * @param[in]     rank            ランク。
 * @param[in]     first_col_index 各行の先頭列番号を保持したpara->m個の配列。
 * @param[in]     B               配列bをデータにもつFqのpara->m*1行列。
 * @param[in]     x_seed          xのシード。サイズはpara->seed_len-Byte。
 * @param[in]     para            パラメータの構造体。
 * @return                        0:成功。-1以下:エラー。
 */
int sample_a_solution_op2(FQ_MATRIX* X, const int* P, const FQ_MATRIX* L, const FQ_MATRIX* U, 
                      const int rank, const int* first_col_index, const FQ_MATRIX* B,
                      const unsigned char* x_seed, const QRUOV_params* para);

#endif /* QRUOV_MATRIX_H_INCLUDED */
