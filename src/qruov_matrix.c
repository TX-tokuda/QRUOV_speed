/**
 * @file  qruov_matrix.c
 * @brief QR-UOVのアルゴリズム内の行列演算の関数を実装したソースファイル。
 * @copyright Copyright (c) 2025 NTT TechnoCross Corporation
 */

#include <omp.h>

#include "expand.h"
#include "qruov_matrix.h"

int compute_Pi3(FQL_MATRIX* Pi3, const FQL_MATRIX* Pi1, FQL_MATRIX* Pi2T, FQL_MATRIX* SdT,
                FQL_MATRIX* Sd, const QRUOV_params* para){

  int ret_code = 0, ret1 = 0;

  FQL_MATRIX SdT_Pi1 = {.data = NULL};
  FQL_MATRIX Pi2T_SdT_Pi1 = {.data = NULL};
  FQL_MATRIX SdT_Pi2 = {.data = NULL};
  FQL_MATRIX Mul = {.data = NULL};

  ret1 = Fql_matrix_init(&SdT_Pi1, para->M, para->V, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1;
    goto end;
  }
  Fql_matrix_clear(&SdT_Pi1, para);
  ret1 = Fql_matrix_mul(&SdT_Pi1, SdT, Pi1, para);  // S'T*Pi1
  if (ret1!=0){
    ret_code = ret1-10;
    goto end;
  }

  ret1 = Fql_matrix_init(&Pi2T_SdT_Pi1, para->M, para->V, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-50;
    goto end;
  }
  Fql_matrix_clear(&Pi2T_SdT_Pi1, para);

  ret1 = Fql_matrix_sub(&Pi2T_SdT_Pi1 ,Pi2T , &SdT_Pi1, para); // Pi2T - S'T*Pi1
  if (ret1!=0){
    ret_code = ret1-60;
    goto end;
  }

  ret1 = Fql_matrix_init(&SdT_Pi2, para->M, para->M, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-70;
    goto end;
  }
  Fql_matrix_clear(&SdT_Pi2, para);

  Fql_matrix_transpose(Pi2T); // Pi2T -> Pi2
  ret1 = Fql_matrix_mul(&SdT_Pi2, SdT, Pi2T, para); // S'T*Pi2
  if (ret1!=0){
    ret_code = ret1-80;
    goto end;
  }
  Fql_matrix_transpose(Pi2T); // Pi2 -> Pi2T

  ret1 = Fql_matrix_init(&Mul, para->M, para->M, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-120;
    goto end;
  }
  Fql_matrix_clear(&Mul, para);

  ret1 = Fql_matrix_mul(&Mul, &Pi2T_SdT_Pi1, Sd, para); // (Pi2T - S'T*Pi1)*S'
  if (ret1!=0){
    ret_code = ret1-130;
    goto end;
  }

  ret1 = Fql_matrix_add(Pi3 ,&Mul , &SdT_Pi2, para); // (Pi2T - S'T*Pi1)*S' + S'T*Pi2
  if (ret1!=0){
    ret_code = ret1-170;
    goto end;
  }

end:
  Fql_matrix_clear(&SdT_Pi1, para);
  Fql_matrix_clear(&Pi2T_SdT_Pi1, para);
  Fql_matrix_clear(&SdT_Pi2, para);
  Fql_matrix_clear(&Mul, para);

  Fql_matrix_free(&SdT_Pi1);
  Fql_matrix_free(&Pi2T_SdT_Pi1);
  Fql_matrix_free(&SdT_Pi2);
  Fql_matrix_free(&Mul);

  return ret_code;
}

int compute_Pi3_op(FQL_MATRIX_OP* Pi3, const FQL_MATRIX_OP* Pi1, FQL_MATRIX_OP* Pi2T,
                   FQL_MATRIX_OP* SdT, FQL_MATRIX_OP* Sd, const QRUOV_params* para){

  int ret_code = 0, ret1 = 0;

  FQL_MATRIX_OP SdT_Pi1 = {.data = NULL};
  FQL_MATRIX_OP Pi2T_SdT_Pi1 = {.data = NULL};
  FQL_MATRIX_OP SdT_Pi2 = {.data = NULL};
  FQL_MATRIX_OP Mul = {.data = NULL};

  ret1 = Fql_matrix_init_op(&SdT_Pi1, para->M, para->V, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1;
    goto end;
  }
  Fql_matrix_clear_op(&SdT_Pi1, para);
  ret1 = Fql_matrix_mul_op(&SdT_Pi1, SdT, Pi1, para);  // S'T*Pi1
  if (ret1!=0){
    ret_code = ret1-10;
    goto end;
  }

  ret1 = Fql_matrix_init_op(&Pi2T_SdT_Pi1, para->M, para->V, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-50;
    goto end;
  }
  Fql_matrix_clear_op(&Pi2T_SdT_Pi1, para);

  ret1 = Fql_matrix_sub_op(&Pi2T_SdT_Pi1 ,Pi2T , &SdT_Pi1, para); // Pi2T - S'T*Pi1
  if (ret1!=0){
    ret_code = ret1-60;
    goto end;
  }

  ret1 = Fql_matrix_init_op(&SdT_Pi2, para->M, para->M, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-70;
    goto end;
  }
  Fql_matrix_clear_op(&SdT_Pi2, para);

  Fql_matrix_transpose_op(Pi2T); // Pi2T -> Pi2
  ret1 = Fql_matrix_mul_op(&SdT_Pi2, SdT, Pi2T, para); // S'T*Pi2
  if (ret1!=0){
    ret_code = ret1-80;
    goto end;
  }
  Fql_matrix_transpose_op(Pi2T); // Pi2 -> Pi2T

  ret1 = Fql_matrix_init_op(&Mul, para->M, para->M, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-120;
    goto end;
  }
  Fql_matrix_clear_op(&Mul, para);

  ret1 = Fql_matrix_mul_op(&Mul, &Pi2T_SdT_Pi1, Sd, para); // (Pi2T - S'T*Pi1)*S'
  if (ret1!=0){
    ret_code = ret1-130;
    goto end;
  }

  ret1 = Fql_matrix_add_op(Pi3 ,&Mul , &SdT_Pi2, para); // (Pi2T - S'T*Pi1)*S' + S'T*Pi2
  if (ret1!=0){
    ret_code = ret1-170;
    goto end;
  }

end:
  Fql_matrix_clear_op(&SdT_Pi1, para);
  Fql_matrix_clear_op(&Pi2T_SdT_Pi1, para);
  Fql_matrix_clear_op(&SdT_Pi2, para);
  Fql_matrix_clear_op(&Mul, para);

  Fql_matrix_free_op(&SdT_Pi1);
  Fql_matrix_free_op(&Pi2T_SdT_Pi1);
  Fql_matrix_free_op(&SdT_Pi2);
  Fql_matrix_free_op(&Mul);

  return ret_code;
}

