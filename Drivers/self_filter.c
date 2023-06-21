#include "self_filter.h"
#include <math.h>

	

#include <math.h>



void kalman_filter_init(KalmanFilter *kf, float init_angle, float Q_angle, float Q_bias, float R_measure) {
    kf->angle = init_angle;
    kf->bias = 0.0;
    kf->P[0][0] = 1.0;
    kf->P[0][1] = 0.0;
    kf->P[1][0] = 0.0;
    kf->P[1][1] = 1.0;
    kf->Q_angle = Q_angle;
    kf->Q_bias = Q_bias;
    kf->R_measure = R_measure;
}

float kalman_filter_update(KalmanFilter *kf, float gyro_rate, float accel_angle,	float dt) {
    // 1. Ԥ��״̬

    kf->angle += dt * (gyro_rate - kf->bias);

    // 2. Ԥ�����Э�������
    kf->P[0][0] += dt * (dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_bias * dt;

    // 3. ���㿨��������22
    float S = kf->P[0][0] + kf->R_measure;
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;

    // 4. ����״̬��������ƣ�
    float y = accel_angle - kf->angle;
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;

    // 5. �������Э�������
    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];
    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;

    return kf->angle;

	}

void custom_low_pass_filter_init(CustomLowPassFilter *lpf, float alpha, float R) {
    lpf->last_value = 0;
    lpf->last_estimate = 0;
    lpf->alpha = alpha;
    lpf->R = R;
}


float custom_low_pass_filter_update(CustomLowPassFilter *lpf, float measurement) {
    // �������
    float prior_estimate = lpf->last_estimate;

    // �������
    float K = lpf->R / (lpf->R + lpf->alpha * lpf->alpha);
    float posterior_estimate = prior_estimate + K * (measurement - prior_estimate);

    // �����˲���״̬
    lpf->last_estimate = posterior_estimate;

    return posterior_estimate;
}