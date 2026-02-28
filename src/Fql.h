/**
 * @file  Fql.h
 * @brief Fq型の定義と、FqとFqlに関する演算の関数を定義したヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef FQL_H_INCLUDED
#define FQL_H_INCLUDED

#include <stdint.h>

#include "parameter.h"
#include "random.h"

/** 有限体Fqの元を表す型。 */
typedef uint8_t Fq;

/** Fqlの元を表す型。q=127における最適化関数で用いる。 */
typedef uint64_t Fql;

/** mod 127の事前計算テーブル。q=127における最適化関数で用いる。 */
extern const uint64_t mod127_tbl[258];

/**
 * Fqlを初期化する関数。
 * @param[out] r    Fqの配列。関数内でメモリが確保される。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_init(Fq** r, const QRUOV_params* para);

/**
 * Fqlを解放する関数。
 *
 * @param[in/out] r Fqの配列。関数内でメモリが解放される。
 */
void Fql_free(Fq** r);

/**
 * Fqの加算を行う関数。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの和であるようなFq。para->qで剰余されている。
 */
inline Fq Fq_add(Fq a, Fq b, const QRUOV_params* para){
  return (Fq)(((uint16_t)a + (uint16_t)b) % para->q);
}

/**
 * Fqの加算を行う関数の。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの和であるようなFq。para->qで剰余されている。
 */
inline Fq Fq_add_op2(Fq a, Fq b, const QRUOV_params* para){
  return (Fq)(((uint16_t)a + (uint16_t)b) % 31);
}

/**
 * Fqの減算を行う関数。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの差であるようなFq。para->qで剰余されている。
 */
inline Fq Fq_sub(Fq a, Fq b, const QRUOV_params* para){
  return (Fq)(((uint16_t)para->q + (uint16_t)a - (uint16_t)b) % para->q);
}

/**
 * Fqの減算を行う関数。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの差であるようなFq。para->qで剰余されている。
 */
inline Fq Fq_sub_op2(Fq a, Fq b, const QRUOV_params* para){
  return (Fq)(((uint16_t)31 + (uint16_t)a - (uint16_t)b) % 31);
}

/**
 * Fqの乗算を行う関数。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの積であるようなFq。para->qで剰余されている。
 */
inline Fq Fq_mul(Fq a, Fq b, const QRUOV_params* para){
  return (Fq)(((uint16_t)a * (uint16_t)b) % para->q);
}

/**
 * Fqの乗算を行う関数。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの積であるようなFq。para->qで剰余されている。
 */
inline Fq Fq_mul_op2(Fq a, Fq b, const QRUOV_params* para){
  return (Fq)(((uint16_t)a * (uint16_t)b) % 31);
}

/**
 * Fqの乗算と加算を行う関数。
 *
 * @param[in]  a    任意のFq。
 * @param[in]  b    任意のFq。
 * @param[in]  c    任意のFq。
 * @param[in]  para パラメータの構造体。
 * @return          aとbの積にcを加算したFq。para->qで剰余されている。
 */
inline Fq Fq_mul_add(Fq a, Fq b, Fq c, const QRUOV_params* para){
  return (Fq)(((uint16_t)a * (uint16_t)b + (uint16_t)c) % para->q);
}

/**
 * Fqの4次のベクトルの内積を行う関数のq=127における最適化関数。
 *
 * 16bitごとに下位から番号をふる。変数.0～3。a == (a.3 << 48)|(a.2 << 32)|(a.1 << 16)|a.0。
 * (aとbのふりかたが揃っていれば、下位からでも上位からでもどちらでもよい。)
 * c = c + a.0*b.0 + a.1*b.1 + a.2*b.2 + a.3*b.3 in F_127
 *
 * @param[in]  a    任意のFqの4次ベクトル。
 * @param[in]  b    任意のFqの4次ベクトル。
 * @param[in]  c    任意のFq。
 * @return          aとbの内積にcを加算したFq。127で剰余されている。
 */
