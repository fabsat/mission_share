/*******************************************************************************
*  skUARTlib - �t�r�`�q�s�֐����C�u����                                        *
*          ���̃��C�u������USART(�V���A���ʐM)���s���ׂ̊֐��W�ł�             *
*                                                                              *
*    InterUART      - �t�r�`�q�s�֘A�̊��荞�ݏ���                             *
*    InitUART       - �t�r�`�q�s�ʐM�̏��������s������                         *
*    UART_Send      - ����Ɏw�肵�����̃f�[�^�𑗐M���鏈��                 *
*    UART_Write     - ����ɂP�o�C�g�̃f�[�^�𑗐M���鏈��                     *
*    UART_Available - ��M�����f�[�^�̌���Ԃ�����                           *
*    UART_Read      - ��M�����f�[�^���P�o�C�g�ǂݍ��ޏ���                     *
*    UART_Flush     - ��M�o�b�t�@���N���A���鏈��                             *
*                                                                              *
*    �����F�W����UART_8N1(8bit,nonParity,1stop)�����A"skUARTlib.h"�̋L�q���A   *
*          #define UART_8O1�Ƃ����(8bit,oddParity,1stop)�ő��M�ł��܂��B      *
*          �����ǂ��A��M���̃p���e�B�`�F�b�N�͍s���Ă��܂���B                *
*                                                                              *
* ============================================================================ *
*  VERSION DATE        BY                    CHANGE/COMMENT                    *
* ---------------------------------------------------------------------------- *
*  1.00    2013-01-20  ���ޒ��H�[(���ނ���)  Create                            *
*  1.10    2013-02-18  ���ޒ��H�[(���ނ���)  UART_Flush�֐��̒ǉ�              *
*  2.00    2015-05-25  ���ޒ��H�[(���ނ���)  16F1825/1829 18F2xK22 �Ή�        *
*  3.01    2017-02-01  ���ޒ��H�[(���ނ���)  �p���e�B(ODD)�Ή�                 *
*  3.10    2017-02-04  ���ޒ��H�[(���ނ���)  �ᑬ�ʐM���x�ɑΉ�                *
* ============================================================================ *
*  PIC 12F1822 16F1705 16F182x 16F1938/19xx 18F2xK22 1814K50 18F8722           *
*  MPLAB IDE(V8.84) MPLAB X(V3.50)                                             *
*  MPLAB(R) XC8 C Compiler Version 1.00/1.32/1.38/1.40                         *
*******************************************************************************/
#include <xc.h>
#include "skUARTlib.h"


char UART_Buffer[UART_BUFFER_SIZE] ;    // ��M�����f�[�^���i�[����o�b�t�@
int  UART_Buffer_inptr ;                // ��M�o�b�t�@�̓ǂݍ��݃|�C���^�[
int  UART_Buffer_outptr ;               // ��M�o�b�t�@�̏������݃|�C���^�[


