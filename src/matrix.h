/**
 * @file  matrix.h
 * @brief FQ_MATRIX型とFQL_MATRIX型の定義と、それらの演算の関数を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#ifdef DEBUG
  #include <stdio.h>
#endif  /* DEBUG */
#include <stdlib.h>

#include "parameter.h"
#include "Fql.h"

/** 行列の(i,j)要素のFq_matrix/Fql_matrixでのインデックス。 */
#define INDEX(i, j, col) ((i)*(col)+(j))

/** 対称行列の(i,j)要素のFql_matrixでのインデックス。 */
#define SYMMETRIC_INDEX(i, j, col) \
        (((i)<=(j))?((2*(col)-(i)+1)*(i)/2+(j)-(i)):((2*(col)-(j)+1)*(j)/2+(i)-(j)))

/** Fqの行列を表す構造体。 */
typedef struct Fq_matrix{
  Fq* data;
  int row;
  int col;
  int size;
}FQ_MATRIX;

/**
 * Fqの行列を初期化する関数。
 *
 * @param[out] R   Fqの行列。関数内でメモリが確保される。
 * @param[in]  row 行列の列数。
 * @param[in]  col 行列の列数。
 * @return         0:成功。-1以下:エラー。
 */
int Fq_matrix_init(FQ_MATRIX* R, int row, int col);

/**
 * Fqの行列を解放する関数。
 *
 * @param[in/out] R 任意のFqの行列。関数内でメモリが解放される。
 */
void Fq_matrix_free(FQ_MATRIX* R);

/**
 * Fqの行列を零埋めする関数。
 *
 * @param[in/out] R 任意のFqの行列。関数内で零行列に上書きされる。
 */
void Fq_matrix_clear(FQ_MATRIX* R);

/**
 * Fqの行列の加算を行う関数。
 *
 * @param[out] R    AとBの和であるようなFqの行列。各Fqはpara->qで剰余されている。
 * @param[in]  A    任意のFqの行列。
 * @param[in]  B    任意のFqの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fq_matrix_add(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B, const QRUOV_params* para);

/**
 * Fqの行列の減算を行う関数。
 *
 * @param[out] R    AとBの差であるようなFqの行列。各Fqはpara->qで剰余されている。
 * @param[in]  A    任意のFqの行列。
 * @param[in]  B    任意のFqの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fq_matrix_sub(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B, const QRUOV_params* para);

/**
 * Fqの行列の乗算を行う関数。
 *
 * @param[out] R    AとBの積であるようなFqの行列。各Fqはpara->qで剰余されている。
 *                  A,Bとは別の行列を指定すること。
 * @param[in]  A    任意のFqの行列。
 * @param[in]  B    任意のFqの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fq_matrix_mul(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B, const QRUOV_params* para);

/**
 * Fqの行列の乗算を行う関数のq=127における最適化関数。
 *
 * @param[out] R    AとBの積であるようなFqの行列。各Fqは127で剰余されている。
 *                  A,Bとは別の行列を指定すること。
 * @param[in]  A    任意のFqの行列。
 * @param[in]  B    任意のFqの行列。
 * @return          0:成功。-1以下:エラー。
 */
int Fq_matrix_mul_op(FQ_MATRIX* R, const FQ_MATRIX* A, const FQ_MATRIX* B);

/** Fqlの行列を表す構造体。 */
typedef struct Fql_matrix{
  Fq** data;
  int row;
  int col;
  int size;
  int symmetric;
  int transpose;
}FQL_MATRIX;

/** Fqlの行列を表す構造体。q=127における最適化関数で用いる。 */
typedef struct Fql_matrix_op{
  Fql* data;
  int row;
  int col;
  int size;
  int symmetric;
  int transpose;
}FQL_MATRIX_OP;

/**
 * Fqlの行列を初期化する関数。
 *
 * @param[out] R         Fqlの行列。関数内でメモリが確保される。
 * @param[in]  row       行列の列数。
 * @param[in]  col       行列の列数。
 * @param[in]  symmetric 対称行列かどうかを表すフラグ。0:対称行列とは限らない。1:対称行列。
 * @param[in]  transpose 転置行列かどうかを表すフラグ。0:転置行列ではない。1:転置行列。
 * @param[in]  para      パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int Fql_matrix_init(FQL_MATRIX* R, int row, int col, int symmetric, int transpose,
                    const QRUOV_params* para);

/**
 * Fqlの行列を初期化する関数のq=127における最適化関数。
 *
 * @param[out] R         Fqlの行列。関数内でメモリが確保される。
 * @param[in]  row       行列の列数。
 * @param[in]  col       行列の列数。
 * @param[in]  symmetric 対称行列かどうかを表すフラグ。0:対称行列とは限らない。1:対称行列。
 * @param[in]  transpose 転置行列かどうかを表すフラグ。0:転置行列ではない。1:転置行列。
 * @param[in]  para      パラメータの構造体。
 * @return               0:成功。-1以下:エラー。
 */
int Fql_matrix_init_op(FQL_MATRIX_OP* R, int row, int col, int symmetric, int transpose,
                       const QRUOV_params* para);

