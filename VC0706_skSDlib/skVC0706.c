/*******************************************************************************
*  skVC0706.c - VC0706-chip�p�֐����C�u����                                    *
*               (Digital Video Processor VC0706)                               *
*                                                                              *
*    VC070x_GetError       - �G���[���𓾂鏈��                              *
*    VC070x_GetVersion     - �t�@�[���E�F�A�̃o�[�W�����𓾂鏈��              *
*    VC070x_Reset          - �J�����̃��Z�b�g���s������                        *
*    VC070x_PowerCtl       - �ȓd�̓��[�h�̐�����s������                      *
*    VC070x_GetImageSize   - �摜�̑傫���𓾂鏈��                            *
*    VC070x_GetColorSts    - COLOR�̃��[�h�𓾂鏈��                           *
*    VC070x_SetColorCtl    - COLOR�̃��[�h��ݒ肷�鏈��                       *
*    VC070x_GetCompression - �摜�̈��k��𓾂鏈��                            *
*    VC070x_SetCompression - �摜�̈��k���ݒ肷�鏈��                        *
*    VC070x_MotionCtlEnable- ���[�V�����Ď��������鏈��                      *
*    VC070x_SetMotionCtl   - �����̃��j�^�[�����O�𐧌䂷�鏈��                *
*    VC070x_MotionDetect   - ���[�V�����Ď����s������                          *
*    VC070x_TakePicture    - �摜���L���v�`���[���鏈��                        *
*    VC070x_ReadPicture    - �摜�f�[�^��ǂݍ��ޏ���                          *
*    VC070x_ResumeVideo    - �r�f�I���ĊJ���鏈��                              *
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
#include <xc.h>
#include "skUARTlib.h"
#include "skVC0706.h"

char ReadReturnBuff[VC070X_READDATA_BUFFSZ] ;     // �R�}���h������M�o�b�t�@
union VC070X_ERROR_t Erro ;                       // �Ō�ɃG���[���Ԃ��ꂽ���̃G���[���

// ���M�R�}���h�ɑ΂���ԓ���ǂݍ��ޏ���
int vc070x_read(char *buff,int len)
{
     uint16_t dt ;
     int i , t , rflg ;

     i = t = 0 ;
     rflg  = 0 ;
     while(1) {
          if (UART_Available() > 0) {
               dt = UART_Read() ;
               if (dt == 0x76) rflg = 1 ;
               if (rflg == 1) {
                    *buff = (char)dt ;
                    buff++ ;
                    i++ ;
                    if (i >= len) return i ; // ��M�I��
               }
               t = 0 ;
          }
          __delay_us(10) ;
          t++ ;
          if (t >= 10000) return -1 ;        // 100ms�Ń^�C���I�[�o�[
     }
     return i ;
}
// �R�}���h�̑��M�ƕԓ��̏������s��
int vc070x_sendread(char *cmd,int snd_su,int rcv_su)
{
     Erro.cmd_no = *(cmd+2) ;
     Erro.err_no = 0 ;
     UART_Send(cmd,snd_su) ;
     if (vc070x_read(ReadReturnBuff,rcv_su) == rcv_su) {
          if (ReadReturnBuff[2] == Erro.cmd_no) {
               if (ReadReturnBuff[3] == 0x00) {
                    return 0 ;
               } else Erro.err_no = ReadReturnBuff[3] ;// �G���[���Ԃ��ꂽ�B
          } else Erro.err_no = 10 ;     // ���M�R�}���h�ɑ΂���ԓ��łȂ���B
     } else Erro.err_no = 11 ;          // ��M�^�C���I�[�o�[
     return -1 ;
}
/*******************************************************************************
*  ans = VC070x_GetError()                                                     *
*    �G���[���𓾂鏈��                                                      *
*    �Ō�Ɏ�M�������̃G���[�ԍ���Ԃ��܂��B                                  *
*                                                                              *
*    ans  : �G���[�ԍ���Ԃ�                                                   *
*******************************************************************************/
uint16_t VC070x_GetError()
{
     return Erro.info ;
}
/*******************************************************************************
*  ans = VC070x_Reset()                                                        *
*    �J�����̃��Z�b�g���s������                                                *
*    ���Z�b�g����Ɠd��ON���Ɠ������b�Z�[�W�𑗐M���ė���̂œǂݎ̂Ă�        *
*    �d��ON��2-3�b�����ăR�}���h�w�����J�n����l�Ƀf�[�^�V�[�g�ɏ����ėL��̂� *
*    ���Z�b�g������2-3�b���҂��������ǂ��������B                               *
*                                                                              *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_Reset()
{
     char cmd[4]= {0x56,0x00,0x26,0x00} ;    // SYSTEM_RESET
     int ans ;

     if ((ans=vc070x_sendread(cmd,4,5)) == 0) {
          __delay_ms(1000) ;  // �J��������̃��b�Z�[�W�͂��̊Ԃɓǂݎ̂Ă�
     }
     UART_Flush() ;           // UART�o�b�t�@�̃N���A
     return ans ;
}
/*******************************************************************************
*  ans = VC070x_GetVersion()                                                   *
*    �t�@�[���E�F�A�̃o�[�W�����𓾂鏈��                                      *
*                                                                              *
*    *ans : �������̓o�[�W������񕶎���ւ̃A�h���X��Ԃ�(��F"VC0703 1.00")  *
*           ���s���͂O��Ԃ�                                                   *
*******************************************************************************/
char *VC070x_GetVersion()
{
     char cmd[4]= {0x56,0x00,0x11,0x00} ;    // GEN_VERSION

     if (vc070x_sendread(cmd,4,16) == 0) {
           ReadReturnBuff[16] = 0x00 ;
           return (char *)&ReadReturnBuff[5] ;
     }
     return 0 ;
}
/*******************************************************************************
*  ans = VC070x_PowerCtl(info,time)                                            *
*    �ȓd�̓��[�h�̐�����s������                                              *
*                                                                              *
*    info : �O=OFF �P=ON ���̑�=time���Ԃŏȓd�̓��[�h�ɓ���                   *
*    time : 10ms�P�ʂŎw�肷��                                                 *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_PowerCtl(int info,uint16_t time)
{
     char cmd[8]= {0x56,0x00,0x3E,0x03,0x00,0x01,0x00,0x00} ;    // POWER_SAVE_CTRL
     int  su ;

     switch(info) {
        case 0:     // �ȓd�̓��[�hOFF
        case 1:     // �ȓd�̓��[�hON
               cmd[6] = (char)info ;
               su = 7 ;
               break ;
        default :   // �w�莞�Ԃŏȓd�̓��[�h�ɓ���
               cmd[3] = 0x04 ;
               cmd[4] = 0x01 ;
               cmd[5] = 0x04 ;
               cmd[6] = (char)(time >> 8) ;
               cmd[7] = (char)(time & 0x00FF) ;
               su = 8 ;
               break ;
     }
     return vc070x_sendread(cmd,su,5) ;
}
/*******************************************************************************
*  ans = VC070x_GetCompression()                                               *
*    �摜�̈��k��𓾂鏈��                                                    *
*                                                                              *
*    ans : ��������0x00�`0xFF                                                  *
*          ���s����-1��Ԃ�                                                    *
*******************************************************************************/
int VC070x_GetCompression()
{
     char cmd[8]= {0x56,0x00,0x30,0x04,0x01,0x01,0x12,0x04} ;         // READ_DATA

     if (vc070x_sendread(cmd,8,6) == 0) {
           return (int)ReadReturnBuff[5] ;
     }
     return -1 ;
}
/*******************************************************************************
*  ans = VC070x_SetCompression(val)                                            *
*    �摜�̈��k���ݒ肷�鏈��                                                *
*    ���Z�b�g����ƌ��ɖ߂�܂��B                                              *
*                                                                              *
*    val  : �摜���k��̎w�������(���l�������ƈ��k�������)                   *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_SetCompression(int val)
{
     char cmd[9]= {0x56,0x00,0x31,0x05,0x01,0x01,0x12,0x04,0x00} ;    // WRITE_DATA

     cmd[8] = val ;
     return vc070x_sendread(cmd,9,5) ;
}
/*******************************************************************************
*  ans = VC070x_GetColorSts()                                                  *
*    COLOR�̃��[�h�𓾂鏈��                                                   *
*                                                                              *
*    ans : �������� 0=�����ݒ�  1=COLOR���[�h  2=�������[�h                    *
*          ���s����-1��Ԃ�                                                    *
*******************************************************************************/
int VC070x_GetColorSts()
{
     char cmd[4]= {0x56,0x00,0x3D,0x00} ;         // COLOR_STATUS

     if (vc070x_sendread(cmd,4,8) == 0) {
          return (int)ReadReturnBuff[6] ;
     }
     return -1 ;
}
/*******************************************************************************
*  ans = VC070x_SetColorCtl(val)                                               *
*    COLOR�̃��[�h��ݒ肷�鏈��                                               *
*    �������[�h�ɂ��Č��������A�F�����H�ɂȂ��������Ŕ����ɂȂ�Ȃ��������H    *
*                                                                              *
*    val  : COLOR�̃��[�h�̎w�������(0=�����ݒ�  1=COLOR���[�h  2=�������[�h) *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_SetColorCtl(int val)
{
     char cmd[6]= {0x56,0x00,0x3C,0x02,0x01,0x00} ;    // COLOR_CTRL

     cmd[5] = val ;
     return vc070x_sendread(cmd,6,5) ;
}
/*******************************************************************************
*  ans = VC070x_GetImageSize()                                                 *
*    �摜�̑傫���𓾂鏈��                                                    *
*                                                                              *
*    ans : �������� 0(0x00)=640x480  17(0x11)=320x240  34(0x22)=160x120        *
*          ���s����-1��Ԃ�                                                    *
*******************************************************************************/
int VC070x_GetImageSize()
{
     char cmd[8]= {0x56,0x00,0x30,0x04,0x04,0x01,0x00,0x19} ;         // READ_DATA

     if (vc070x_sendread(cmd,8,6) == 0) {
           return (int)ReadReturnBuff[5] ;
     }
     return -1 ;
}
/*******************************************************************************
*  ans = VC070x_SetImageSize(sz)                                               *
*    �摜�̑傫����ݒ肷�鏈��                                                *
*    ���̊֐��Őݒ肵�Ă��A640x480�̂܂ܕς��Ȃ��B                           *
*    ��肭���삹���A���Z�b�g���Ă��_���ł��������L�邩�������s���ł��B      *
*                                                                              *
*    sz   : �摜�T�C�Y�̎w�������                                             *
*           0(0x00)=640x480  17(0x11)=320x240  34(0x22)=160x120                *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_SetImageSize(int sz)
{
     char cmd[9]= {0x56,0x00,0x31,0x05,0x04,0x01,0x00,0x19,0x00} ;    // WRITE_DATA

     cmd[8] = sz ;
     return vc070x_sendread(cmd,9,5) ;
}
/*******************************************************************************
*  ans = VC070x_MotionCtlEnable()                                              *
*    ���[�V�����Ď��������鏈��                                              *
*                                                                              *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_MotionCtlEnable()
{
     char cmd[7]= {0x56,0x00,0x42,0x03,0x00,0x01,0x01} ;   // MOTION_CTRL

     // ���[�V�����Ď����g�p�o����悤�ɋ�����
     return vc070x_sendread(cmd,7,5) ;
}
/*******************************************************************************
*  ans = VC070x_SetMotionCtl(val)                                              *
*    �����̃��j�^�[�����O�𐧌䂷�鏈��                                        *
*                                                                              *
*    val  : 1=���j�^�[�����O�J�n  0=���j�^�[�����O��~                         *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_SetMotionCtl(int val)
{
     char cmd[5]= {0x56,0x00,0x37,0x01,0x00} ;             // COMM_MOTION_CTRL

     // ���j�^�[�����O�̐ݒ�
     cmd[4] = val ;
     return vc070x_sendread(cmd,5,5) ;
}
/*******************************************************************************
*  ans = VC070x_MotionDetect()                                                 *
*    ���[�V�����Ď����s������                                                  *
*                                                                              *
*    ans  : -1 = ������M���Ă��Ȃ��������̓G���[����M����                    *
*           ����ȊO�́A��M�����R�}���h�ԍ���Ԃ��܂��B                       *
*******************************************************************************/
int VC070x_MotionDetect()
{
     if (UART_Available() > 0) {
          if (vc070x_read(ReadReturnBuff,4) == 4) {
               if ((ReadReturnBuff[3] == 0x00)) {
                    return ReadReturnBuff[2] ;    // ��M�����R�}���h�ԍ���Ԃ�
               } // else ��error��Ԃ���
          } // else �͎�M�^�C���I�[�o�[
     }
     return -1 ;
}
/*******************************************************************************
*  ans = VC070x_ResumeVideo()                                                  *
*    �r�f�I���ĊJ���鏈��                                                      *
*                                                                              *
*    ans  : �������͂O��Ԃ�                                                   *
*           ���s����-1��Ԃ�                                                   *
*******************************************************************************/
int VC070x_ResumeVideo()
{
     char cmd[5]= {0x56,0x00,0x36,0x01,0x03} ;   // FBUF_CTRL(resume frame)

     return vc070x_sendread(cmd,5,5) ;
}
/*******************************************************************************
*  ans = VC070x_TakePicture()                                                  *
*    �摜���L���v�`���[���鏈��                                                *
*    ���̊֐����R�[������ꍇ�́A�r�f�I���N�����Ă��鎖���O��ł��B            *
*                                                                              *
*    ans : �������̓L���v�`���[�����摜�̃t�@�C���T�C�Y��Ԃ�                  *
*          ���s���͂O��Ԃ�                                                    *
*******************************************************************************/
uint32_t VC070x_TakePicture()
{
     union {
          struct {
               unsigned char b1 ;  // Low byte
               unsigned char b2 ;
               unsigned char b3 ;
               unsigned char b4 ;  // High byte
          } ;
          uint32_t  l ;
     } dt ;
     char cmd1[5]= {0x56,0x00,0x36,0x01,0x00} ;   // FBUF_CTRL(stop current frame)
     char cmd2[5]= {0x56,0x00,0x34,0x01,0x00} ;   // GET_FBUF_LEN(JPEG file size)

     UART_Flush() ;           // UART�o�b�t�@�̃N���A
     // �N�����Ă���r�f�I���~������(�L���v�`���[����)
     if (vc070x_sendread(cmd1,5,5) == 0) {
          // �摜�̃t�@�C���T�C�Y�𓾂�
          if (vc070x_sendread(cmd2,5,9) == 0) {
               dt.b4 = ReadReturnBuff[5] ;
               dt.b3 = ReadReturnBuff[6] ;
               dt.b2 = ReadReturnBuff[7] ;
               dt.b1 = ReadReturnBuff[8] ;
               return dt.l ;
          }
     }
     return 0 ;
}
/*******************************************************************************
*  ans = VC070x_ReadPicture(adrs,len)                                          *
*    �摜�f�[�^��ǂݍ��ޏ���                                                  *
*    �L���v�`���[�����摜�f�[�^��FBUF����ǂݍ��݂܂��B                        *
*                                                                              *
*    adrs : �ǂݍ���FBUF�̃A�h���X���w�肷��                                   *
*    len  : �ǂݍ��ރf�[�^�̒����w�肷��(VC070x_READDATA_BUFFSIZ�����Ƃ���)    *
*    *ans : �ǂݍ��񂾃f�[�^�̊i�[���Ԃ�                                     *
*******************************************************************************/
char *VC070x_ReadPicture(uint16_t adrs,int len)
{
     uint16_t dt ;
     int i , c ;
     char cmd[16]= {0x56,0x00,0x32,0x0c,0x00,0x0A,
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A} ;   // READ_FBUF

     // len�̒������̃f�[�^�𑗂�w��
     cmd[8] = adrs >> 8 ;     // FBUF�̓ǂݏo���A�h���X��HIGH��
     cmd[9] = adrs & 0xFF ;   // FBUF�̓ǂݏo���A�h���X��LOW��
     cmd[12]= len >> 8 ;      // �ǂݏo���f�[�^������HIGH��
     cmd[13]= len & 0xFF ;    // �ǂݏo���f�[�^������LOW��
     cmd[14]= VC070X_READDATA_DELAY >> 8 ;      // Interval time��HIGH��
     cmd[15]= VC070X_READDATA_DELAY & 0xFF ;    // Interval time��LOW��
     UART_Send(cmd,16) ;
     // �f�[�^����M����
     // �擪�ƍŌ�ɂ�"76 00 32 00 00"���L��̂œǂݎ̂Ă�
     i = c = 0 ;
     while(1) {
          if (UART_Available() > 0) {
               dt = UART_Read() ;
               i++ ;
               if (i>5) {
                    // 6�o�C�g�ڂ���len�̒������ǂݍ���
                    ReadReturnBuff[c] = dt ;
                    c++ ;
                    if (c == len) break ;
               }
          }
     }
     i = 0 ;
     while(1) {
          if (UART_Available() > 0) {
               dt = UART_Read() ;
               i++ ;
               if (i==4) c = dt ;  // �ʐM�󋵂̎��o��
               if (i>=5) break ;
          }
     }
     return (char *)&ReadReturnBuff ;
}
