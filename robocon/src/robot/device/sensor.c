#include "ecrobot_interface.h"
#include "./sensor.h"

/* 変更禁止 */
/* センサポート定義 */
#define LIGHT_SENSOR_PORT   NXT_PORT_S3
#define SONAR_SENSOR_PORT   NXT_PORT_S2
#define GYRO_SENSOR_PORT    NXT_PORT_S1
#define TOUCH_SENSOR_PORT   NXT_PORT_S4
/* 変更禁止 */

#define LIGHTBUFFSIZE 20

static  unsigned short  u2g_light_data_buff[LIGHTBUFFSIZE];    /* 光センサ値のバッファ */
static  long  s4g_light_data_sum;                              /* 合計 */
static  unsigned short  u2g_light_data_pt;                     /* ポインタ */

unsigned short measure_sens( void );                           /* キャリブレーション用 */
void sort( unsigned short *data, unsigned int num );           /* キャリブレーション用 */
void setAverVal( SENSOR_LIGHT* this );                         /* 光センサ値のバッファ初期設定用 */

// SENSOR CLASS
void resetSensorValue ( SENSOR* this ) {
    getLightSensorValue ( this ) ;
	getSonarSensorValue ( this ) ;
	getGyroSensorValue ( this ) ;
}

unsigned short getLightSensorValue ( SENSOR* this ) {
	return getLightValue( &(this->LIGHT) );
}

int getSonarSensorValue ( SENSOR* this ) {
	return getSonarValue ( &(this->SONAR) );
}

unsigned short getGyroSensorValue ( SENSOR* this ) {
	return getGyroValue ( &(this->GYRO) );
}

void startSensor( SENSOR* this )
{
    startLightSensor( &(this->LIGHT) );
}

void stopSensor( SENSOR* this )
{
    stopLightSensor( &(this->LIGHT) );
}

// LIGHT SENSOR CLASS
unsigned short getLightValue ( SENSOR_LIGHT* this ) {

    unsigned short i;
    
	this->LIGHT_VALUE = ecrobot_get_light_sensor ( LIGHT_SENSOR_PORT ) ;
    
    /* 光センサ値の入力 */
    u2g_light_data_buff[u2g_light_data_pt] = this->LIGHT_VALUE;
    
    /* データ挿入位置の更新 */
    u2g_light_data_pt = u2g_light_data_pt + 1;
    if ( u2g_light_data_pt >= LIGHTBUFFSIZE )
    {
        u2g_light_data_pt = 0;
    }
    
    /* 合計算出 */
    s4g_light_data_sum = 0;
    for( i = 0; i < LIGHTBUFFSIZE; i++ )
    {
        s4g_light_data_sum = s4g_light_data_sum + u2g_light_data_buff[i];
    }
    
    /* 光センサ値の移動平均値を算出 */
	return s4g_light_data_sum / LIGHTBUFFSIZE;

}

void startLightSensor( SENSOR_LIGHT* this )
{
    ecrobot_set_light_sensor_active( LIGHT_SENSOR_PORT );
}

void stopLightSensor( SENSOR_LIGHT* this )
{
    ecrobot_set_light_sensor_inactive( LIGHT_SENSOR_PORT );
}

void setWhiteValue( SENSOR_LIGHT* this )
{
	this->white = measure_sens();
//    this->white = ecrobot_get_light_sensor ( LIGHT_SENSOR_PORT );
}
void setBlackValue( SENSOR_LIGHT* this )
{
    this->black = measure_sens();
//    this->black = ecrobot_get_light_sensor ( LIGHT_SENSOR_PORT );
    setAverVal( this );
}
void setGreyValue( SENSOR_LIGHT* this )
{
    this->grey = measure_sens();
//    this->grey = ecrobot_get_light_sensor ( LIGHT_SENSOR_PORT );
}


// SONAR SENSOR CLASS
int getSonarValue ( SENSOR_SONAR* this ) {
	this->SONAR_VALUE = ecrobot_get_sonar_sensor ( SONAR_SENSOR_PORT ) ;
	return this->SONAR_VALUE;
}

// GYRO SENSOR CLASS
unsigned short getGyroValue ( SENSOR_GYRO* this ) {
	this->GYRO_VALUE = ecrobot_get_gyro_sensor ( GYRO_SENSOR_PORT ) ;
	return this->GYRO_VALUE;
}

void setGyroOffsetValue ( SENSOR_GYRO* this )
{
	this->gyroOffsetValue = ecrobot_get_gyro_sensor ( GYRO_SENSOR_PORT ) ;
}


unsigned short
measure_sens( void )
{
    unsigned short  i;
    unsigned short  u2t_input[100];
    unsigned int  u4t_sum;
    unsigned int  u4t_start_time;
    
    /* 計測スタート音を出す */
    ecrobot_sound_tone(1000, 100, 50);

    /* センサ値を取得 */
    for ( i = 0; i < 100; i++ )
    {
       u2t_input[i] = ecrobot_get_light_sensor(LIGHT_SENSOR_PORT);

       /* 8ms待つ */
       u4t_start_time = ecrobot_get_systick_ms();
       while ( (ecrobot_get_systick_ms() - u4t_start_time) < 8U );
    }

    /* ソート */
    sort( u2t_input, 100 );

    /* 下10個・上10個を捨てて、合計算出 */
    u4t_sum = 0;
    for ( i = 10; i < 90; i++ )
    {
        u4t_sum = u4t_sum + (unsigned int)u2t_input[i];
    }

    /* 計測エンド音①を出す */
    ecrobot_sound_tone(500, 70, 50);

    /* 100ms待つ */
    u4t_start_time = ecrobot_get_systick_ms();
    while ( (ecrobot_get_systick_ms() - u4t_start_time) < 100U );

    /* 計測エンド音②を出す */
    ecrobot_sound_tone(500, 70, 50);

    /* センサ値（処理済み）の更新 */
    return (unsigned short)( u4t_sum / 80 );

}

void
sort( unsigned short *data, unsigned int num )
{
    unsigned int  i, j;
    unsigned int  min_indx;
    unsigned int  min_data;
    unsigned int  swap_data;

    for ( i = 0; i < num; i++ )
    {
        min_data = data[i];
        min_indx = i;

        for ( j = i; j < num; j++ )
        {
            if ( data[j] < min_data )
            {
                min_data = data[j];
                min_indx = j;
            }
        }
        swap_data = data[i];
        data[i] = data[min_indx];
        data[min_indx] = swap_data;
    }
}


void
setAverVal( SENSOR_LIGHT* this )
{
	unsigned char i;
	unsigned short val;

	val = (( this->white + this->black ) / 2 + this->white ) / 2;
	for( i = 0; i < LIGHTBUFFSIZE ; i++)
	{
	    u2g_light_data_buff[i] = val;
	}
}
