
#ifndef ROBOT_STATUS_H
#define ROBOT_STATUS_H

typedef struct _ROBOT_STATUS {

    unsigned short lightval;       /* ���Z���T�Ȃ܂��l */
    unsigned short gyroval;        /* �W���C���Z���T���l */
    unsigned short gyro_offset;    /* �W���C���Z���Toffset */

             int   sonarval;       /* ������Z���T���l */
             int   motorangle_L;   /* �����[�^�G���R�[�_���l */
             int   motorangle_R;   /* �E���[�^�G���R�[�_���l */
             int   motorangle_T;   /* �����[�^�G���R�[�_���l */

    short motorSpeed;              /* ���E���[�^�G���R�[�h�l�ω��� */

    unsigned short batteryval;     /* �o�b�e���d�����l lsb=1, unit=mv */
    unsigned int   nowtime;        /* �t���[�����^�C�}���� */

} ROBOT_STATUS;

extern ROBOT_STATUS robot_status;

#endif