int compute_Pi3_op2(FQL_MATRIX* Pi3, const FQL_MATRIX* Pi1, FQL_MATRIX* Pi2T, FQL_MATRIX* SdT,
                FQL_MATRIX* Sd, const QRUOV_params* para){

  int ret_code = 0, ret1 = 0;

  FQL_MATRIX SdT_Pi1 = {.data = NULL};
  FQL_MATRIX Pi2T_SdT_Pi1 = {.data = NULL};
  FQL_MATRIX SdT_Pi2 = {.data = NULL};
  FQL_MATRIX Mul = {.data = NULL};

  ret1 = Fql_matrix_init(&SdT_Pi1, para->M, para->V, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1;
    goto end;
  }
  Fql_matrix_clear(&SdT_Pi1, para);
  ret1 = Fql_matrix_mul_op2(&SdT_Pi1, SdT, Pi1, para);  // S'T*Pi1
  if (ret1!=0){
    ret_code = ret1-10;
    goto end;
  }

  ret1 = Fql_matrix_init(&Pi2T_SdT_Pi1, para->M, para->V, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-50;
    goto end;
  }
  Fql_matrix_clear(&Pi2T_SdT_Pi1, para);

  ret1 = Fql_matrix_sub_op2(&Pi2T_SdT_Pi1 ,Pi2T , &SdT_Pi1, para); // Pi2T - S'T*Pi1
  if (ret1!=0){
    ret_code = ret1-60;
    goto end;
  }

  ret1 = Fql_matrix_init(&SdT_Pi2, para->M, para->M, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-70;
    goto end;
  }
  Fql_matrix_clear(&SdT_Pi2, para);

  Fql_matrix_transpose(Pi2T); // Pi2T -> Pi2
  ret1 = Fql_matrix_mul_op2(&SdT_Pi2, SdT, Pi2T, para); // S'T*Pi2
  if (ret1!=0){
    ret_code = ret1-80;
    goto end;
  }
  Fql_matrix_transpose(Pi2T); // Pi2 -> Pi2T

  ret1 = Fql_matrix_init(&Mul, para->M, para->M, 0, 0, para);
  if (ret1!=0){
    ret_code = ret1-120;
    goto end;
  }
  Fql_matrix_clear(&Mul, para);

  ret1 = Fql_matrix_mul_op2(&Mul, &Pi2T_SdT_Pi1, Sd, para); // (Pi2T - S'T*Pi1)*S'
  if (ret1!=0){
    ret_code = ret1-130;
    goto end;
  }

  ret1 = Fql_matrix_add_op2(Pi3 ,&Mul , &SdT_Pi2, para); // (Pi2T - S'T*Pi1)*S' + S'T*Pi2
  if (ret1!=0){
    ret_code = ret1-170;
    goto end;
  }

end:
  Fql_matrix_clear(&SdT_Pi1, para);
  Fql_matrix_clear(&Pi2T_SdT_Pi1, para);
  Fql_matrix_clear(&SdT_Pi2, para);
  Fql_matrix_clear(&Mul, para);

  Fql_matrix_free(&SdT_Pi1);
  Fql_matrix_free(&Pi2T_SdT_Pi1);
  Fql_matrix_free(&SdT_Pi2);
  Fql_matrix_free(&Mul);

  return ret_code;
}

