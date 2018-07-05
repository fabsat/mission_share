/*******************************************************************************
*  skUARTlib.h - �t�r�`�q�s�ʐM���s���֐��̃w�b�_�[�t�@�C��                    *
*                                                                              *
* ============================================================================ *
*   VERSION  DATE        BY                    CHANGE/COMMENT                  *
* ---------------------------------------------------------------------------- *
*   1.00     2013-01-20  ���ޒ��H�[(���ނ���)  Create                          *
*   2.00     2015-05-25  ���ޒ��H�[(���ނ���)  16F1825/1829 18F2xK22 �Ή�      *
*   3.01     2017-02-01  ���ޒ��H�[(���ނ���)  �p���e�B(ODD)�Ή�               *
*   3.10     2017-02-04  ���ޒ��H�[(���ނ���)  �ᑬ�ʐM���x�ɑΉ�              *
*******************************************************************************/
#ifndef _SKUARTLIB_H_
#define _SKUARTLIB_H_

#define UART_8N1                   // Data=8bit Parity=none Stop=1bit Flow=none (�W��)
//#define UART_8O1                   // Data=8bit Parity=0dd Stop=1bit Flow=none (��p���e�B�L��))

#define UART_BUFFER_SIZE   128     // �t�r�`�q�s�̎�M�o�b�t�@�T�C�Y

//#define UART2_USE                  // USART2�𗘗p����ꍇ�̓R�����g���O���܂��B
// �t�r�`�q�s���W���[���̃��W�X�^�[��`
#ifdef  UART2_USE
// USART�Q�𗘗p����ꍇ�̒�`
#define TXSTA0  TXSTA2
#define TXREG0  TXREG2
#define RXSTA0  RCSTA2
#define RXREG0  RCREG2
#define SPBRG0  SPBRG2
#define SPBRGH0 SPBRGH2
#define RCREG0  RCREG2
#define BRGH0   BRGH2
#define BRG160  BRG162
#define TX9D0   TX9D2
#define RC0IF   RC2IF
#define RC0IE   RC2IE
#define TX0IF   TX2IF
#define RCSTA_OERR  OERR2
#define RCSTA_CREN  CREN2
#else
// USART�P�𗘗p����ꍇ�̒�`
#if defined(_18F25K22) || defined(_18F26K22)
#define TXSTA0  TXSTA1
#define TXREG0  TXREG1
#define RXSTA0  RCSTA1
#define RXREG0  RCREG1
#define SPBRG0  SPBRG1
#define SPBRGH0 SPBRGH1
#define RCREG0  RCREG1
#define BRGH0   BRGH1
#define BRG160  BRG161
#define TX9D0   TX9D1
#define RC0IF   RC1IF
#define RC0IE   RC1IE
#define TX0IF   TX1IF
#define RCSTA_OERR  OERR1
#define RCSTA_CREN  CREN1
#else
// USART�𗘗p����ꍇ�̒�`
#define TXSTA0  TXSTA
#define TXREG0  TXREG
#define RXSTA0  RCSTA
#define RXREG0  RCREG
#define SPBRG0  SPBRG
#define SPBRGH0 SPBRGH
#define RCREG0  RCREG
#define BRGH0   BRGH
#define BRG160  BRG16
#define TX9D0   TX9D
#define RC0IF   RCIF
#define RC0IE   RCIE
#define TX0IF   TXIF
#define RCSTA_OERR  OERR
#define RCSTA_CREN  CREN
#endif
#endif

// �p���e�B�`�F�b�N�p�\���̂̒�`
union  pari_t {
  unsigned int  dt ;
  struct {
    unsigned b0     :1;
    unsigned b1     :1;
    unsigned b2     :1;
    unsigned b3     :1;
    unsigned b4     :1;
    unsigned b5     :1;
    unsigned b6     :1;
    unsigned b7     :1;
  } ;
} ;

// �֐��̃v���g�^�C�v�錾
void InterUART( void ) ;
void InitUART(int rx, int tx, unsigned int brg) ;
void UART_Send(const char *dt,int num) ;
void UART_Write(char dt) ;
int  UART_Available() ;
unsigned int UART_Read() ;
void UART_Flush() ;

#endif
