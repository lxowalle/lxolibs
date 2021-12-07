#include <stdio.h>
#include "kalman.h"

/**
 * v
 * a
 * p
 * 
 * 
 * __________________________
*/
typedef double (*Fk_func_t)(double Xlast, void *param);

double Xpred, Xlast, Bk, uk;

typedef struct
{
    double val;
    double a;
    double v;
    double t;
}X_t;

/** 
 * @brief 预测状态方程
 * @details Xpred = Xlast * Fk + Bk * uk
 * @param [in]  Xlast   上个状态
 * @param [in]  Fk      状态预测方程
 * @param [in]  Fk_par  状态预测方差的参数
 * @param [in]  Bk      控制量的变换矩阵
 * @param [in]  uk      当前状态的控制增益，可以为0
 * @return Xpred
 * 
 */
double kalman_caculate_xpred(double Xlast, Fk_func_t Fk, void *Fk_par, double Bk, double uk)
{
    double Xpred;

    Xpred = Fk(Xlast, Fk_par) + Bk * uk;

    return Xpred;
}

/**
 * @brief 预测协方差方程
 * @details Ppred = Fk * Plast * Fk_T + Qk;
 * 
*/
double kalman_caculate_ppred(double Ppred, Fk_func_t Fk, void *Fk_par, double Qk)
{

}

/**
 * @brief 卡尔曼增益方程
 * @details 
*/

int kalman_init(void)
{
    printf("kalman init\n");
    return 0;
}