static inline Fq Fq_vec_mul_add_op(const uint64_t a[1], const uint64_t b[1], uint8_t c[1]) {

  // aのワード順を逆転させる。t.3=a.0, t.2=a.1, t.1=a.2, t.0=a.3
  uint64_t t = (a[0] >> 32) | (a[0] << 32);
  t = ((t & 0xffff0000ffff0000) >> 16) | ((t & 0x0000ffff0000ffff) << 16);

  // t.3 = t.3*b.0 + t.2*b.1 + t.1*b.2 + t.0*b.3 = a.0*b.0 + a.1*b.1 + a.2*b.2 + a.3*b.3 最大63504
  t *= b[0];
  t >>= 48;
  t += c[0]; // 最大63630
  t = (t >> 14) + ((t >> 7) & 0x007f) + (t & 0x007f); // 最大3+127+127=257

  return mod127_tbl[t];
}

/**
 * Fqlに零を代入する関数。
 *
 * @param[in/out] r    任意のFql。関数内で零に上書きされる。
 * @param[in]     para パラメータの構造体。
*/
void Fql_zero(Fq* r, const QRUOV_params* para);

/**
 * Fqlのコピーを行う関数。
 *
 * @param[out] r    aがコピーされたFql。
 * @param[in]  a    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_copy(Fq* r, const Fq* a, const QRUOV_params* para);

/**
 * Fqlのコピーを行う関数。
 *
 * @param[out] r    aがコピーされたFql。
 * @param[in]  a    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_copy_op2(Fq* r, const Fq* a, const QRUOV_params* para);

/**
 * Fqlの加算を行う関数。
 *
 * @param[out] r    aとbの和であるようなFql。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_add(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fqlの加算を行う関数のq=127における最適化関数。
 *
 * @param[out] r    aとbの和であるようなFql。各係数は127で剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 */
void Fql_add_op(Fql* r, const Fql* a, const Fql* b);

/**
 * Fqlの加算を行う関数。
 *
 * @param[out] r    aとbの和であるようなFql。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_add_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fqlの減算を行う関数。
 *
 * @param[out] r    aとbの差(a-b)であるようなFql。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_sub(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fqlの減算を行う関数のq=127における最適化関数。
 *
 * @param[out] r    aとbの差(a-b)であるようなFql。各係数は127で剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 */
void Fql_sub_op(Fql* r, const Fql* a, const Fql* b);

/**
 * Fqlの減算を行う関数。
 *
 * @param[out] r    aとbの差(a-b)であるようなFql。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_sub_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fqlの乗算を行う関数。
 *
 * @param[out] r    aとbの積であるようなFql_accumulator。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_mul(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fqlの乗算を行う関数。
 *
 * @param[out] r    aとbの積であるようなFql_accumulator。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
void Fql_mul_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fqlの乗算と加算を行う関数のq=127における最適化関数。
 *
 * @param[out] r    aとbの積にcを加算したFql。各係数は127で剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  c    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
static inline void Fql_mul_add_op(Fql r[1], const Fql a[1], const Fql b[1], const Fql c[1]){

  // 16bitに収まる最大値は65535
  uint64_t ab = a[0] * b[0]; // ab.3～ab.0は最大47628
  ab =   ((ab & 0xc000c000c000c000) >> 14)
       + ((ab & 0x3f803f803f803f80) >> 7)
       + (ab & 0x007f007f007f007f); // ab.3～ab.0は最大3+127+127=257
  uint64_t t = (a[0] >> 32) * (b[0] >> 32); // t.t.0は最大47628

  t = (ab >> 48) | (t << 16); // t.3=t.2=0、t.1は最大47628、t.0は最大257
  t += (t << 16); // t.3は0、t.2は最大47628、t.1は最大47628+257=47885、t.0は最大257+0=257
  t += ab; // (t.3は最大257:a1*b1+a1*b2の残骸でもう使わない)、
           // t.2は最大47628+257=47885、t.1は最大47885+257=48142、t.0は最大257+257=514
  t += c[0]; // (t.3は最大257:a1*b1+a1*b2の残骸でもう使わない)、
             // t.2は最大47885+126=48011、t.1は最大48142+126=48268、t.0は最大514+126=640

  // マスク値を上位16bitを0にしてt.3を消しながらmod 127
  t =   ((t & 0x0000c000c000c000) >> 14)
      + ((t & 0x00003f803f803f80) >> 7) 
      + (t & 0x0000007f007f007f); // t.3は0、t.2～t.0は最大3+127+127=257
  r[0] =   (mod127_tbl[t >> 32] << 32)
         | (mod127_tbl[(t >> 16) & 0x01ff] << 16) 
         | mod127_tbl[t & 0x01ff];

  ab = t = 0;

  return;
}

/**
 * Fqlの乗算を行う関数のq=127における最適化関数。
 *
 * @param[out] r    aとbの積であるようなFql。各係数は127で剰余されている。
 * @param[in]  a    任意のFql。
 * @param[in]  b    任意のFql。
 * @param[in]  para パラメータの構造体。
 */
