/**
 * @file  qruov.h
 * @brief QR-UOV署名ライブラリの外部インタフェースのヘッダファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 *
 * 各関数の返り値のエラーコードは、QR-UOV署名ライブラリ_(別紙)_メッセージ仕様.xlsxを参照。
 */

#ifndef QRUOV_H_INCLUDED
#define QRUOV_H_INCLUDED

/*
 * パラメータの初期化を行う関数。
 *
 * @param[in] param_name パラメータ名。
 * @param[in] prg        疑似乱数生成器。"SHAKE"または"AES"を指定。
 * @param[in] para       パラメータの構造体。関数の実行前にメモリを確保しておくこと。
 * @return               0:成功。-1以下:エラー。
 */
int QRUOV_paramgen(char* param_name, char* prg, QRUOV_params* para);

/**
 * QR-UOVの鍵生成を行う関数。
 *
 * @param[out]    pk    公開鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] pklen pkのサイズ(Byte)。関数内でpkに書き込まれたサイズが格納される。
 * @param[out]    sk    秘密鍵。関数の実行前に必要なメモリを確保しておくこと。
 * @param[in/out] sklen skのサイズ(Byte)。関数内でskに書き込まれたサイズが格納される。
 * @param[in]     para  パラメータの構造体。
 * @return              0:成功。-1以下:エラー。
 */
int QRUOV_keygen(unsigned char* pk, size_t* pklen, unsigned char* sk,
                 size_t* sklen, QRUOV_params* para);

/**
 * QR-UOVの署名生成を行う関数。
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
int QRUOV_sign(unsigned char* sk, size_t sklen, unsigned char* sig,
               size_t* siglen, unsigned char* m, size_t mlen, QRUOV_params* para);

/**
 * QR-UOVの署名検証を行う関数。
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
int QRUOV_verify(unsigned char* pk, size_t pklen, unsigned char* sig,
                 size_t siglen, unsigned char* m, size_t mlen, QRUOV_params* para);

#endif /* QRUOV_H_INCLUDED */
