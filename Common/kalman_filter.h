
#ifndef __COMMON_KALMAN_FILTER_H__
#define __COMMON_KALMAN_FILTER_H__

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        double value; //k-1时刻的滤波值，即是k-1时刻的值
        double gain;  //   Kalamn增益
        double A;     // x(n)=A*x(n-1)+u(n),u(n)~N(0,Q)
        double H;     // z(n)=H*x(n)+w(n),w(n)~N(0,R)
        double Q;     //预测过程噪声偏差的方差
        double R;     //测量噪声偏差，(系统搭建好以后，通过测量统计实验获得)
        double P;     //估计误差协方差
    } kalman_filter_t, *kalman_filter_pt;

    void kalman_init(kalman_filter_pt info, double init_value, double Q, double R);
    double kalman_filter(kalman_filter_pt info, double new_value);
#ifdef __cplusplus
}
#endif
#endif
