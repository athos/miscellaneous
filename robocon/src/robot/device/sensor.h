
#ifndef SENSOR_H
#define SENSOR_H


// LIGHT SENSOR CLASS
typedef struct _light_sensor {
	unsigned short LIGHT_VALUE;		//	光センサ値
	unsigned short black;
	unsigned short white;
	unsigned short grey;
} SENSOR_LIGHT;

unsigned short getLightValue ( SENSOR_LIGHT* this ) ;		//	光センサ値の取得
void startLightSensor( SENSOR_LIGHT* this );  /* 光センサを起動 */
void stopLightSensor( SENSOR_LIGHT* this );   /* 光センサを停止 */
void setWhiteValue ( SENSOR_LIGHT* this ) ;		//	光センサ値の取得
void setBlackValue ( SENSOR_LIGHT* this ) ;		//	光センサ値の取得
void setGreyValue ( SENSOR_LIGHT* this ) ;		//	光センサ値の取得


// SONAR SENSOR CLASS
typedef struct _sonar_sensor {
	int SONAR_VALUE;		//	超音波センサ値
/*  void startSonarSensor( void ); */ /* 超音波センサを起動 */
} SENSOR_SONAR;

int getSonarValue ( SENSOR_SONAR* this ) ;	//	超音波センサ値の取得

// GYRO SENSOR CLASS
typedef struct _gyro_sensor {
	unsigned short GYRO_VALUE;		//	ジャイロセンサ値
	unsigned short gyroOffsetValue;
} SENSOR_GYRO;

unsigned short getGyroValue ( SENSOR_GYRO* this ) ;		//	ジャイロセンサ値の取得
void setGyroOffsetValue ( SENSOR_GYRO* this ) ;		//	光センサ値の取得

// SENSOR CLASS
typedef struct _sensor {
	SENSOR_LIGHT LIGHT;		//	光センサ情報
	SENSOR_SONAR SONAR;		//	超音波センサ情報
	SENSOR_GYRO GYRO;		//	ジャイロセンサ情報

} SENSOR;

void resetSensorValue ( SENSOR* this ) ;				//	センサ情報の初期化

unsigned short getLightSensorValue ( SENSOR* this ) ;	//	光センサ情報の取得
int getSonarSensorValue ( SENSOR* this ) ;				//	超音波センサ情報の取得
unsigned short getGyroSensorValue ( SENSOR* this ) ;	//	ジャイロセンサ情報の取得

void startSensor( SENSOR* this );
void stopSensor( SENSOR* this );

#endif