/**
 * Fqlの行列を解放する関数。
 *
 * @param[in/out] R 任意のFqlの行列。関数内でメモリが解放される。
 */
void Fql_matrix_free(FQL_MATRIX* R);

/**
 * Fqlの行列を解放する関数のq=127における最適化関数。
 *
 * @param[in/out] R 任意のFqlの行列。関数内でメモリが解放される。
 */
void Fql_matrix_free_op(FQL_MATRIX_OP* R);

/**
 * Fqlの行列を零埋めする関数。
 *
 * @param[in/out] R    任意のFqlの行列。関数内で零行列に上書きされる。
 * @param[in]     para パラメータの構造体。
 */
void Fql_matrix_clear(FQL_MATRIX* R, const QRUOV_params* para);

/**
 * Fqlの行列を零埋めする関数のq=127における最適化関数。
 *
 * @param[in/out] R    任意のFqlの行列。関数内で零行列に上書きされる。
 * @param[in]     para パラメータの構造体。
 */
void Fql_matrix_clear_op(FQL_MATRIX_OP* R, const QRUOV_params* para);

/**
 * Fqlの行列を転置する関数。
 *
 * @param[in/out] R 任意のFqlの行列。関数内で転置される。
 */
void Fql_matrix_transpose(FQL_MATRIX* R);

/**
 * Fqlの行列を転置する関数のq=127における最適化関数。
 *
 * @param[in/out] R 任意のFqlの行列。関数内で転置される。
 */
void Fql_matrix_transpose_op(FQL_MATRIX_OP* R);

/**
 * Fqlの行列をコピーする関数。
 *
 * @param[out] R    AがコピーされたFqlの行列。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_copy(FQL_MATRIX* R, const FQL_MATRIX* A, const QRUOV_params* para);

/**
 * Fqlの行列をコピーする関数のq=127における最適化関数。
 *
 * @param[out] R    AがコピーされたFqlの行列。
 * @param[in]  A    任意のFqlの行列。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_copy_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A);

/**
 * Fqlの行列の加算を行う関数。
 *
 * @param[out] R    AとBの和であるようなFqlの行列。Fqlの各係数はpara->qで剰余されている。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_add(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                   const QRUOV_params* para);

/**
 * Fqlの行列の加算を行う関数のq=127における最適化関数。
 *
 * @param[out] R    AとBの和であるようなFqlの行列。Fqlの各係数はpara->qで剰余されている。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_add_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                      const QRUOV_params* para);
                   
/**
 * Fqlの行列の減算を行う関数。
 *
 * @param[out] R    AとBの差(A-B)であるようなFqlの行列。Fqlの各係数はpara->qで剰余されている。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_sub(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                   const QRUOV_params* para);

/**
 * Fqlの行列の減算を行う関数のq=127における最適化関数。
 *
 * @param[out] R    AとBの差(A-B)であるようなFqlの行列。Fqlの各係数はpara->qで剰余されている。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_sub_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                      const QRUOV_params* para);

/**
 * Fqlの行列の乗算を行う関数。
 *
 * @param[out] R    AとBの積であるようなFqlの行列。Fqlの各係数はpara->qで剰余されている。
 *                  A,Bとは別の行列を指定すること。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_mul(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                   const QRUOV_params* para);

/**
 * Fqlの行列の乗算を行う関数のq=127における最適化関数。
 *
 * @param[out] R    AとBの積であるようなFqlの行列。Fqlの各係数はpara->qで剰余されている。
 *                  A,Bとは別の行列を指定すること。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_mul_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                      const QRUOV_params* para);
                   
/**
 * Fqlの行列の乗算と加算を行う関数。
 *
 * @param[out] R    AとBの積にCを加算したFqlの行列。Fqlの各係数はpara->qで剰余されている。
 *                  A,Bとは別の行列を指定すること。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  C    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_mul_add(FQL_MATRIX* R, const FQL_MATRIX* A, const FQL_MATRIX* B,
                       const FQL_MATRIX* C, const QRUOV_params* para);

/**
 * Fqlの行列の乗算と加算を行う関数のq=127における最適化関数。
 *
 * @param[out] R    AとBの積にCを加算したFqlの行列。Fqlの各係数はpara->qで剰余されている。
 *                  A,Bとは別の行列を指定すること。
 * @param[in]  A    任意のFqlの行列。
 * @param[in]  B    任意のFqlの行列。
 * @param[in]  C    任意のFqlの行列。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_matrix_mul_add_op(FQL_MATRIX_OP* R, const FQL_MATRIX_OP* A, const FQL_MATRIX_OP* B,
                          const FQL_MATRIX_OP* C, const QRUOV_params* para);
                       
/**
 * Fql*Wを計算したとき、計算結果のFqlの項が元のFqlのどの項だったのかを出力する関数。
 *
 * @param[out] index_a  index_aWのaにおける項の次数。
 * @param[in]  index_aW a*WをあらわすFqlの任意の項の次数。
 * @param[in]  para     パラメータの構造体。
 * @return              0:成功。-1以下:エラー。
 */
int Fql_index_permute(int* index_a, const int index_aW, const QRUOV_params* para);

#endif /* MATRIX_H_INCLUDED */
