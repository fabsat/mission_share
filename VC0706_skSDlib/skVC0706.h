/*******************************************************************************
*  skVC0706.h - VC0706-chip�p�֐����C�u�����w�b�_�t�@�C��                      *
*               (Digital Video Processor VC0706)                               *
*                                                                              *
* ============================================================================ *
*  VERSION DATE        BY                    CHANGE/COMMENT                    *
* ---------------------------------------------------------------------------- *
*  1.01    2017-03-10  ���ޒ��H�[(���ނ���)  Create                            *
* ============================================================================ *
*  PIC 18F2xK22                                                                *
*  MPLAB X(v3.50)                                                              *
*  MPLAB(R) XC8 C Compiler Version 1.40                                        *
*******************************************************************************/
#ifndef _SKVC0706_H_
#define _SKVC0706_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef _XTAL_FREQ
 // Unless already defined assume 32MHz system frequency
 // This definition is required to calibrate __delay_us() and __delay_ms()
 #define _XTAL_FREQ 32000000 // �g�p����PIC���ɂ�蓮����g���l��ݒ肷��
#endif


#define VC070X_READDATA_BUFFSZ        128    // �摜�f�[�^��M�o�b�t�@�T�C�Y
#define VC070X_READDATA_DELAY          10    // �摜�f�[�^��M����Interval time���w��(x0.01mS�P��)

#define VC070X_MOTION_DETECT_ON         1    // �����̊Ď����J�n����
#define VC070X_MOTION_DETECT_OFF        0    // �����̊Ď����~����
#define VC070X_COMM_MOTION_DETECTED  0x39    // ���������o�������ɑ��M�����R�}���h
#define VC070X_COLOR_AUTO            0x00    // �J���[���[�h�͎���
#define VC070X_COLOR_COLOR           0x01    // �J���[���[�h��COLOR
#define VC070X_COLOR_MONO            0x02    // �J���[���[�h�͔���
#define VC070X_640X480               0x00    // �摜�̃T�C�Y��640x480�̑傫��
#define VC070X_320X240               0x11    // �摜�̃T�C�Y��320x240�̑傫��
#define VC070X_160X120               0x22    // �摜�̃T�C�Y��160x120�̑傫��


// �G���[���Ԃ��ꂽ���̏��\����
union VC070X_ERROR_t {
     struct {
          uint8_t cmd_no ;
          uint8_t err_no ;
     } ;
     uint16_t info ;
} ;

// �֐��̃v���g�^�C�v�錾
uint16_t VC070x_GetError() ;
char *VC070x_GetVersion() ;
int VC070x_Reset() ;
int VC070x_PowerCtl(int info,uint16_t time) ;
int VC070x_MotionCtlEnable() ;
int VC070x_MotionDetect() ;
int VC070x_SetMotionCtl(int val) ;
int VC070x_GetImageSize() ;
int VC070x_SetImageSize(int sz) ;
int VC070x_GetColorSts() ;
int VC070x_SetColorCtl(int val) ;
int VC070x_GetCompression() ;
int VC070x_SetCompression(int val) ;
int VC070x_ResumeVideo() ;
uint32_t VC070x_TakePicture() ;
char *VC070x_ReadPicture(uint16_t adrs,int len) ;



#endif
