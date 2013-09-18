
#ifndef SENSOR_H
#define SENSOR_H


// LIGHT SENSOR CLASS
typedef struct _light_sensor {
	unsigned short LIGHT_VALUE;		//	���Z���T�l
	unsigned short black;
	unsigned short white;
	unsigned short grey;
} SENSOR_LIGHT;

unsigned short getLightValue ( SENSOR_LIGHT* this ) ;		//	���Z���T�l�̎擾
void startLightSensor( SENSOR_LIGHT* this );  /* ���Z���T���N�� */
void stopLightSensor( SENSOR_LIGHT* this );   /* ���Z���T���~ */
void setWhiteValue ( SENSOR_LIGHT* this ) ;		//	���Z���T�l�̎擾
void setBlackValue ( SENSOR_LIGHT* this ) ;		//	���Z���T�l�̎擾
void setGreyValue ( SENSOR_LIGHT* this ) ;		//	���Z���T�l�̎擾


// SONAR SENSOR CLASS
typedef struct _sonar_sensor {
	int SONAR_VALUE;		//	�����g�Z���T�l
/*  void startSonarSensor( void ); */ /* �����g�Z���T���N�� */
} SENSOR_SONAR;

int getSonarValue ( SENSOR_SONAR* this ) ;	//	�����g�Z���T�l�̎擾

// GYRO SENSOR CLASS
typedef struct _gyro_sensor {
	unsigned short GYRO_VALUE;		//	�W���C���Z���T�l
	unsigned short gyroOffsetValue;
} SENSOR_GYRO;

unsigned short getGyroValue ( SENSOR_GYRO* this ) ;		//	�W���C���Z���T�l�̎擾
void setGyroOffsetValue ( SENSOR_GYRO* this ) ;		//	���Z���T�l�̎擾

// SENSOR CLASS
typedef struct _sensor {
	SENSOR_LIGHT LIGHT;		//	���Z���T���
	SENSOR_SONAR SONAR;		//	�����g�Z���T���
	SENSOR_GYRO GYRO;		//	�W���C���Z���T���

} SENSOR;

void resetSensorValue ( SENSOR* this ) ;				//	�Z���T���̏�����

unsigned short getLightSensorValue ( SENSOR* this ) ;	//	���Z���T���̎擾
int getSonarSensorValue ( SENSOR* this ) ;				//	�����g�Z���T���̎擾
unsigned short getGyroSensorValue ( SENSOR* this ) ;	//	�W���C���Z���T���̎擾

void startSensor( SENSOR* this );
void stopSensor( SENSOR* this );

#endif
