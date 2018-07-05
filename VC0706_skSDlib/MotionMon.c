// VC0706�`�b�v�̃J�����e�X�g(�������Ď�����T���v��)
// skSDlib�̃��C�u�������g�p�����v���O����
// MPLAB X v3.50/XC8 v1.40 �ł̃R���p�C���ł��B
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skUARTlib.h"
#include "skVC0706.h"
#include "skSPIlib.h"
#include "skSDlib.h"
#include "skI2Clib.h"
#include "skI2CLCDlib.h"

// �萔�̒�`
#define _XTAL_FREQ   32000000      // delay�p�ɕK�v(�N���b�N32MHz���w��)

// �R���t�B�M�����[�V�����̐ݒ�(�����̋L�q�ɖ����ݒ�̓f�t�H���g�l�ł̓���)
// �O���N���b�N�͎g�p���Ȃ�(PRICLKEN_OFF)
// ����N���b�N��4�{�̓\�t�g�Ő���(PLLCFG_OFF)�F�����ۯ����g�p��CLKOUT��(INTIO67)
#pragma config PRICLKEN=OFF,PLLCFG=OFF,FOSC=INTIO67     // CONFIG1H
// �d���d���~���펞�Ď��@�\ON(BOREN_NOSLP)�F�Ď��d����(2.85V)�ɐݒ�
// �d��ON�����65.6ms����۸��т��J�n����(PWRTEN_ON)
#pragma config BOREN=NOSLP,BORV=285,PWRTEN=ON          // CONFIG2L
// �����ޯ����ϰ����(WDTEN_OFF)
#pragma config WDTEN=OFF                               // CONFIG2H
// MCLR��ؾ����݂Ƃ��Ďg�p����A�޼��ٓ��͕s��(MCLRE_EXTMCLR)
// �I�V���[�^�����肷��̂�҂��ăV�X�e���N���b�N����������(HFOFST_OFF)
#pragma config MCLRE=EXTMCLR,HFOFST=OFF                // CONFIG3H
// ��d���v���O���~���O�@�\�g�p���Ȃ�(LVP_OFF)
#pragma config LVP=OFF                                 // CONFIG4L

int FileNo ;     // �t�@�C���ԍ�
char JPG_Size[3][8] = {"640x480","320x240","160x120"} ;

// ���荞�݂̏���
void interrupt InterFunction( void )
{
     // �h�Q�b�֘A�̊��荞��(LCD�ŗ��p)
     InterI2C() ;
     // �t�r�`�q�s�֘A�̊��荞��(CAMERA�ŗ��p)
     InterUART() ;
}
// ���C���̏���
void main()
{
     int  ans ;
     char *p ;

     PLLEN  = 1;              // �����N���b�N��4x�{�ŗ��p����
     OSCCON = 0b01100000;     // �����N���b�N�Ƃ���(8MHzx4=32MHz)
     INTCON2bits.RBPU = 0 ;   // �����v���A�b�v���s��
     WPUB   = 0b00100110 ;    // RB1/RB2/RB5���v���A�b�v����
     ANSELA = 0b00000000 ;    // AN0-4�A�i���O�͎g�p���Ȃ��A�f�W�^��I/O�Ɋ���
     ANSELB = 0b00000000 ;    // AN8-13�A�i���O�͎g�p���Ȃ��A�f�W�^��I/O�Ɋ���
     ANSELC = 0b00000000 ;    // AN14-19�A�i���O�͎g�p���Ȃ��A�f�W�^��I/O�Ɋ���
     TRISA  = 0b00000000 ;    // RA0-RA7�S�ďo�͂ɐݒ�
     TRISB  = 0b00100110 ;    // RB1(SCL2)RB2(SDA2)RB5(SW)�͓��́A���͑S�ďo�́A1�œ��� 0�ŏo��
     TRISC  = 0b10010000 ;    // RC4(SDI1)RC7(RX1)�͓��́A���͑S�ďo�́A1�œ��� 0�ŏo��
     PORTA  = 0b00000000 ;    // �o�̓s���̏�����(�S��LOW�ɂ���)
     PORTB  = 0b00000000 ;    // �o�̓s���̏�����(�S��LOW�ɂ���)
     PORTC  = 0b00000000 ;    // �o�̓s���̏�����(�S��LOW�ɂ���)

     // USART�ʐM�̏��������s��(38400bps)
     InitUART(0,0,51) ;
     // I2C�ʐM�̃}�X�^�[���[�h�ŏ��������s���A�ʐM���x��400KHz
     InitI2C_Master(1) ;
     // I2C�ڑ����^LCD���W���[���̏���������
     // ICON OFF,�R���g���X�g(0-63),VDD=3.3V�Ŏg��,LCD��8������
     LCD_Init(LCD_NOT_ICON,35,LCD_VDD3V,8) ;
     LCD_Puts(" Camera ") ;
     // SPI�̏��������s��(�N���b�N�ɐ�:HIGH �N���b�N�ʑ�:0�@�ʐM���x:Fosc/4)
     SPI_Init(SPI_MODE3,SPI_CLOCK_DIV4,0) ;
//     // MMC/SDC�̏��������s��
//     ans = SD_Init() ;
//     if (ans != 0) {
//          // SD�J�[�h�������G���[(�J�[�h���}�������H)
//          LCD_SetCursor(0,1) ;
//          LCD_Puts("ErrorSDC") ;
//          while(1) ;     // �I��
//     }
     FileNo = 0 ;        // �ʐ^�ԍ��A�ʐ^���B��x�Ƀv���X�����

     __delay_ms(3000) ;  // 3�b��ɊJ�n(�J��������̃��b�Z�[�W�͂��̊Ԃɓǂݎ̂Ă�)
     UART_Flush() ;      // UART�o�b�t�@�̃N���A

     // GetVersion����
     LCD_SetCursor(0,0) ;
     p = VC070x_GetVersion() ;
     if (p == 0) LCD_Puts("ERR-0x11") ;
     else        LCD_Puts(p) ;
     // Motion�Ď����J�n����
     LCD_SetCursor(0,1) ;
     ans = VC070x_MotionCtlEnable() ;
     ans = VC070x_SetMotionCtl(VC070X_MOTION_DETECT_ON) ;
     if (ans == -1) {
          LCD_Puts("ERR-0x37") ;
          while(1) ;     // �I��
     }
     LCD_Puts("StartMon") ;

     while(1) {
          // �J��������̃f�[�^��M���s��(���[�V�����Ď�)
          ans = VC070x_MotionDetect() ;
          if (ans != -1) {
               // ������M�����悤��
               if (ans == VC070X_COMM_MOTION_DETECTED) {
                    // ���������o��������
                    LCD_SetCursor(0,0) ;
                    LCD_Puts("Detected") ;
                    LCD_SetCursor(0,1) ;
                    LCD_Puts("        ") ;
                    // �R�b��ɕ\��������
                    __delay_ms(3000) ;
                    LCD_SetCursor(0,0) ;
                    LCD_Puts("        ") ;

                    /*  �����Ŏʐ^��ۑ�����Ȃ�x����o���Ȃ育���R��  */

               } else {
                    // ���̃R�}���h����M�����悤��
                    LCD_SetCursor(0,1) ;
                    LCD_Puts("etc. cmd") ;
                    UART_Flush() ;      // UART�o�b�t�@�̃N���A
               }
          }
     }
}
