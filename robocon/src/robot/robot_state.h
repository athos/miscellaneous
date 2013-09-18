
#ifndef ROBOT_STATUS_H
#define ROBOT_STATUS_H

typedef struct _ROBOT_STATUS {

    unsigned short lightval;       /* 光センサなまし値 */
    unsigned short gyroval;        /* ジャイロセンサ生値 */
    unsigned short gyro_offset;    /* ジャイロセンサoffset */

             int   sonarval;       /* 超音場センサ生値 */
             int   motorangle_L;   /* 左モータエンコーダ生値 */
             int   motorangle_R;   /* 右モータエンコーダ生値 */
             int   motorangle_T;   /* 左モータエンコーダ生値 */

    short motorSpeed;              /* 左右モータエンコード値変化量 */

    unsigned short batteryval;     /* バッテリ電圧生値 lsb=1, unit=mv */
    unsigned int   nowtime;        /* フリーランタイマ時間 */

} ROBOT_STATUS;

extern ROBOT_STATUS robot_status;

#endif