int compute_Li_ui(FQ_MATRIX* Li, FQ_MATRIX* ui, const FQL_MATRIX* Pi1, const FQL_MATRIX* Pi2T,
                  const FQL_MATRIX* SdT, const FQL_MATRIX* y, const QRUOV_params* para){

  int ret_code, ret = 0;

  FQL_MATRIX Pi1_y = {.data = NULL};
  FQL_MATRIX yT = {.data = NULL};
  FQL_MATRIX Pi2T_y = {.data = NULL};
  FQL_MATRIX SdT_Pi1_y = {.data = NULL};
  FQL_MATRIX Sub = {.data = NULL};
  FQL_MATRIX Li_Fql = {.data = NULL};
  FQL_MATRIX u_Fql = {.data = NULL};

  ret = Fql_matrix_init(&Pi1_y, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-0; goto end; }
  Fql_matrix_clear(&Pi1_y, para);

  ret = Fql_matrix_init(&yT, 1, para->V, 0, 0, para);
  if(ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_clear(&yT, para);
  for(int i=0; i<y->size; i++){
    Fql_copy(yT.data[i], y->data[i], para);
  }

  ret = Fql_matrix_init(&Pi2T_y, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-20; goto end; }
  Fql_matrix_clear(&Pi2T_y, para);

  ret = Fql_matrix_init(&SdT_Pi1_y, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-30; goto end; }
  Fql_matrix_clear(&SdT_Pi1_y, para);

  ret = Fql_matrix_init(&Sub, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-40; goto end; }
  Fql_matrix_clear(&Sub, para);

  ret = Fql_matrix_init(&Li_Fql, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-50; goto end; }
  Fql_matrix_clear(&Li_Fql, para);

  ret = Fql_matrix_init(&u_Fql, 1, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-60; goto end; }
  Fql_matrix_clear(&u_Fql, para);

  // compute
  ret = Fql_matrix_mul(&Pi1_y, Pi1, y, para); // Pi1_y := Pi1 * y
  if(ret!=0){ ret_code = ret-70; goto end; }

  ret = Fql_matrix_mul(&u_Fql, &yT, &Pi1_y, para); // u_Fql := yT * Pi1_y
  if(ret!=0){ ret_code = ret-110; goto end; }

  // u: Fql -> Fq
  // ui     FQ_MATRIX: 1 * 1
  int coeff_index_perm_0 = 0;
  ret = Fql_index_permute(&coeff_index_perm_0, 0, para);
  if(ret!=0){ ret_code = ret-150; goto end; }

  Fql2Fq(&ui->data[0], u_Fql.data[0], coeff_index_perm_0, para);

  ret = Fql_matrix_mul(&Pi2T_y, Pi2T, y, para); // Pi2T_y := Pi2T * y
  if(ret!=0){ ret_code = ret-160; goto end; }

  // SdT_Pi1_y := S'T * PiT1 * y (Pi1T=Pi1 (P1 is symmetric) )
  ret = Fql_matrix_mul(&SdT_Pi1_y, SdT, &Pi1_y, para);
  if(ret!=0){ ret_code = ret-200; goto end; }

  ret = Fql_matrix_sub(&Sub, &Pi2T_y, &SdT_Pi1_y, para); // Sub := Pi2T*y - Sd'T*Pi1T*y
  if(ret!=0){ ret_code = ret-240; goto end; }

  ret = Fql_matrix_add(&Li_Fql, &Sub, &Sub, para); // Li_Fql = 2 * Sub
  if(ret!=0){ ret_code = ret-250; goto end; }

  // L: Fql -> Fq
  int j = 0;
  int k = 0;
  int coeff_index_perm_k = 0;

#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(j, k, coeff_index_perm_k) shared(Li, Li_Fql, ret_code, para)
#endif
for(j=0; j<para->M; j++){
    for(k=0; k<para->l; k++){

      int ret1 = Fql_index_permute(&coeff_index_perm_k, k, para);
      if (ret1!=0){
        ret_code = ret1-260;
#ifndef QRUOV_USE_MULTI_THREAD
        break;
#endif
        goto end_for;
      }

    Fql2Fq(&(Li->data[INDEX(para->l * j + k, 0, Li->col)]),
              Li_Fql.data[INDEX(j, 0, Li_Fql.col)],
              coeff_index_perm_k, para);
    }
    if(ret_code!=0){
#ifndef QRUOV_USE_MULTI_THREAD
        break;
#endif
        goto end_for;
    }

    end_for: // for multi thread
    j=j; // dummy
  }

end:
  Fql_matrix_clear(&Pi1_y, para);
  Fql_matrix_clear(&yT, para);
  Fql_matrix_clear(&Pi2T_y, para);
  Fql_matrix_clear(&SdT_Pi1_y, para);
  Fql_matrix_clear(&Sub, para);
  Fql_matrix_clear(&Li_Fql, para);
  Fql_matrix_clear(&u_Fql, para);

  Fql_matrix_free(&Pi1_y);
  Fql_matrix_free(&yT);
  Fql_matrix_free(&Pi2T_y);
  Fql_matrix_free(&SdT_Pi1_y);
  Fql_matrix_free(&Sub);
  Fql_matrix_free(&Li_Fql);
  Fql_matrix_free(&u_Fql);

  return ret_code;
}

int compute_Li_ui_op(FQ_MATRIX* Li, FQ_MATRIX* ui, const FQL_MATRIX_OP* Pi1,
                     const FQL_MATRIX_OP* Pi2T, const FQL_MATRIX_OP* SdT, const FQL_MATRIX_OP* y,
                     const QRUOV_params* para){

  int ret_code, ret = 0;

  FQL_MATRIX_OP Pi1_y = {.data = NULL};
  FQL_MATRIX_OP yT = {.data = NULL};
  FQL_MATRIX_OP Pi2T_y = {.data = NULL};
  FQL_MATRIX_OP SdT_Pi1_y = {.data = NULL};
  FQL_MATRIX_OP Sub = {.data = NULL};
  FQL_MATRIX_OP Li_Fql = {.data = NULL};
  FQL_MATRIX_OP u_Fql = {.data = NULL};

  ret = Fql_matrix_init_op(&Pi1_y, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-0; goto end; }
  Fql_matrix_clear_op(&Pi1_y, para);

  ret = Fql_matrix_init_op(&yT, 1, para->V, 0, 0, para);
  if(ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_clear_op(&yT, para);
  for(int i=0; i<y->size; i++){
    yT.data[i] = y->data[i];
  }

  ret = Fql_matrix_init_op(&Pi2T_y, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-20; goto end; }
  Fql_matrix_clear_op(&Pi2T_y, para);

  ret = Fql_matrix_init_op(&SdT_Pi1_y, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-30; goto end; }
  Fql_matrix_clear_op(&SdT_Pi1_y, para);

  ret = Fql_matrix_init_op(&Sub, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-40; goto end; }
  Fql_matrix_clear_op(&Sub, para);

  ret = Fql_matrix_init_op(&Li_Fql, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-50; goto end; }
  Fql_matrix_clear_op(&Li_Fql, para);

  ret = Fql_matrix_init_op(&u_Fql, 1, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-60; goto end; }
  Fql_matrix_clear_op(&u_Fql, para);

  // compute
  ret = Fql_matrix_mul_op(&Pi1_y, Pi1, y, para); // Pi1_y := Pi1 * y
  if(ret!=0){ ret_code = ret-70; goto end; }

  ret = Fql_matrix_mul_op(&u_Fql, &yT, &Pi1_y, para); // u_Fql := yT * Pi1_y
  if(ret!=0){ ret_code = ret-110; goto end; }

  // u: Fql -> Fq
  // ui     FQ_MATRIX: 1 * 1
  int coeff_index_perm_0 = 0;
  ret = Fql_index_permute(&coeff_index_perm_0, 0, para);
  if(ret!=0){ ret_code = ret-150; goto end; }

  Fql2Fq_op(&ui->data[0], &(u_Fql.data[0]), coeff_index_perm_0, para);

  ret = Fql_matrix_mul_op(&Pi2T_y, Pi2T, y, para); // Pi2T_y := Pi2T * y
  if(ret!=0){ ret_code = ret-160; goto end; }

  // SdT_Pi1_y := S'T * PiT1 * y (Pi1T=Pi1 (P1 is symmetric) )
  ret = Fql_matrix_mul_op(&SdT_Pi1_y, SdT, &Pi1_y, para);
  if(ret!=0){ ret_code = ret-200; goto end; }

  ret = Fql_matrix_sub_op(&Sub, &Pi2T_y, &SdT_Pi1_y, para); // Sub := Pi2T*y - Sd'T*Pi1T*y
  if(ret!=0){ ret_code = ret-240; goto end; }

  ret = Fql_matrix_add_op(&Li_Fql, &Sub, &Sub, para); // Li_Fql = 2 * Sub
  if(ret!=0){ ret_code = ret-250; goto end; }

  // L: Fql -> Fq
  int j = 0;
  int k = 0;
  int coeff_index_perm_k = 0;

#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(j, k, coeff_index_perm_k) shared(Li, Li_Fql, ret_code, para)
#endif
for(j=0; j<para->M; j++){
    for(k=0; k<para->l; k++){

      int ret1 = Fql_index_permute(&coeff_index_perm_k, k, para);
      if (ret1!=0){
        ret_code = ret1-260;
#ifndef QRUOV_USE_MULTI_THREAD
        break;
#endif
        goto end_for;
      }

    Fql2Fq_op(&(Li->data[INDEX(para->l * j + k, 0, Li->col)]),
                 &(Li_Fql.data[INDEX(j, 0, Li_Fql.col)]),
                 coeff_index_perm_k, para);
    }
    if(ret_code!=0){
#ifndef QRUOV_USE_MULTI_THREAD
        break;
#endif
        goto end_for;
    }

    end_for: // for multi thread
    j=j; // dummy
  }

end:
  Fql_matrix_clear_op(&Pi1_y, para);
  Fql_matrix_clear_op(&yT, para);
  Fql_matrix_clear_op(&Pi2T_y, para);
  Fql_matrix_clear_op(&SdT_Pi1_y, para);
  Fql_matrix_clear_op(&Sub, para);
  Fql_matrix_clear_op(&Li_Fql, para);
  Fql_matrix_clear_op(&u_Fql, para);

  Fql_matrix_free_op(&Pi1_y);
  Fql_matrix_free_op(&yT);
  Fql_matrix_free_op(&Pi2T_y);
  Fql_matrix_free_op(&SdT_Pi1_y);
  Fql_matrix_free_op(&Sub);
  Fql_matrix_free_op(&Li_Fql);
  Fql_matrix_free_op(&u_Fql);

  return ret_code;
}

int compute_Li_ui_op2(FQ_MATRIX* Li, FQ_MATRIX* ui, const FQL_MATRIX* Pi1, const FQL_MATRIX* Pi2T,
                  const FQL_MATRIX* SdT, const FQL_MATRIX* y, const QRUOV_params* para){

  int ret_code, ret = 0;

  FQL_MATRIX Pi1_y = {.data = NULL};
  FQL_MATRIX yT = {.data = NULL};
  FQL_MATRIX Pi2T_y = {.data = NULL};
  FQL_MATRIX SdT_Pi1_y = {.data = NULL};
  FQL_MATRIX Sub = {.data = NULL};
  FQL_MATRIX Li_Fql = {.data = NULL};
  FQL_MATRIX u_Fql = {.data = NULL};

  ret = Fql_matrix_init(&Pi1_y, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-0; goto end; }
  Fql_matrix_clear(&Pi1_y, para);

  ret = Fql_matrix_init(&yT, 1, para->V, 0, 0, para);
  if(ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_clear(&yT, para);
  for(int i=0; i<y->size; i++){
    Fql_copy(yT.data[i], y->data[i], para);
  }

  ret = Fql_matrix_init(&Pi2T_y, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-20; goto end; }
  Fql_matrix_clear(&Pi2T_y, para);

  ret = Fql_matrix_init(&SdT_Pi1_y, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-30; goto end; }
  Fql_matrix_clear(&SdT_Pi1_y, para);

  ret = Fql_matrix_init(&Sub, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-40; goto end; }
  Fql_matrix_clear(&Sub, para);

  ret = Fql_matrix_init(&Li_Fql, para->M, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-50; goto end; }
  Fql_matrix_clear(&Li_Fql, para);

  ret = Fql_matrix_init(&u_Fql, 1, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-60; goto end; }
  Fql_matrix_clear(&u_Fql, para);

  // compute
  ret = Fql_matrix_mul_op2(&Pi1_y, Pi1, y, para); // Pi1_y := Pi1 * y
  if(ret!=0){ ret_code = ret-70; goto end; }

  ret = Fql_matrix_mul_op2(&u_Fql, &yT, &Pi1_y, para); // u_Fql := yT * Pi1_y
  if(ret!=0){ ret_code = ret-110; goto end; }

  // u: Fql -> Fq
  // ui     FQ_MATRIX: 1 * 1
  int coeff_index_perm_0 = 0;
  ret = Fql_index_permute_op2(&coeff_index_perm_0, 0, para);
  if(ret!=0){ ret_code = ret-150; goto end; }

  Fql2Fq(&ui->data[0], u_Fql.data[0], coeff_index_perm_0, para);

  ret = Fql_matrix_mul_op2(&Pi2T_y, Pi2T, y, para); // Pi2T_y := Pi2T * y
  if(ret!=0){ ret_code = ret-160; goto end; }

  // SdT_Pi1_y := S'T * PiT1 * y (Pi1T=Pi1 (P1 is symmetric) )
  ret = Fql_matrix_mul_op2(&SdT_Pi1_y, SdT, &Pi1_y, para);
  if(ret!=0){ ret_code = ret-200; goto end; }

  ret = Fql_matrix_sub_op2(&Sub, &Pi2T_y, &SdT_Pi1_y, para); // Sub := Pi2T*y - Sd'T*Pi1T*y
  if(ret!=0){ ret_code = ret-240; goto end; }

  ret = Fql_matrix_add_op2(&Li_Fql, &Sub, &Sub, para); // Li_Fql = 2 * Sub
  if(ret!=0){ ret_code = ret-250; goto end; }

  // L: Fql -> Fq
  int j = 0;
  int k = 0;
  int coeff_index_perm_k = 0;

#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(j, k, coeff_index_perm_k) shared(Li, Li_Fql, ret_code, para)
#endif
for(j=0; j<para->M; j++){
    for(k=0; k<para->l; k++){

      int ret1 = Fql_index_permute_op2(&coeff_index_perm_k, k, para);
      if (ret1!=0){
        ret_code = ret1-260;
#ifndef QRUOV_USE_MULTI_THREAD
        break;
#endif
        goto end_for;
      }

    Fql2Fq(&(Li->data[INDEX(para->l * j + k, 0, Li->col)]),
              Li_Fql.data[INDEX(j, 0, Li_Fql.col)],
              coeff_index_perm_k, para);
    }
    if(ret_code!=0){
#ifndef QRUOV_USE_MULTI_THREAD
        break;
#endif
        goto end_for;
    }

    end_for: // for multi thread
    j=j; // dummy
  }

end:
  Fql_matrix_clear(&Pi1_y, para);
  Fql_matrix_clear(&yT, para);
  Fql_matrix_clear(&Pi2T_y, para);
  Fql_matrix_clear(&SdT_Pi1_y, para);
  Fql_matrix_clear(&Sub, para);
  Fql_matrix_clear(&Li_Fql, para);
  Fql_matrix_clear(&u_Fql, para);

  Fql_matrix_free(&Pi1_y);
  Fql_matrix_free(&yT);
  Fql_matrix_free(&Pi2T_y);
  Fql_matrix_free(&SdT_Pi1_y);
  Fql_matrix_free(&Sub);
  Fql_matrix_free(&Li_Fql);
  Fql_matrix_free(&u_Fql);

  return ret_code;
}

int compute_s(FQL_MATRIX* s, const FQL_MATRIX* y, const FQL_MATRIX* x, const FQL_MATRIX* Sd,
              const QRUOV_params* para){

  int ret_code = 0, ret = 0;

  FQL_MATRIX Sd_oil = {.data = NULL};
  FQL_MATRIX tmp_V = {.data = NULL};

  ret = Fql_matrix_init(&Sd_oil, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-0; goto end; }
  Fql_matrix_clear(&Sd_oil, para);

  ret = Fql_matrix_init(&tmp_V, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_clear(&tmp_V, para);

  ret = Fql_matrix_mul(&Sd_oil, Sd, x, para);
  if(ret!=0){ ret_code = ret-20; goto end; }

  ret = Fql_matrix_sub(&tmp_V, y, &Sd_oil, para);
  if(ret!=0){ ret_code = ret-60; goto end; }

  for(int i=0; i<para->V; i++){
    Fql_copy(s->data[i], tmp_V.data[i], para);
  }
  for(int i=0; i<para->M; i++){
    Fql_copy(s->data[para->V+i], x->data[i], para);
  }

end:
  Fql_matrix_clear(&Sd_oil, para);
  Fql_matrix_free(&Sd_oil);
  Fql_matrix_free(&tmp_V);

  return ret_code;
}

int compute_s_op2(FQL_MATRIX* s, const FQL_MATRIX* y, const FQL_MATRIX* x, const FQL_MATRIX* Sd,
              const QRUOV_params* para){

  int ret_code = 0, ret = 0;

  FQL_MATRIX Sd_oil = {.data = NULL};
  FQL_MATRIX tmp_V = {.data = NULL};

  ret = Fql_matrix_init(&Sd_oil, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-0; goto end; }
  Fql_matrix_clear(&Sd_oil, para);

  ret = Fql_matrix_init(&tmp_V, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_clear(&tmp_V, para);

  ret = Fql_matrix_mul_op2(&Sd_oil, Sd, x, para);
  if(ret!=0){ ret_code = ret-20; goto end; }

  ret = Fql_matrix_sub_op2(&tmp_V, y, &Sd_oil, para);
  if(ret!=0){ ret_code = ret-60; goto end; }

  for(int i=0; i<para->V; i++){
    Fql_copy_op2(s->data[i], tmp_V.data[i], para);
  }
  for(int i=0; i<para->M; i++){
    Fql_copy_op2(s->data[para->V+i], x->data[i], para);
  }

end:
  Fql_matrix_clear(&Sd_oil, para);
  Fql_matrix_free(&Sd_oil);
  Fql_matrix_free(&tmp_V);

  return ret_code;
}

int compute_s_op(FQL_MATRIX_OP* s, const FQL_MATRIX_OP* y, const FQL_MATRIX_OP* x,
                 const FQL_MATRIX_OP* Sd, const QRUOV_params* para){

  int ret_code = 0, ret = 0;

  FQL_MATRIX_OP Sd_oil = {.data = NULL};
  FQL_MATRIX_OP tmp_V = {.data = NULL};

  ret = Fql_matrix_init_op(&Sd_oil, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-0; goto end; }
  Fql_matrix_clear_op(&Sd_oil, para);

  ret = Fql_matrix_init_op(&tmp_V, para->V, 1, 0, 0, para);
  if(ret!=0){ ret_code = ret-10; goto end; }
  Fql_matrix_clear_op(&tmp_V, para);

  ret = Fql_matrix_mul_op(&Sd_oil, Sd, x, para);
  if(ret!=0){ ret_code = ret-20; goto end; }

  ret = Fql_matrix_sub_op(&tmp_V, y, &Sd_oil, para);
  if(ret!=0){ ret_code = ret-60; goto end; }

  for(int i=0; i<para->V; i++){
    s->data[i] = tmp_V.data[i];
  }
  for(int i=0; i<para->M; i++){
    s->data[para->V+i] = x->data[i];
  }

end:
  Fql_matrix_clear_op(&Sd_oil, para);
  Fql_matrix_free_op(&Sd_oil);
  Fql_matrix_free_op(&tmp_V);

  return ret_code;
}

int compute_td(FQ_MATRIX* td, const FQL_MATRIX* s, const FQL_MATRIX* P1, const FQL_MATRIX* P2T,
               const FQL_MATRIX* P3, const QRUOV_params* para){

  int ret_code = 0, ret1 = 0;

  FQL_MATRIX s_vinegar = {.data = NULL};
  FQL_MATRIX s_oil = {.data = NULL};
  FQL_MATRIX s_vinegarT = {.data = NULL};
  FQL_MATRIX s_oilT = {.data = NULL};

  ret1 = Fql_matrix_init(&s_vinegar, para->V, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-0; goto end; }
  ret1 = Fql_matrix_init(&s_oil, para->M, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-10; goto end; }

  for (int i=0; i<para->V; i++){
    for (int j=0; j<para->l; j++){
      s_vinegar.data[i][j] = s->data[i][j];
    }
  }
  for (int i=0; i<para->M; i++){
    for (int j=0; j<para->l; j++){
      s_oil.data[i][j] = s->data[(para->V)+i][j];
    }
  }

  ret1 = Fql_matrix_init(&s_vinegarT, para->V, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-20; goto end; }
  ret1 = Fql_matrix_init(&s_oilT, para->M, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-30; goto end; }

  ret1 = Fql_matrix_copy(&s_vinegarT, &s_vinegar, para);
  if (ret1!=0){ ret_code = ret1-40; goto end; }
  Fql_matrix_transpose(&s_vinegarT);

  ret1 = Fql_matrix_copy(&s_oilT, &s_oil, para);
  if (ret1!=0){ ret_code = ret1-50; goto end; }
  Fql_matrix_transpose(&s_oilT);

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(td, s_vinegar, s_oil, P1, P2T, P3, s_vinegarT, \
                                             s_oilT, para)
#endif

  for (i=0; i<para->m; i++){

    int ret2 = 0;

    FQL_MATRIX tmp_vinegar = {.data = NULL};
    FQL_MATRIX tmp_oil = {.data = NULL};
    FQL_MATRIX tmp = {.data = NULL};

    ret2 = Fql_matrix_init(&tmp_vinegar, para->V, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-60; goto end_for; }
    Fql_matrix_clear(&tmp_vinegar, para);

    ret2 = Fql_matrix_init(&tmp_oil, para->M, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-70; goto end_for; }
    Fql_matrix_clear(&tmp_oil, para);

    ret2 = Fql_matrix_init(&tmp, 1, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-80; goto end_for; }
    Fql_matrix_clear(&tmp, para);

    ret2 = Fql_matrix_mul(&tmp_vinegar, &(P1[i]), &s_vinegar, para);
    if (ret2!=0){ ret_code = ret2-90; goto end_for; }

    ret2 = Fql_matrix_mul(&tmp_oil, &(P2T[i]), &s_vinegar, para);
    if (ret2!=0){ ret_code = ret2-130; goto end_for; }

    ret2 = Fql_matrix_add(&tmp_oil, &tmp_oil, &tmp_oil, para);
    if (ret2!=0){ ret_code = ret2-170; goto end_for; }

    ret2 = Fql_matrix_mul_add(&tmp_oil, &(P3[i]), &s_oil, &tmp_oil, para);
    if (ret2!=0){ ret_code = ret2-180; goto end_for; }

    ret2 = Fql_matrix_mul(&tmp, &s_vinegarT, &tmp_vinegar, para);
    if (ret2!=0){ ret_code = ret2-230; goto end_for; }

    ret2 = Fql_matrix_mul_add(&tmp, &s_oilT, &tmp_oil, &tmp, para);
    if (ret2!=0){ ret_code = ret2-270; goto end_for; }

    int coeff_index_perm_0 = 0;
    ret2 = Fql_index_permute(&coeff_index_perm_0, 0, para);
    if (ret2!=0){ ret_code = ret2-320; goto end_for; }

    Fql2Fq(&(td->data[i]), tmp.data[0], coeff_index_perm_0, para);

    end_for:
    Fql_matrix_free(&tmp);
    Fql_matrix_free(&tmp_oil);
    Fql_matrix_free(&tmp_vinegar);
  }

end:
  Fql_matrix_free(&s_oil);
  Fql_matrix_free(&s_vinegar);
  Fql_matrix_free(&s_oilT);
  Fql_matrix_free(&s_vinegarT);

  return ret_code;
}

int compute_td_op(FQ_MATRIX* td, const FQL_MATRIX_OP* s, const FQL_MATRIX_OP* P1,
                  const FQL_MATRIX_OP* P2T, const FQL_MATRIX_OP* P3, const QRUOV_params* para){

  int ret_code = 0, ret1 = 0;

  FQL_MATRIX_OP s_vinegar = {.data = NULL};
  FQL_MATRIX_OP s_oil = {.data = NULL};
  FQL_MATRIX_OP s_vinegarT = {.data = NULL};
  FQL_MATRIX_OP s_oilT = {.data = NULL};

  ret1 = Fql_matrix_init_op(&s_vinegar, para->V, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-0; goto end; }
  ret1 = Fql_matrix_init_op(&s_oil, para->M, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-10; goto end; }

  for (int i=0; i<para->V; i++){
    s_vinegar.data[i] = s->data[i];
  }
  for (int i=0; i<para->M; i++){
    s_oil.data[i] = s->data[(para->V)+i];
  }

  ret1 = Fql_matrix_init_op(&s_vinegarT, para->V, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-20; goto end; }
  ret1 = Fql_matrix_init_op(&s_oilT, para->M, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-30; goto end; }

  ret1 = Fql_matrix_copy_op(&s_vinegarT, &s_vinegar);
  if (ret1!=0){ ret_code = ret1-40; goto end; }
  Fql_matrix_transpose_op(&s_vinegarT);

  ret1 = Fql_matrix_copy_op(&s_oilT, &s_oil);
  if (ret1!=0){ ret_code = ret1-50; goto end; }
  Fql_matrix_transpose_op(&s_oilT);

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(td, s_vinegar, s_oil, P1, P2T, P3, s_vinegarT, \
                                             s_oilT, para)
#endif

  for (i=0; i<para->m; i++){

    int ret2 = 0;

    FQL_MATRIX_OP tmp_vinegar = {.data = NULL};
    FQL_MATRIX_OP tmp_oil = {.data = NULL};
    FQL_MATRIX_OP tmp = {.data = NULL};

    ret2 = Fql_matrix_init_op(&tmp_vinegar, para->V, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-60; goto end_for; }
    Fql_matrix_clear_op(&tmp_vinegar, para);

    ret2 = Fql_matrix_init_op(&tmp_oil, para->M, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-70; goto end_for; }
    Fql_matrix_clear_op(&tmp_oil, para);

    ret2 = Fql_matrix_init_op(&tmp, 1, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-80; goto end_for; }
    Fql_matrix_clear_op(&tmp, para);

    ret2 = Fql_matrix_mul_op(&tmp_vinegar, &(P1[i]), &s_vinegar, para);
    if (ret2!=0){ ret_code = ret2-90; goto end_for; }

    ret2 = Fql_matrix_mul_op(&tmp_oil, &(P2T[i]), &s_vinegar, para);
    if (ret2!=0){ ret_code = ret2-130; goto end_for; }

    ret2 = Fql_matrix_add_op(&tmp_oil, &tmp_oil, &tmp_oil, para);
    if (ret2!=0){ ret_code = ret2-170; goto end_for; }

    ret2 = Fql_matrix_mul_add_op(&tmp_oil, &(P3[i]), &s_oil, &tmp_oil, para);
    if (ret2!=0){ ret_code = ret2-180; goto end_for; }

    ret2 = Fql_matrix_mul_op(&tmp, &s_vinegarT, &tmp_vinegar, para);
    if (ret2!=0){ ret_code = ret2-230; goto end_for; }

    ret2 = Fql_matrix_mul_add_op(&tmp, &s_oilT, &tmp_oil, &tmp, para);
    if (ret2!=0){ ret_code = ret2-270; goto end_for; }

    int coeff_index_perm_0 = 0;
    ret2 = Fql_index_permute(&coeff_index_perm_0, 0, para);
    if (ret2!=0){ ret_code = ret2-320; goto end_for; }

    Fql2Fq_op(&(td->data[i]), &(tmp.data[0]), coeff_index_perm_0, para);

    end_for:
    Fql_matrix_free_op(&tmp);
    Fql_matrix_free_op(&tmp_oil);
    Fql_matrix_free_op(&tmp_vinegar);
  }

end:
  Fql_matrix_free_op(&s_oil);
  Fql_matrix_free_op(&s_vinegar);
  Fql_matrix_free_op(&s_oilT);
  Fql_matrix_free_op(&s_vinegarT);

  return ret_code;
}

int compute_td_op2(FQ_MATRIX* td, const FQL_MATRIX* s, const FQL_MATRIX* P1, const FQL_MATRIX* P2T,
               const FQL_MATRIX* P3, const QRUOV_params* para){

  int ret_code = 0, ret1 = 0;

  FQL_MATRIX s_vinegar = {.data = NULL};
  FQL_MATRIX s_oil = {.data = NULL};
  FQL_MATRIX s_vinegarT = {.data = NULL};
  FQL_MATRIX s_oilT = {.data = NULL};

  ret1 = Fql_matrix_init(&s_vinegar, para->V, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-0; goto end; }
  ret1 = Fql_matrix_init(&s_oil, para->M, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-10; goto end; }

  for (int i=0; i<para->V; i++){
    for (int j=0; j<para->l; j++){
      s_vinegar.data[i][j] = s->data[i][j];
    }
  }
  for (int i=0; i<para->M; i++){
    for (int j=0; j<para->l; j++){
      s_oil.data[i][j] = s->data[(para->V)+i][j];
    }
  }

  ret1 = Fql_matrix_init(&s_vinegarT, para->V, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-20; goto end; }
  ret1 = Fql_matrix_init(&s_oilT, para->M, 1, 0, 0, para);
  if (ret1!=0){ ret_code = ret1-30; goto end; }

  ret1 = Fql_matrix_copy_op2(&s_vinegarT, &s_vinegar, para);
  if (ret1!=0){ ret_code = ret1-40; goto end; }
  Fql_matrix_transpose(&s_vinegarT);

  ret1 = Fql_matrix_copy_op2(&s_oilT, &s_oil, para);
  if (ret1!=0){ ret_code = ret1-50; goto end; }
  Fql_matrix_transpose(&s_oilT);

  int i;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(td, s_vinegar, s_oil, P1, P2T, P3, s_vinegarT, \
                                             s_oilT, para)
#endif

  for (i=0; i<para->m; i++){

    int ret2 = 0;

    FQL_MATRIX tmp_vinegar = {.data = NULL};
    FQL_MATRIX tmp_oil = {.data = NULL};
    FQL_MATRIX tmp = {.data = NULL};

    ret2 = Fql_matrix_init(&tmp_vinegar, para->V, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-60; goto end_for; }
    Fql_matrix_clear(&tmp_vinegar, para);

    ret2 = Fql_matrix_init(&tmp_oil, para->M, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-70; goto end_for; }
    Fql_matrix_clear(&tmp_oil, para);

    ret2 = Fql_matrix_init(&tmp, 1, 1, 0, 0, para);
    if (ret2!=0){ ret_code = ret2-80; goto end_for; }
    Fql_matrix_clear(&tmp, para);

    ret2 = Fql_matrix_mul_op2(&tmp_vinegar, &(P1[i]), &s_vinegar, para);
    if (ret2!=0){ ret_code = ret2-90; goto end_for; }

    ret2 = Fql_matrix_mul_op2(&tmp_oil, &(P2T[i]), &s_vinegar, para);
    if (ret2!=0){ ret_code = ret2-130; goto end_for; }

    ret2 = Fql_matrix_add_op2(&tmp_oil, &tmp_oil, &tmp_oil, para);
    if (ret2!=0){ ret_code = ret2-170; goto end_for; }

    ret2 = Fql_matrix_mul_add_op2(&tmp_oil, &(P3[i]), &s_oil, &tmp_oil, para);
    if (ret2!=0){ ret_code = ret2-180; goto end_for; }

    ret2 = Fql_matrix_mul_op2(&tmp, &s_vinegarT, &tmp_vinegar, para);
    if (ret2!=0){ ret_code = ret2-230; goto end_for; }

    ret2 = Fql_matrix_mul_add_op2(&tmp, &s_oilT, &tmp_oil, &tmp, para);
    if (ret2!=0){ ret_code = ret2-270; goto end_for; }

    int coeff_index_perm_0 = 0;
    ret2 = Fql_index_permute_op2(&coeff_index_perm_0, 0, para);
    if (ret2!=0){ ret_code = ret2-320; goto end_for; }

    Fql2Fq(&(td->data[i]), tmp.data[0], coeff_index_perm_0, para);

    end_for:
    Fql_matrix_free(&tmp);
    Fql_matrix_free(&tmp_oil);
    Fql_matrix_free(&tmp_vinegar);
  }

end:
  Fql_matrix_free(&s_oil);
  Fql_matrix_free(&s_vinegar);
  Fql_matrix_free(&s_oilT);
  Fql_matrix_free(&s_vinegarT);

  return ret_code;
}

int LU_decompose(int* P, FQ_MATRIX* L, FQ_MATRIX* U, int* rank, int* first_col_index,
                 FQ_MATRIX* A_org, const QRUOV_params* para){

  int ret_code= 0, ret1 = 0;
  *rank = 0;

  FQ_MATRIX A = {.data = NULL};
  ret1 = Fq_matrix_init(&A, A_org->row, A_org->col);
  if (ret1!=0){ ret_code = ret1; goto end; }

  Fq_matrix_clear(&A);
  memcpy(A.data, A_org->data, sizeof(Fq) * A.size);

  // init L=I, U=O
  Fq_matrix_clear(L);
  for(int i=0; i<(L->row); i++){
    for(int j=0; j<(L->col); j++){
      if (i==j) L->data[INDEX(i, j, L->col)] = 1;
    }
  }
  Fq_matrix_clear(U);

  {
    int i = 0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(P, first_col_index, para)
#endif
    for(i=0; i<para->m; i++){
      P[i] = i;
      first_col_index[i] = -1;
    }
  }

  // row swap
  int current_col = -1;
  for(int i=0; i<para->m; i++){
    current_col++;
    if(current_col>=para->m){
      break;
    }

    int nonzero_row = i;
    int finish = 0;
    while(A.data[INDEX(nonzero_row, current_col, A.col)]==0){
      nonzero_row++;
      if(nonzero_row>=para->m){
        current_col++;
        if(current_col>=para->m){
          finish = 1;
          break;
        }
        nonzero_row = i;
      }
    }
    if(finish==1){
      break;
    }

    if(nonzero_row!=i){
      int ret = row_swap(&A, P, i, nonzero_row, para);
      if(ret!=0){ ret_code = ret-10; break; }
    }
    (*rank)++;

    first_col_index[i] = current_col;

    // LU decompose
    Fq inv = para->Fq_inv_table[A.data[INDEX(i, current_col, A.col)]];
    A.data[INDEX(i, i, A.col)] = A.data[INDEX(i, current_col, A.col)];

    for(int k=current_col+1; k<para->m; k++){
      A.data[INDEX(i, k, A.col)] = Fq_mul(A.data[INDEX(i, k, A.col)], inv, para);
    }

    for(int j_row=i+1; j_row<para->m; j_row++){
      Fq mul = A.data[INDEX(j_row, current_col, A.col)];
      A.data[INDEX(j_row, i, A.col)] = mul;
      for(int k=current_col+1; k<para->m; k++){
        A.data[INDEX(j_row, k, A.col)]
          = Fq_sub(A.data[INDEX(j_row, k, A.col)],
                   Fq_mul(mul, A.data[INDEX(i, k, A.col)], para),
                   para);
      }
    }
  }
  if(ret_code!=0){ goto end; }

  {
    int i=0, j_L=0, j_u=0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j_L, j_u) shared(rank, A, L, U, para)
#endif
    for(i=0; i<*rank; i++){

      for(j_L=i; j_L<para->m; j_L++){
        L->data[INDEX(j_L, i, L->col)] = A.data[INDEX(j_L, i, A.col)];
      }
      int col = first_col_index[i];
      U->data[INDEX(i, col, U->col)] = 1;
      for(j_u=col+1; j_u<para->m; j_u++){
        U->data[INDEX(i, j_u, U->col)] = A.data[INDEX(i, j_u, A.col)];
      }
    }
  }

end:
  Fq_matrix_clear(&A);
  Fq_matrix_free(&A);

  return ret_code;
}

int LU_decompose_op2(int* P, FQ_MATRIX* L, FQ_MATRIX* U, int* rank, int* first_col_index,
                 FQ_MATRIX* A_org, const QRUOV_params* para){

  int ret_code= 0, ret1 = 0;
  *rank = 0;

  FQ_MATRIX A = {.data = NULL};
  ret1 = Fq_matrix_init(&A, A_org->row, A_org->col);
  if (ret1!=0){ ret_code = ret1; goto end; }

  Fq_matrix_clear(&A);
  memcpy(A.data, A_org->data, sizeof(Fq) * A.size);

  // init L=I, U=O
  Fq_matrix_clear(L);
  for(int i=0; i<(L->row); i++){
    for(int j=0; j<(L->col); j++){
      if (i==j) L->data[INDEX(i, j, L->col)] = 1;
    }
  }
  Fq_matrix_clear(U);

  {
    int i = 0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i) shared(P, first_col_index, para)
#endif
    for(i=0; i<para->m; i++){
      P[i] = i;
      first_col_index[i] = -1;
    }
  }

  // row swap
  int current_col = -1;
  for(int i=0; i<para->m; i++){
    current_col++;
    if(current_col>=para->m){
      break;
    }

    int nonzero_row = i;
    int finish = 0;
    while(A.data[INDEX(nonzero_row, current_col, A.col)]==0){
      nonzero_row++;
      if(nonzero_row>=para->m){
        current_col++;
        if(current_col>=para->m){
          finish = 1;
          break;
        }
        nonzero_row = i;
      }
    }
    if(finish==1){
      break;
    }

    if(nonzero_row!=i){
      int ret = row_swap(&A, P, i, nonzero_row, para);
      if(ret!=0){ ret_code = ret-10; break; }
    }
    (*rank)++;

    first_col_index[i] = current_col;

    // LU decompose
    Fq inv = para->Fq_inv_table[A.data[INDEX(i, current_col, A.col)]];
    A.data[INDEX(i, i, A.col)] = A.data[INDEX(i, current_col, A.col)];

    for(int k=current_col+1; k<para->m; k++){
      A.data[INDEX(i, k, A.col)] = Fq_mul_op2(A.data[INDEX(i, k, A.col)], inv, para);
    }

    for(int j_row=i+1; j_row<para->m; j_row++){
      Fq mul = A.data[INDEX(j_row, current_col, A.col)];
      A.data[INDEX(j_row, i, A.col)] = mul;
      for(int k=current_col+1; k<para->m; k++){
        A.data[INDEX(j_row, k, A.col)]
          = Fq_sub_op2(A.data[INDEX(j_row, k, A.col)],
                   Fq_mul_op2(mul, A.data[INDEX(i, k, A.col)], para),
                   para);
      }
    }
  }
  if(ret_code!=0){ goto end; }

  {
    int i=0, j_L=0, j_u=0;
#ifdef QRUOV_USE_MULTI_THREAD
  #pragma omp parallel for private(i, j_L, j_u) shared(rank, A, L, U, para)
#endif
    for(i=0; i<*rank; i++){

      for(j_L=i; j_L<para->m; j_L++){
        L->data[INDEX(j_L, i, L->col)] = A.data[INDEX(j_L, i, A.col)];
      }
      int col = first_col_index[i];
      U->data[INDEX(i, col, U->col)] = 1;
      for(j_u=col+1; j_u<para->m; j_u++){
        U->data[INDEX(i, j_u, U->col)] = A.data[INDEX(i, j_u, A.col)];
      }
    }
  }

end:
  Fq_matrix_clear(&A);
  Fq_matrix_free(&A);

  return ret_code;
}

int row_swap(FQ_MATRIX* A, int* P, const int i_row, const int j_row, const QRUOV_params* para){

  if(!(i_row<para->m && j_row<para->m)){
#ifdef DEBUG
  fprintf(stderr, "[row_swap] i_row and j_row must be < para->m.\n");
#endif
    return -1;
  }

  Fq* tmp_row_A = NULL;
  tmp_row_A = (Fq*)malloc(sizeof(Fq) * para->m);
  if (tmp_row_A == NULL){
#ifdef DEBUG
    fprintf(stderr, "[row_swap] malloc failed.\n");
#endif
    return -2;
  }
  memset(tmp_row_A, 0, para->m);

  int tmp_P = 0;

  memcpy(tmp_row_A, &(A->data[A->col * i_row]), sizeof(Fq) * A->col);
  tmp_P = P[i_row];

  memcpy(&(A->data[A->col * i_row]), &(A->data[A->col * j_row]), sizeof(Fq) * A->col);
  P[i_row] = P[j_row];

  memcpy(&(A->data[A->col * j_row]), tmp_row_A, sizeof(Fq) * A->col);
  P[j_row] = tmp_P;

  if (tmp_row_A!=NULL) free(tmp_row_A);

  return 0;
}

int L_inverse(FQ_MATRIX* L_inv, const FQ_MATRIX* L, const int rank, const QRUOV_params* para){

  int is_zero_matrix = 1;
  for(int i=0; i<(L->size); i++){
    if(L->data[i] != 0){
      is_zero_matrix = 0;
      break;
    }
  }

  if(is_zero_matrix){
#ifdef DEBUG
  fprintf(stderr, "[L_inverse] L must not be a zero matrix.\n");
#endif
    return -1;
  }

  for(int i=0; i<para->m; i++){
    L_inv->data[INDEX(i, i, L_inv->col)] = 1;
  }

  for(int i=0; i<rank; i++){
    Fq inv = para->Fq_inv_table[L->data[INDEX(i, i, L->col)]];
    for(int k=0; k<=i; k++){
      L_inv->data[INDEX(i, k, L_inv->col)]
        = Fq_mul(L_inv->data[INDEX(i, k, L_inv->col)], inv, para);
    }

    for(int j=i+1; j<para->m; j++){
      for(int k=0; k<=i; k++){
        Fq mul = Fq_mul(L_inv->data[INDEX(i, k, L_inv->col)],
                        L->data[INDEX(j, i, L->col)], para);
        L_inv->data[INDEX(j, k, L_inv->col)] =
          Fq_sub(L_inv->data[INDEX(j, k, L_inv->col)], mul, para);
      }
    }
  }

  return 0;
}

int L_inverse_op2(FQ_MATRIX* L_inv, const FQ_MATRIX* L, const int rank, const QRUOV_params* para){

  int is_zero_matrix = 1;
  for(int i=0; i<(L->size); i++){
    if(L->data[i] != 0){
      is_zero_matrix = 0;
      break;
    }
  }

  if(is_zero_matrix){
#ifdef DEBUG
  fprintf(stderr, "[L_inverse] L must not be a zero matrix.\n");
#endif
    return -1;
  }

  for(int i=0; i<para->m; i++){
    L_inv->data[INDEX(i, i, L_inv->col)] = 1;
  }

  for(int i=0; i<rank; i++){
    Fq inv = para->Fq_inv_table[L->data[INDEX(i, i, L->col)]];
    for(int k=0; k<=i; k++){
      L_inv->data[INDEX(i, k, L_inv->col)]
        = Fq_mul_op2(L_inv->data[INDEX(i, k, L_inv->col)], inv, para);
    }

    for(int j=i+1; j<para->m; j++){
      for(int k=0; k<=i; k++){
        Fq mul = Fq_mul_op2(L_inv->data[INDEX(i, k, L_inv->col)],
                        L->data[INDEX(j, i, L->col)], para);
        L_inv->data[INDEX(j, k, L_inv->col)] =
          Fq_sub_op2(L_inv->data[INDEX(j, k, L_inv->col)], mul, para);
      }
    }
  }

  return 0;
}

int consistent(int* result, FQ_MATRIX* L_inv, int* has_L_inv, const int* P, const FQ_MATRIX* L,
               const int rank, const FQ_MATRIX* B, const QRUOV_params* para){

  if(*has_L_inv!=1){
    int ret = L_inverse(L_inv, L, rank, para);
    if(ret!=0){
      return ret;
    }
    *has_L_inv = 1;
  }

  for(int i=rank; i<para->m; i++){
    Fq acc = 0;
    for(int j=0; j<para->m; j++){
      acc = Fq_add(acc, Fq_mul(L_inv->data[INDEX(i, j, L_inv->col)], B->data[P[j]], para), para);
    }
    if(acc!=0){
      *result = 0;
      return 0;
    }
  }
  *result = 1;

  return 0;
}

int consistent_op2(int* result, FQ_MATRIX* L_inv, int* has_L_inv, const int* P, const FQ_MATRIX* L,
               const int rank, const FQ_MATRIX* B, const QRUOV_params* para){

  if(*has_L_inv!=1){
    int ret = L_inverse_op2(L_inv, L, rank, para);
    if(ret!=0){
      return ret;
    }
    *has_L_inv = 1;
  }

  for(int i=rank; i<para->m; i++){
    Fq acc = 0;
    for(int j=0; j<para->m; j++){
      acc = Fq_add_op2(acc, Fq_mul_op2(L_inv->data[INDEX(i, j, L_inv->col)], B->data[P[j]], para), para);
    }
    if(acc!=0){
      *result = 0;
      return 0;
    }
  }
  *result = 1;

  return 0;
}

int sample_a_solution(FQ_MATRIX* X, const int* P, const FQ_MATRIX* L, const FQ_MATRIX* U,
                      const int rank, const int* first_col_index, const FQ_MATRIX* B,
                      const unsigned char* x_seed, const QRUOV_params* para){

  int ret = 0;
  int tau = para->tau_m;
  int len = para->m;
  Fq vec[len];

  if (rank<para->m){
    void* ctx = NULL;

    if (para->prg_shake==1){
      ret = SHAKE_init((SHAKE_CTX**)&ctx, para->SHAKE_len*8);
    }else{
      ret = AES_init((AES_CTX**)&ctx);
    }
    if (ret!=0) { ret-=10; goto m_end; }

    if (para->prg_shake==1){
      ret = SHAKE_update((SHAKE_CTX*)ctx, x_seed, para->seed_len);
    }else{
      ret = AES_update((AES_CTX*)ctx, x_seed, para->AES_len*8);
    }
    if (ret!=0) { ret-=20; goto m_end; }

    if (para->prg_shake==1){
      ret = rej_samp_PRG_SHAKE((SHAKE_CTX*)ctx, vec, 0, tau, len, para);
    }else{
      ret = rej_samp_PRG_AES((AES_CTX*)ctx, vec, 0, tau, len, para);
    }
    if (ret!=0) { ret-=30; goto m_end; }

    m_end:
    if (para->prg_shake==1){
      SHAKE_free((SHAKE_CTX**)&ctx);
    }else{
      AES_free((AES_CTX**)&ctx);
    }

    if (ret!=0) return ret;
  }

  // Ax = b
  // PA = LU, A=P^-1 LU
  // P^-1 LUx = b, LUx = Pb
  // c := Ux, Lc = Pb, solve c

  Fq* c = NULL;
  c = (Fq*)malloc(sizeof(Fq) * para->m);
  if (c == NULL){
#ifdef DEBUG
    fprintf(stderr, "[sample_a_solution] malloc failed.\n");
#endif
    return -1;
  }
  memset(c, 0, para->m);

  int rand_index = 0;
  for(int i=0; i<rank; i++){
    Fq lc_acc = 0;
    for(int j=0; j<i; j++){
      lc_acc = Fq_add(lc_acc, Fq_mul(L->data[INDEX(i, j, L->col)], c[j], para), para);
    }
    Fq inv = para->Fq_inv_table[L->data[INDEX(i, i, L->col)]];
    c[i] = Fq_mul(Fq_sub(B->data[P[i]], lc_acc, para), inv, para);
  }

  // Ux = c, solve x
  for(int i=para->m-1; i>=0; i--){

    int j = -1;
    for(int row_idx=0; row_idx<rank; row_idx++){
      if(first_col_index[row_idx]==i){
        j=row_idx;
        break;
      }
    }

    if(j!=-1){
      Fq ux_acc = 0;
      for(int k=i+1; k<para->m; k++){
        ux_acc = Fq_add(ux_acc, Fq_mul(U->data[INDEX(j, k , U->col)], X->data[k], para), para);
      }
      X->data[i] = Fq_sub(c[j], ux_acc, para);
    }else{
      X->data[i] = vec[rand_index++];
    }
  }

  if (c!=NULL) free(c);

  return 0;
}

int sample_a_solution_op2(FQ_MATRIX* X, const int* P, const FQ_MATRIX* L, const FQ_MATRIX* U,
                      const int rank, const int* first_col_index, const FQ_MATRIX* B,
                      const unsigned char* x_seed, const QRUOV_params* para){

  int ret = 0;
  int tau = para->tau_m;
  int len = para->m;
  Fq vec[len];

  if (rank<para->m){
    void* ctx = NULL;

    if (para->prg_shake==1){
      ret = SHAKE_init((SHAKE_CTX**)&ctx, para->SHAKE_len*8);
    }else{
      ret = AES_init((AES_CTX**)&ctx);
    }
    if (ret!=0) { ret-=10; goto m_end; }

    if (para->prg_shake==1){
      ret = SHAKE_update((SHAKE_CTX*)ctx, x_seed, para->seed_len);
    }else{
      ret = AES_update((AES_CTX*)ctx, x_seed, para->AES_len*8);
    }
    if (ret!=0) { ret-=20; goto m_end; }

    if (para->prg_shake==1){
      ret = rej_samp_PRG_SHAKE((SHAKE_CTX*)ctx, vec, 0, tau, len, para);
    }else{
      ret = rej_samp_PRG_AES((AES_CTX*)ctx, vec, 0, tau, len, para);
    }
    if (ret!=0) { ret-=30; goto m_end; }

    m_end:
    if (para->prg_shake==1){
      SHAKE_free((SHAKE_CTX**)&ctx);
    }else{
      AES_free((AES_CTX**)&ctx);
    }

    if (ret!=0) return ret;
  }

  // Ax = b
  // PA = LU, A=P^-1 LU
  // P^-1 LUx = b, LUx = Pb
  // c := Ux, Lc = Pb, solve c

  Fq* c = NULL;
  c = (Fq*)malloc(sizeof(Fq) * para->m);
  if (c == NULL){
#ifdef DEBUG
    fprintf(stderr, "[sample_a_solution] malloc failed.\n");
#endif
    return -1;
  }
  memset(c, 0, para->m);

  int rand_index = 0;
  for(int i=0; i<rank; i++){
    Fq lc_acc = 0;
    for(int j=0; j<i; j++){
      lc_acc = Fq_add_op2(lc_acc, Fq_mul_op2(L->data[INDEX(i, j, L->col)], c[j], para), para);
    }
    Fq inv = para->Fq_inv_table[L->data[INDEX(i, i, L->col)]];
    c[i] = Fq_mul_op2(Fq_sub_op2(B->data[P[i]], lc_acc, para), inv, para);
  }

  // Ux = c, solve x
  for(int i=para->m-1; i>=0; i--){

    int j = -1;
    for(int row_idx=0; row_idx<rank; row_idx++){
      if(first_col_index[row_idx]==i){
        j=row_idx;
        break;
      }
    }

    if(j!=-1){
      Fq ux_acc = 0;
      for(int k=i+1; k<para->m; k++){
        ux_acc = Fq_add_op2(ux_acc, Fq_mul_op2(U->data[INDEX(j, k , U->col)], X->data[k], para), para);
      }
      X->data[i] = Fq_sub_op2(c[j], ux_acc, para);
    }else{
      X->data[i] = vec[rand_index++];
    }
  }

  if (c!=NULL) free(c);

  return 0;
}