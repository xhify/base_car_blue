

typedef struct {
    float angle;
    float bias;
    float P[2][2];
    float Q_angle; // �Ƕȹ�������Э����
    float Q_bias;  // ƫ���������Э����
    float R_measure; // ��������Э����
} KalmanFilter;

typedef struct {
    float last_value;
    float last_estimate;
    float alpha; // ʱ�䳣��
    float R; // ��������Э����
} CustomLowPassFilter;

#ifdef __cplusplus
extern "C" {
#endif
void kalman_filter_init(KalmanFilter *kf, float init_angle, float Q_angle, float Q_bias, float R_measure);
float kalman_filter_update(KalmanFilter *kf, float gyro_rate, float accel_angle,float dt);
void custom_low_pass_filter_init(CustomLowPassFilter *lpf, float alpha, float R);
float custom_low_pass_filter_update(CustomLowPassFilter *lpf, float measurement);
	#ifdef __cplusplus
}
#endif