
#include "kalman_filter.h"

/**
 * Q:����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
 * R:����������R���󣬶�̬��Ӧ�����������ȶ��Ա�� 
 */

/* @brief kalman_init   ��ʼ���˲����ĳ�ʼֵ
* @param info  �˲���ָ��
* @param Q Ԥ���������� ��ϵͳ�ⲿ�ⶨ����
* @param R ������������ ��ϵͳ�ⲿ�ⶨ����
*/
void kalman_init(kalman_filter_pt info, double init_value, double Q, double R)
{
    info->A = 1;              //����������
    info->H = 1;              //
    info->P = 10;             //����״̬����ֵ���ķ���ĳ�ʼֵ����ҪΪ0���ⲻ��
    info->Q = Q;              //Ԥ�⣨���̣��������� Ӱ���������ʣ����Ը���ʵ���������
    info->R = R;              //�������۲⣩�������� ����ͨ��ʵ���ֶλ��
    info->value = init_value; // �����ĳ�ʼֵ
}
double kalman_filter(kalman_filter_pt info, double new_value)
{
    //Ԥ����һʱ�̵�ֵ
    double predict = info->A * info->value; //x�������������һ��ʱ���ĺ������ֵ��������Ϣ�������˴���Ҫ���ݻ�վ�߶���һ���޸�

    //��Э����
    info->P = info->A * info->A * info->P + info->Q; //������������� p(n|n-1)=A^2*p(n-1|n-1)+q
    // double preValue = info->value;             //��¼�ϴ�ʵ�������ֵ

    //����kalman����
    info->gain = info->P * info->H / (info->P * info->H * info->H + info->R); //Kg(k)= P(k|k-1) H�� / (H P(k|k-1) H�� + R)
    //����������������˲�ֵ
    info->value = predict + (new_value - predict) * info->gain; //���ò������Ϣ���ƶ�x(t)�Ĺ��ƣ�����������ƣ����ֵҲ�������  X(k|k)= X(k|k-1)+Kg(k) (Z(k)-H X(k|k-1))
    //���º������
    info->P = (1 - info->gain * info->H) * info->P; //������������  P[n|n]=(1-K[n]*H)*P[n|n-1]

    return info->value;
}