//TODO para不使用
static inline void Fql_mul_op(Fql* r, const Fql* a, const Fql* b, const QRUOV_params* para){
  Fql c = 0;
  Fql_mul_add_op(r, a, b, &c);

  return;
}

/**
 * Fql_accumulatorを初期化する関数。
 * @param[out] r    Fqの配列。関数内でメモリが確保される。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_accumulator_init(Fq** r, const QRUOV_params* para);

/**
 * Fql_accumulatorを解放する関数。
 *
 * @param[in/out] r Fqの配列。関数内でメモリが解放される。
 */
void Fql_accumulator_free(Fq** r);

/**
 * Fql_accumulatorに零を代入する関数。
 *
 * @param[in/out] r    任意のFql_accumulator。関数内で零に上書きされる。
 * @param[in]     para パラメータの構造体。
 */
void Fql_accumulator_zero(Fq* r, const QRUOV_params* para);

/**
 * Fql_accumulatorの加算を行う関数。
 *
 * @param[out] r    aとbの和であるようなFql_accumulator。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql_accumulator。
 * @param[in]  b    任意のFql_accumulator。
 * @param[in]  para パラメータの構造体。
 */
void Fql_accumulator_add(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fql_accumulatorの加算を行う関数。
 *
 * @param[out] r    aとbの和であるようなFql_accumulator。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql_accumulator。
 * @param[in]  b    任意のFql_accumulator。
 * @param[in]  para パラメータの構造体。
 */
void Fql_accumulator_add_op2(Fq* r, const Fq* a, const Fq* b, const QRUOV_params* para);

/**
 * Fql_accumulatorのコピーを行う関数。
 *
 * @param[out] r    aがコピーされたFql_accumulator。
 * @param[in]  a    任意のFql_accumulator。
 * @param[in]  para パラメータの構造体。
 */
void Fql_accumulator_copy(Fq* r, const Fq* a, const QRUOV_params* para);

/**
 * Fql_accumulatorの剰余を行う関数。
 *
 * @param[out] r    既約多項式により剰余されたFql。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql_accumulator。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_accumulator_reduce(Fq* r, const Fq* a, const QRUOV_params* para);

/**
 * Fql_accumulatorの剰余を行う関数。
 *
 * @param[out] r    既約多項式により剰余されたFql。各係数はpara->qで剰余されている。
 * @param[in]  a    任意のFql_accumulator。
 * @param[in]  para パラメータの構造体。
 * @return          0:成功。-1以下:エラー。
 */
int Fql_accumulator_reduce_op2(Fq* r, const Fq* a, const QRUOV_params* para);

/**
 * Fqlの係数を得る関数。
 *
 * @param[out] r      aのdegree次の係数であるようなFq。
 * @param[in]  a      任意のFql。
 * @param[in]  degree 項の次数。0からpara->l-1までの値とする。
 * @param[in]  para   パラメータの構造体。
 * @return            0:成功。-1以下:エラー。
 */
int Fql2Fq(Fq* r, const Fq* a, const int degree, const QRUOV_params* para);

/**
 * Fqlの係数を得る関数のq=127における最適化関数。
 *
 * @param[out] r      aのdegree次の係数であるようなFq。
 * @param[in]  a      任意のFql。
 * @param[in]  degree 項の次数。0からpara->l-1までの値とする。
 * @param[in]  para   パラメータの構造体。
 * @return            0:成功。-1以下:エラー。
 */
int Fql2Fq_op(Fq* r, const Fql* a, const int degree, const QRUOV_params* para);

#endif /* FQL_H_INCLUDED */