/*******************************************************************************
*  InterUART( void )                                                           *
*    �t�r�`�q�s�֘A�̊��荞�ݏ���                                              *
*     ���̊֐��͎�M���s���ꍇ�̓��C���v���O�����̊����݊֐��ŕK���Ăт܂�     *
*******************************************************************************/
void InterUART( void )
{
     int  x ;
     char dt ;

     if (RC0IF == 1) {        // �����݂͂t�r�`�q�s�ʐM�̎�M���H
          // ���W�X�^����f�[�^����M
          dt = RCREG0 ;
          x = (UART_Buffer_outptr + 1) % UART_BUFFER_SIZE ;
          if (x != UART_Buffer_inptr) {
               // �o�b�t�@�����t�łȂ��Ȃ�f�[�^��ۑ�����
               UART_Buffer[UART_Buffer_outptr] = dt ;
               // �o�b�t�@�������݃|�C���^�[�����ɐi�߂�
               UART_Buffer_outptr = x ;
          }

          // �I�[�o�����G���[����(RCREG�o�b�t�@���R�ڂ̃f�[�^����M������I�[�o�t���[)
          if (RCSTA_OERR == 1) {
               RCSTA_CREN = 0 ;
               while( RCSTA_OERR ) ;
               RCSTA_CREN = 1 ;
          }

          // �����ݎ�M�t���O�����Z�b�g
          RC0IF = 0 ;
     }
}
/*******************************************************************************
*  InitUART(rx,tx,brg)                                                         *
*    �t�r�`�q�s�ʐM�̏��������s������                                          *
*    rx  : ��M(RX)����s���̔ԍ����w�肷��                                    *
*    tx  : ���M(TX)����s���̔ԍ����w�肷��                                    *
*    brg : �{�[���[�g�W�F�l���[�^�̒l���w�肵�܂�(�ʐM���x�ɂȂ�܂�)          *
*          (�{�[���[�g�l��"207"�𒴂�����ABRGH=0/BRG16=1�ɐݒ肵�܂�)         *
*                    4MHz  8MHz 16MHz 32MHz 40MHz 48MHz 64MHz                  *
*          (SYNC=0 BRGH=1 BRG16=0)                                             *
*           9600bps   25    51   103   207    x     x     x                    *
*          19200bps   12    25    51   103   129   155   207                   *
*          (SYNC=0 BRGH=0 BRG16=1)                                             *
*           4800bps                    416   520   624   832                   *
*           9600bps                          259   311   416                   *
*******************************************************************************/
void InitUART(int rx, int tx, unsigned int brg)
{
#if defined(_12F1822)
     RXDTSEL = 0 ;                 // 6�ԃs��(RA1)��RX��M�s���Ƃ���
     TXCKSEL = 0 ;                 // 7�ԃs��(RA0)��TX���M�s���Ƃ���
     if (rx == 2) RXDTSEL = 1 ;    // 2�ԃs��(RA5)��RX��M�s���Ƃ���
     if (tx == 3) TXCKSEL = 1 ;    // 3�ԃs��(RA4)��TX��M�s���Ƃ���
#endif

#if defined(_16F1823) || defined(_16F1825)
     RXDTSEL = 0 ;                 //  5�ԃs��(RC5)��RX��M�s���Ƃ���
     TXCKSEL = 0 ;                 //  6�ԃs��(RC4)��TX���M�s���Ƃ���
     if (rx == 12) RXDTSEL = 1 ;   // 12�ԃs��(RA1)��RX��M�s���Ƃ���
     if (tx == 13) TXCKSEL = 1 ;   // 13�ԃs��(RA0)��TX��M�s���Ƃ���
#endif

#if defined(_16F1826) || defined(_16F1827)
     RXDTSEL = 0 ;                 //  7�ԃs��(RB1)��RX��M�s���Ƃ���
     TXCKSEL = 0 ;                 //  8�ԃs��(RB2)��TX���M�s���Ƃ���
     if (rx == 8)  RXDTSEL = 1 ;   //  8�ԃs��(RB2)��RX��M�s���Ƃ���
     if (tx == 11) TXCKSEL = 1 ;   // 11�ԃs��(RB5)��TX��M�s���Ƃ���
#endif

#if defined(_16F1829)
     RXDTSEL = 0 ;                 // 12�ԃs��(RB5)��RX��M�s���Ƃ���
     TXCKSEL = 0 ;                 // 10�ԃs��(RB7)��TX���M�s���Ƃ���
     if (rx == 5) RXDTSEL = 1 ;    //  5�ԃs��(RC5)��RX��M�s���Ƃ���
     if (tx == 6) TXCKSEL = 1 ;    //  6�ԃs��(RC4)��TX��M�s���Ƃ���
#endif

#if defined(UART_8O1) || defined(UART_8E1)
     // Data=8bit Parity=0dd/Even Stop=1bit Flow=none
     TXSTA0 = 0b01100100 ;         // ���M���ݒ�F�񓯊����[�h�@�X�r�b�g�E�p���e�B�L��
     RXSTA0 = 0b11010000 ;         // ��M���ݒ�
#else
     // Data=8bit Parity=none Stop=1bit Flow=none
     TXSTA0 = 0b00100100 ;         // ���M���ݒ�F�񓯊����[�h�@�W�r�b�g�E�m���p���e�B
     RXSTA0 = 0b10010000 ;         // ��M���ݒ�
#endif
     // �{�[���[�g�̐ݒ�
     if (brg <= 207) {
          BRGH0  = 1 ;
          BRG160 = 0 ;
          SPBRG0 = brg ;
     } else {
          BRGH0  = 0 ;
          BRG160 = 1 ;
          SPBRGH0 = (brg >> 8) ;
          SPBRG0  = brg & 0x00FF ;
     }
     RC0IF  = 0 ;                  // USART�����ݎ�M�t���O�̏�����
     RC0IE  = 1 ;                  // USART�����ݎ�M��L���ɂ���
     PEIE   = 1 ;                  // ���ӑ��u�����݂�L���ɂ���
     GIE    = 1 ;                  // �S�����ݏ�����������
     UART_Buffer_inptr  = 0 ;      // ��M�o�b�t�@�̓ǂݍ��݃|�C���^�[�̏�����
     UART_Buffer_outptr = 0 ;      // ��M�o�b�t�@�̏������݃|�C���^�[�̏�����
}
/*******************************************************************************
*  UART_Send(dt,num)                                                           *
*    ����Ɏw�肵�����̃f�[�^�𑗐M���鏈��                                  *
*                                                                              *
*    dt  : ���M����f�[�^���i�[�����z����w�肵�܂��B                          *
*    num : ���M����f�[�^�̌����w�肵�܂��B                                  *
*******************************************************************************/
void UART_Send(const char *dt,int num)
{
     union pari_t test ;
     int i , x ;

     // �w�肵�������J��Ԃ�
     for (i=0 ; i<num ; i++) {
#ifdef UART_8O1
          // ���1�A�����͂O�œ�����̂Ŕ��]���ăZ�b�g����(��p���e�B)
          test.dt = *dt ;
          x = test.b0 + test.b1 +  test.b2 + test.b3 + test.b4 + test.b5 + test.b6 + test.b7 ;
          TX9D0 = (x & 0x01) ^ 0x01 ; // ��p���e�B�̃Z�b�g
#endif
          while(TX0IF==0) ;   // ���M�\�ɂȂ�܂ő҂�
          TXREG0 = *dt++ ;    // ���M����
     }
}
/*******************************************************************************
*  UART_Write(dt)                                                              *
*    ����ɂP�o�C�g�̃f�[�^�𑗐M���鏈��                                      *
*                                                                              *
*    dt  : ���M����f�[�^���w�肵�܂��B                                        *
*******************************************************************************/
void UART_Write(char dt)
{
     union pari_t test ;
     int x ;

#ifdef UART_8O1
     // ���1�A�����͂O�œ�����̂Ŕ��]���ăZ�b�g����(��p���e�B)
     test.dt = dt ;
     x = test.b0 + test.b1 +  test.b2 + test.b3 + test.b4 + test.b5 + test.b6 + test.b7 ;
     TX9D0 = (x & 0x01) ^ 0x01 ; // ��p���e�B�̃Z�b�g
#endif
     while(TX0IF==0) ;        // ���M�\�ɂȂ�܂ő҂�
     TXREG0 = dt ;            // ���M����
}
/*******************************************************************************
*  ans = UART_Available()                                                      *
*    ��M�����f�[�^�̌���Ԃ�����                                            *
*                                                                              *
*    ans : ��M�����f�[�^�̌���Ԃ��܂��B                                    *
*******************************************************************************/
int UART_Available()
{
     return (UART_Buffer_outptr + UART_BUFFER_SIZE - UART_Buffer_inptr) % UART_BUFFER_SIZE ;
}
/*******************************************************************************
*  ans = UART_Read()                                                           *
*    ��M�����f�[�^���P�o�C�g�ǂݍ��ޏ���                                      *
*                                                                              *
*    ans : ��M�����f�[�^��Ԃ��܂�  0xffff��Ԃ������M�f�[�^�͋�ł��B      *
*******************************************************************************/
unsigned int UART_Read()
{
     unsigned int  ans ;

     // �o�b�t�@�͋�ł���
     if (UART_Buffer_inptr == UART_Buffer_outptr) return 0xffff ;
     // �o�b�t�@����ǂݏo��
     ans = UART_Buffer[UART_Buffer_inptr] ;
     // �o�b�t�@�ǂݍ��݃|�C���^�[�����ɐi�߂�
     UART_Buffer_inptr = (UART_Buffer_inptr + 1) % UART_BUFFER_SIZE ;

     return ans ;
}
/*******************************************************************************
*  UART_Flush()                                                                *
*    ��M�o�b�t�@���N���A���鏈��                                              *
*******************************************************************************/
void UART_Flush()
{
     UART_Buffer_inptr  = 0 ;      // ��M�o�b�t�@�̓ǂݍ��݃|�C���^�[�̏�����
     UART_Buffer_outptr = 0 ;      // ��M�o�b�t�@�̏������݃|�C���^�[�̏�����
}
