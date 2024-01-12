
#ifndef __COMMON_KALMAN_FILTER_H__
#define __COMMON_KALMAN_FILTER_H__

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        double value; //k-1ʱ�̵��˲�ֵ������k-1ʱ�̵�ֵ
        double gain;  //   Kalamn����
        double A;     // x(n)=A*x(n-1)+u(n),u(n)~N(0,Q)
        double H;     // z(n)=H*x(n)+w(n),w(n)~N(0,R)
        double Q;     //Ԥ���������ƫ��ķ���
        double R;     //��������ƫ�(ϵͳ����Ժ�ͨ������ͳ��ʵ����)
        double P;     //�������Э����
    } kalman_filter_t, *kalman_filter_pt;

    void kalman_init(kalman_filter_pt info, double init_value, double Q, double R);
    double kalman_filter(kalman_filter_pt info, double new_value);
#ifdef __cplusplus
}
#endif
#endif
