/*******************************************************************************
*  skSDlib.c - �l�l�b�^�r�c�J�[�h�A�N�Z�X�֐����C�u����                        *
*                                                                              *
*    SD_Init     - �l�l�b�^�r�c�b�̏��������s������                            *
*    SD_Open     - �t�@�C���̃I�[�v�����s������                                *
*    SD_Close    - �t�@�C���̃N���[�Y���s������                                *
*    SD_Write    - �t�@�C���֎w�肵���o�C�g�������������ޏ���                  *
*    SD_Read     - �t�@�C������w�肵���o�C�g�������ǂݍ��ޏ���                *
*    SD_fGets    - �t�@�C������P�s�ǂݍ��ޏ���                                *
*    SD_Size     - �t�@�C���̃T�C�Y�𓾂鏈��                                  *
*    SD_Position - �t�@�C���̓Ǐ����|�C���^�l�𓾂鏈��                        *
*    SD_Seek     - �t�@�C���̓Ǐ����|�C���^���ړ������鏈��                    *
*                                                                              *
*    �����F�W��SD�EMicroSD(FAT16)/�W��SDHC�EMicroSDHC(FAT32)�̂ݓǏ����n�j     *
*          "skSDlib.h"��SPI��CS(�J�[�h�I��M��)���L��̂Ńs���̊��t�����s�����B*
*                                                                              *
* ============================================================================ *
*  VERSION DATE        BY                    CHANGE/COMMENT                    *
* ---------------------------------------------------------------------------- *
*  1.00    2012-04-30  ���ޒ��H�[(���ނ���)  Create                            *
*  2.00    2012-05-03  ���ޒ��H�[(���ނ���)  SDHC�ɑΉ�                        *
*  2.01    2012-06-09  ���ޒ��H�[(���ނ���)  �W��SD/SDHC��OK�R�����g��ύX     *
*  2.02    2014-02-02  ���ޒ��H�[(���ނ���)  XC8 C Compiler �Ή��ɕύX         *
*  3.00    2014-09-01  ���b�V���񂩂�̕�  �s��C��                        *
*  3.01    2014-09-03  ���ޒ��H�[(���ނ���)  �t�@�C����8�����s�s����C��   *
*  3.02    2014-09-03  ���ޒ��H�[(���ނ���)  Seek()�̃t���O4�̏�����ύX       *
*  4.00    2017-02-22  ���ޒ��H�[(���ނ���)  XC16�ƕ��������I�[�v���ɑΉ�      *
* ============================================================================ *
*  PIC 16F1938 18F2xK22 24EP256MC202                                           *
*  (���Ԃ񂻂̑���PIC�ł�SRAM��768byte�ȏ�L��΂��̊֐��͗��p�\�Ǝv��)      *
*  MPLAB X IDE v3.50                                                           *
*  MPLAB(R) XC8  C Compiler Version 1.32/1.40                                  *
*  MPLAB(R) XC16 C Compiler Version 1.30                                       *
*******************************************************************************/
#include <xc.h>
#include <string.h>
//#include "skMonitorLCD.h"
#include "skSPIlib.h"
#include "skSDlib.h"


char MMCbuffer[SECTER_BYTES] ;          // �l�l�b�J�[�h�Ǐ����o�b�t�@
int  CardType ;                         // �J�[�h�̎��
int  FatType ;                          // �e�`�s�̎��

//�e�`�s�t�@�C���V�X�e���̃p�����[�^
unsigned long Dir_Entry_StartP ;        // �f�B���N�g���G���g���̊J�n�Z�N�^�ʒu
unsigned int  DirEntry_SectorSU ;       // �f�B���N�g���G���g���̃Z�N�^��
unsigned long Data_Area_StartP ;        // �f�[�^�̈�̊J�n�Z�N�^�ʒu
unsigned long Fat_Area_StartP ;         // FAT�̈�̊J�n�Z�N�^�ʒu
unsigned int  Cluster1_SectorSU ;       // �P�N���X�^������̃Z�N�^��
unsigned long SectorsPerFatSU ;         // �P�g��FAT�̈悪��߂�Z�N�^��


////////////////////////////////////////////////////////////////////////////////
// �l�l�b�^�r�c�b�̃J�[�h��I���ACS�M����HIGH/LOW���o�͂��鏈��
void cs_select(int flag)
{
     CS = flag ;
     SPI_transfer(0xff) ;     // �m���ɂb�r��ؑւ���ׂɃN���b�N�𑗐M���Ă���
}
////////////////////////////////////////////////////////////////////////////////
// ���f�B��Ԃ̃`�F�b�N���s������
int ready_check()
{
     int c ;
     
     c = 0 ;
     while (SPI_transfer(0xff) != 0xff && c<=500) {
          c++ ;
          __delay_ms(1) ;
     }
     if (c>=500) return 1 ;    // �^�C���A�E�g
     else        return 0 ;    // �n�j�ł�
}
////////////////////////////////////////////////////////////////////////////////
// �l�l�b�^�r�c�b�փR�}���h�𑗂鏈��
int send_command(unsigned char cmd, unsigned long arg)
{
     int x , ans ;

     cs_select(LOW) ;
     ans = ready_check() ;
     if (ans != 0) return 0xff00 ;           // �J�[�h�� Busy �炵��
     // �R�}���h�̑��M���s��
     SPI_transfer(cmd | 0x40) ;                             // �R�}���h�̑��M
     for (x=24 ; x >= 0 ; x -= 8) SPI_transfer(arg >> x) ;  // �p�����[�^�̑��M
     x = 0XFF ;
     if (cmd == CMD0) x = 0X95 ;
     if (cmd == CMD8) x = 0X87 ;
     SPI_transfer(x) ;                                      // CRC�̑��M
     // �R�}���h�̕ԓ���҂������s��
     x = 0 ;
     do {
          ans = SPI_transfer(0xff) ;
          x++;
     } while(ans & 0x80 && x < 256) ;
    if (x >= 256) ans = (cmd|0x40) << 8 ;    // ���X�|���X�^�C���A�E�g

    return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �w��̃Z�N�^�ʒu����f�[�^���P�u���b�N(�V���O���E���C�g)�����ޏ���
int sblock_write(unsigned long sector,char *buff)
{
     int i , ans ;

     if (CardType != 0x13) sector <<= 9 ;         // SDHC�łȂ�
     ans = send_command(CMD24, sector) ;          // �V���O���E���[�h�R�}���h�̔��s
     if (ans == 0) {
          SPI_transfer(0xfe) ;                    // �f�[�^�g�[�N���̑��M
          for (i=0 ; i<SECTER_BYTES ; i++) {
               SPI_transfer(*buff) ;              // �f�[�^���̑��M
               buff++ ;
          }
          SPI_transfer(0xff) ;                    // CRC�̕����𑗐M
          SPI_transfer(0xff) ;
          ans = SPI_transfer(0xff) ;              // ���X�|���X�̎�M
          ans = ans & 0x1F ;
		if (ans == 0x05) {
               ans = ready_check() ;
               if (ans != 0) ans = 0x8800 ;       // �J�[�h�� Busy �炵��
               else {
                    // �����݂�����ɍs��ꂽ���₢���킹���s��
                    ans = send_command(CMD13, sector) ;
                    i  = SPI_transfer(0xff) ;
                    if (ans == 0) {
                         if (i != 0) ans = 0x8900|ans ; // �����ݎ��s���Ă���
                    }
               }
          } else ans = 0x8800 | ans ;             // �������݃G���[
     } else ans = CMD24 << 8 | ans ;              // CMD24�G���[
     cs_select(HIGH) ;

     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �w��̃Z�N�^�ʒu����f�[�^���P�u���b�N(�V���O���E���[�h)�Ǎ��ޏ���
int sblock_read(unsigned long sector,char *buff)
{
     int i , ans ;

     if (CardType != 0x13) sector <<= 9 ;         // SDHC�łȂ�
     ans = send_command(CMD17, sector) ;          // �V���O���E���[�h�R�}���h�̔��s
     if (ans == 0) {
          i = 0 ;
          // �ԐM�f�[�^�̂P�o�C�g�ڂ�҂�
          while(1) {
               ans = SPI_transfer(0xff) ;
               i++ ;
               if (ans != 0xff || i>=1000) {
                    if (i>=1000) ans = 0x8600 ;   // �f�[�^�g�[�N���^�C���A�E�g
                    break ;
               }
               __delay_ms(1) ;
          }
          // �Q�o�C�g�ڈȍ~�̎�M
          if (ans == 0xfe) {
               for (i=0 ; i<SECTER_BYTES ; i++) {
                    *buff = SPI_transfer(0xff) ;
                     buff++ ;
               }
               SPI_transfer(0xff) ;               // CRC�̕�������M
               SPI_transfer(0xff) ;
               ans = 0 ;
          } else ans = 0x8600 | ans ;             // �ǂݍ��݃G���[
     } else ans = CMD17 << 8 | ans ;              // CMD17�G���[
     cs_select(HIGH) ;

     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �e�`�s�t�@�C���V�X�e���̃p�����[�^��Ǎ��ޏ���
int fat_para_read()
{
     union {
          unsigned char c[4] ;
          unsigned long l ;
     } dt ;
     struct FAT_PARA *fat ;
     int i , ans ;

     // �J�[�h�̐擪����T�P�Q�o�C�g�ǂݏo��
     ans = sblock_read(0,MMCbuffer) ;
     if (ans == 0) {
          // BPB(�u�[�g�Z�N�^)�܂ł̃I�t�Z�b�g�l����o��
          for (i=0 ; i<4 ; i++) {
               dt.c[i] = MMCbuffer[i+0x1c6] ;
          }
          // �e�`�s�t�@�C���V�X�e���̃p�����[�^��Ǎ���
          ans = sblock_read(dt.l,MMCbuffer) ;
          if (ans == 0) {
               fat = (struct FAT_PARA *)MMCbuffer ;
               // �P�N���X�^������̃Z�N�^��
               Cluster1_SectorSU = fat->SectorsPerCluster ;
               // �P�g��FAT�̈悪��߂�Z�N�^��
               if (fat->SectorsPerFat16 != 0) {
                    FatType = 2 ;
                    SectorsPerFatSU = fat->SectorsPerFat16 ;
               } else {
                    FatType = 4 ;
                    SectorsPerFatSU = fat->SectorsPerFat32 ;
               }
               // �e�`�s�̈�̊J�n�Z�N�^�ʒu
               Fat_Area_StartP = fat->ReservedSectorCount + dt.l ;
               // ���[�g�f�B���N�g���G���g���̊J�n�Z�N�^�ʒu
               Dir_Entry_StartP = Fat_Area_StartP + (SectorsPerFatSU * fat->FatCount) ;
               // �f�[�^�̈�̊J�n�Z�N�^�ʒu
               DirEntry_SectorSU = fat->RootDirEntryCount / (SECTER_BYTES / sizeof(struct DIR_ENTRY)) ;
               Data_Area_StartP  = Dir_Entry_StartP + DirEntry_SectorSU ;
               if (FatType == 4) DirEntry_SectorSU = Cluster1_SectorSU ;   // FAT32
          }
     }
     return ans ;
}
/*******************************************************************************
*  ans = SD_Init()                                                             *
*    �l�l�b�^�r�c�b�̏��������s������                                          *
*    �W���^�C�vSD/SDHC��MicroSD/SDHC��FAT16/FAT32�̂ݗ��p�\                  *
*                                                                              *
*    ans  : ���큁�O  �ُ큁�O�ȊO�̃G���[�R�[�h�l                             *
*******************************************************************************/
int SD_Init()
{
     unsigned long arg ;
     unsigned char r7[4] ;
     unsigned int i , ans ;

     CS       = 1 ;
     CardType = 0 ;

     // CLK��74�N���b�N�ȏ㑗�M���A�J�[�h���R�}���h�҂���Ԃɂ���
     for (i = 0 ; i < 10 ; i++) SPI_transfer(0xFF) ;

     // �J�[�h�Ƀ��Z�b�g�R�}���h(CMD0)�𑗐M����(SPI���[�h�Ɉڍs)
     ans = send_command(CMD0, 0) ;
     if (ans == 1) {
//          MonitorPuts("CMD0") ;
          // �r�c�b�p�̏������������s��
          arg = 0 ;
          // ����d���̊m�F�ƃJ�[�h�̃o�[�W�����`�F�b�N
          ans = send_command(CMD8, 0x1AA) ;
          if (ans == 1) {
               for (i=0 ; i<4 ; i++) r7[i] = SPI_transfer(0xff) ;
               if (r7[3] == 0xAA) {
                    CardType = 0x12 ;                  // SDCver.2�̃J�[�h
                    arg = 0X40000000 ;
               } else ans = 0x8200 ;                   // CMD8�̃G���[
          } else {
               if (ans & 0x4) CardType = 0x11 ;        // SDCver.1�̃J�[�h
          }
          if (CardType != 0) {
//               MonitorPuts(":8") ;
               // �r�c�b�p�̏������R�}���h(ACMD41)�𔭍s����
               i = 0 ;
               while(1) {
                    ans = send_command(CMD55, 0) ;
                    ans = send_command(ACMD41, arg) ;
                    i++ ;
                    if (ans == 0 || i>=2000) {
                         if (i>=2000) ans = 0x8300|ans ;    // ACMD41�^�C���A�E�g
                         break ;
                    }
                    __delay_ms(1) ;
               }
//               if (ans == 0) MonitorPuts(":A41") ;
               if (ans == 0 && CardType == 0x12) {
                    // Ver.2�Ȃ�OCR�̓Ǐo���R�}���h(CMD58)�𔭍s����
                    ans = send_command(CMD58, 0) ;
                    if (ans == 0) {
//                         MonitorPuts(":58") ;
                         for (i=0 ; i<4 ; i++) r7[i] = SPI_transfer(0xff) ;
                         if (r7[0] & 0x40) CardType = 0x13 ;// SDHC�̃J�[�h
                    } else ans = CMD58 << 8 | ans ;         // CMD58�G���[
               }
          } else {
               // �l�l�b�p�̏������R�}���h(CMD1)�𔭍s����
               i = 0 ;
               while(1) {
                    ans = send_command(CMD1, 0) ;
                    i++ ;
                    if (ans != 1 || i>=2000) {
                         if (i>=2000) ans = 0x8100 ;   // �������^�C���A�E�g
                         break ;
                    }
                    __delay_ms(1) ;
               }
               if (ans == 0) {
                    CardType = 0x01 ;                  // MMC�̃J�[�h
//                    MonitorPuts(":1") ;
               } else {
                    if (ans != 0x8100) ans = CMD1 << 8 | ans ;   // CMD1�G���[
               }
          }
          if (ans == 0) {
               // �u���b�N�T�C�Y�̏����l���T�P�Q�o�C�g�ɍĐݒ肷��
               ans = send_command(CMD16, SECTER_BYTES) ;
               if (ans != 0) ans = CMD16 << 8 | ans ;  // CMD16�̃G���[
//               else MonitorPuts(":16") ;
          }
     } else ans = CMD0 << 8 | ans ;                    // CMD0�G���[
     cs_select(HIGH) ;
     
     // ������������ɏI��������e�`�s�t�@�C���V�X�e���̃p�����[�^��Ǎ���
     if (ans == 0) {
          ans = fat_para_read() ;
     }

//          MonitorPuts("CardType = ") ;
//          MonitorPuth(CardType,HEX) ;

     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �w��t�@�C���̃G���g�����𓾂�A�Ȃ���΃G���g���̋�̏ꏊ�𓾂鏈��
int search_file(char *filename,struct SDFILE_OBJECT *fp)
{
     struct DIR_ENTRY *inf ;
     int i , j , c , x , ans ;

     ans = c = 0 ;
     fp->DirEntryIndex = 0 ;
     // �f�B���N�g���G���g���̑S�Z�N�^�������J��Ԃ�
     for (i=0 ; i<DirEntry_SectorSU ; i++) {
          // �f�B���N�g���G���g���̃f�[�^��Ǎ���
          ans = sblock_read(Dir_Entry_StartP+i,MMCbuffer) ;
          if (ans == 0) {
               // �P�Z�N�^���̃G���g���̌��������J��Ԃ�
               x = SECTER_BYTES / sizeof(struct DIR_ENTRY) ;
               for (j=0 ; j<x ; j++ ) {
                    c++ ;
                    // �t�@�C���̃G���g���𒲂ׂ�
                    inf = (struct DIR_ENTRY *)&MMCbuffer[j*sizeof(struct DIR_ENTRY)] ;
                    if (inf->FileName[0] == 0x2e) continue ;// �T�u�f�B���N�g��
                    if (inf->FileName[0] == 0xe5) {
                         if (fp->DirEntryIndex == 0) fp->DirEntryIndex = c ;
                         continue ;                         // �폜���ꂽ�G���g��
                    }
                    if (inf->FileName[0] == 0x00) {
                         if (fp->DirEntryIndex == 0) fp->DirEntryIndex = c ;
                         i = DirEntry_SectorSU ;
                         ans = 2 ;                          // ��������Ȃ�����
                         break ;                            // ��̃G���g��
                    }
                    // �t�@�C���̑����𒲂ׂ�
                    // 0x00��0x20�łȂ��Ȃ畁�ʂ̃t�@�C���łȂ��Ƃ���
                    if (!((inf->Attributes==0x20)||(inf->Attributes==0x00))) continue ;
                    // �t�@�C�������r����
                    if (memcmp(inf->FileName,filename,11) == 0) {
                         fp->DirEntryIndex = c ;
                         // �t�@�C���̃A�N�Z�X����ݒ肷��
                         fp->FileSeekP = 0 ;               // �t�@�C���̓Ǎ��ވʒu�͂O
                         fp->FileSize = inf->FileSize ;    // �t�@�C���̃T�C�Y
                         // �f�[�^�i�[���FAT�ԍ����L�^
                         fp->FirstFatno = inf->FirstClusterHigh ;
                         fp->FirstFatno = (fp->FirstFatno<<16) | inf->FirstClusterLow ;
                         i = DirEntry_SectorSU ;
                         ans = 1 ;
                         break ;                            // ����I��
                    }
               }
          } else ans = -1 ;
     }

     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �󂫂̂e�`�s�ԍ���T������
unsigned long search_fat()
{
     unsigned long ans ;
     unsigned int i ;
     int j , x , k ;

     ans = 0 ;
     // �P�g�̑��e�`�s�����J��Ԃ�
     for (i=0 ; i<SectorsPerFatSU ; i++) {
          if (sblock_read(Fat_Area_StartP+i,MMCbuffer) == 0) {
               for (j=0 ; j<SECTER_BYTES ; j=j+FatType) {
                    // �󂫂̂e�`�s���L��������
                    x = 0 ;
                    for (k=0 ; k<FatType ; k++) x = x | MMCbuffer[j+k] ;
                    if (x == 0) {
                         MMCbuffer[j]   = 0xff ;
                         MMCbuffer[j+1] = 0xff ;
                         if (FatType == 4) {    // FAT32
                              MMCbuffer[j+2] = 0xff ;
                              MMCbuffer[j+3] = 0x0f ;
                         }
                         // �\��X�V
                         if (sblock_write(Fat_Area_StartP+i,MMCbuffer) == 0) {
                              // �Q�g�ڂ�FAT��������
                              sblock_write((Fat_Area_StartP+i)+SectorsPerFatSU,MMCbuffer) ;
                              ans = (i * SECTER_BYTES + j) / FatType ;
                         }
                         i = SectorsPerFatSU ;
                         break ;  // �����I��
                    }
               }
          } else break ;
     }
     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �f�B���N�g���G���g���̍X�V����
int direntry_make(unsigned long no,char *filename,struct SDFILE_OBJECT *fp)
{
     struct DIR_ENTRY *inf ;
     unsigned long p ;
     unsigned int x , y ;
     int ans ;

     ans = -1 ;
     // �f�B���N�g���G���g����Ǎ���
     x = fp->DirEntryIndex - 1 ;
     y = SECTER_BYTES / sizeof(struct DIR_ENTRY) ;
     p = Dir_Entry_StartP + (x / y) ;
     if (sblock_read(p,MMCbuffer) == 0) {
          inf = (struct DIR_ENTRY *)&MMCbuffer[(x % y) * sizeof(struct DIR_ENTRY)] ;
          if (no != 0) {
               memset(inf,0x00,sizeof(struct DIR_ENTRY)) ;
               // �t�@�C������ݒ肷��
               memcpy(inf->FileName,filename,11) ;
               // �t�@�C���̑�����ݒ肷��
               inf->Attributes = 0x20 ;
               // �t�@�C���̐V�K�쐬
               inf->FileSize = 0 ;
               // �t�@�C���̍쐬���Ԃ�ݒ肷��
               inf->CreationTimeTenths = 0 ;
               inf->CreationTime       = 0 ;
               inf->LastWriteTime      = 0 ;
               // �t�@�C���̍쐬����ݒ肷��
               inf->CreationDate  = 0 ;
               inf->LastWriteDate = 0 ;
               // �A�N�Z�X����ݒ肷��
               inf->LastAccessDate = 0 ;
               // �f�[�^�i�[���FAT�ԍ���ݒ肷��
               inf->FirstClusterHigh = (unsigned int)(no >> 16) ;
               inf->FirstClusterLow  = (unsigned int)(no & 0x0000ffff) ;
               // �t�@�C���̃A�N�Z�X����ݒ肷��
               fp->FileSeekP = 0 ;
               fp->FileSize  = 0 ;
               // �f�[�^�i�[���FAT�ԍ����L�^
               fp->FirstFatno = inf->FirstClusterHigh ;
               fp->FirstFatno = (fp->FirstFatno<<16) | inf->FirstClusterLow ;
          } else {
               // �t�@�C���̃T�C�Y���X�V����
               inf->FileSize = inf->FileSize + fp->AppendSize ;
               // �t�@�C�������ݓ�����ݒ肷��
               
               // �A�N�Z�X����ݒ肷��
               
          }
          // �f�B���N�g���G���g���̍X�V
          ans = sblock_write(p,MMCbuffer) ;
     }
     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �t�@�C�����̐��`���s������
void filename_check(char *c,const char *filename)
{
     int i ;

     memset(c,0x20,11) ;
     for (i=0 ; i<8 ; i++) {
          if (*filename == '.') {
               c = c + (8-i) ;
               filename++ ;
               break ;
          }
          *c = *filename ;
          c++ ;
          filename++ ;
     }
     if (i > 7) filename++ ;
     for (i=0 ; i<3 ; i++) {
          if (*filename == 0x00) break ;
          *c = *filename ;
          c++ ;
          filename++ ;
     }
}
/*******************************************************************************
*  ans = SD_Open(fp,filename,oflag)                                            *
*    �t�@�C���̃I�[�v�����s������                                              *
*    *fp      : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                *
*    *filename: �t�@�C����(xxxxxxxx.xxx)���w�肵�܂�                           *
*               �g���q�͕K���w��A�܂��p�����͑啶�����w�� �B                  *
*    oflag    : �A�N�Z�X�̃t���O���w�肵�܂�                                   *
*               O_READ   �ǂݍ��ݐ�p(�t�@�C�������݂��Ȃ����̓G���[)          *
*               O_APPEND �ǉ������ݐ�p(�t�@�C���̍Ō�ɒǉ����܂�)            *
*               O_RDWR   �ǂݏ����\(�t�@�C�����������͍쐬����܂�)          *
*                                                                              *
*    ans  : ���큁�O  �ُ큁�|�P                                               *
*******************************************************************************/
int SD_Open(struct SDFILE_OBJECT *fp,const char *filename,int oflag)
{
     unsigned long no ;
     char c[11] ;
     int  ans , ret ;

     ret = -1 ;
//     if (fp->Oflag != 0) return ret ;        // ���ɃI�[�v������Ă���
     fp->Oflag = 0 ;
     // �w��̃t�@�C�����𐬌`����
     filename_check(c,filename) ;
     // �w�肳�ꂽ�t�@�C������������
     ans = search_file(c,(struct SDFILE_OBJECT *)fp) ;
     if (ans > 0) {
          if (oflag == O_READ && ans == 1) ret = 0 ;
          if (oflag == O_APPEND && ans == 1) {
               // �t�@�C���̃A�N�Z�X�ʒu���t�@�C���̍Ō�{�P�ɐݒ肷��
               fp->FileSeekP = fp->FileSize ;
               ret = 0 ;
          }
          if (oflag == O_RDWR && (ans == 1 || ans == 2)) {
               if (ans == 2) {
                    // �V�K�t�@�C���̍쐬���s��
                    if (fp->DirEntryIndex != 0) {
                         // �󂫂�FAT��T���Ċm�ۂ���
                         no = search_fat() ;
                         if (no != 0) {
                              // �f�B���N�g���G���g���̍쐬���s��
                              if (direntry_make(no,c,(struct SDFILE_OBJECT *)fp) == 0) ret = 0 ;
                         }
                    }
               } else ret = 0 ;
          }
          if (ret == 0) {
               // �t�@�C���ɃA�N�Z�X�\�Ƃ���
               fp->Oflag = oflag ;
               fp->AppendSize = 0 ;
          }
     }

     return ret ;
}
/*******************************************************************************
*  SD_Close(fp)                                                                *
*    �t�@�C���̃N���[�Y���s������                                              *
*    �f�[�^��ǉ��������̃t�@�C���T�C�Y�����̊֐��ŏ������܂�܂��B            *
*    *fp : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                     *
*******************************************************************************/
void SD_Close(struct SDFILE_OBJECT *fp)
{
     // �������݃I�[�v�����̏���
     if (fp->Oflag & (O_APPEND | O_RDWR)) {
          if (fp->AppendSize == 0) return ;
          // �f�B���N�g���G���g���̍X�V���s��
          direntry_make(0,0,(struct SDFILE_OBJECT *)fp) ;
     }
     // �t�@�C���̃A�N�Z�X��������������
     fp->Oflag = 0 ;
}
////////////////////////////////////////////////////////////////////////////////
// �w�肳�ꂽFAT�ԍ��̎���FAT�ԍ��𓾂鏈��
unsigned long next_fat_read(unsigned long fatno,struct SDFILE_OBJECT *fp)
{
     union {
          unsigned char c[4] ;
          unsigned long i ;
     } no ;
     unsigned long p , x , y , ans ;
     int j ;

     // �e�`�s�̈�̃f�[�^��Ǎ���
     p = Fat_Area_StartP + (fatno / (SECTER_BYTES/FatType)) ;
     ans = sblock_read(p,MMCbuffer) ;
     if (ans == 0) {
          x = (fatno % (SECTER_BYTES/FatType)) * FatType ;
          no.i = 0 ;
          for (j=0 ; j<FatType ; j++) no.c[j] = MMCbuffer[x+j] ;
          // ���̃`�F�[����FAT�ԍ��𓾂�
          ans = no.i ;
          if (FatType == 4) y = 0x0fffffff ;  // FAT32
          else              y = 0xffff ;
          // ���̃`�F�[���悪�Ȃ����͐V�K�Ƀ`�F�[����e�`�s���쐬����
          if (y == ans) {
               ans = search_fat() ;     // �V�e�`�s�ԍ��̋󂫂�T��
               if (ans != 0) {
                    // �`�F�[�����̂e�`�s�����X�V����
                    if (sblock_read(p,MMCbuffer) == 0) {
                         no.i = ans ;
                         for (j=0 ; j<FatType ; j++) MMCbuffer[x+j] = no.c[j] ;
                         if (sblock_write(p,MMCbuffer) == 0) {
                              // �Q�g�ڂ�FAT��������
                              sblock_write(p + SectorsPerFatSU,MMCbuffer) ;
                         } else ans = 0 ;
                    } else ans = 0 ;
               }
          }
     } else ans = 0 ;

     return ans ;
}
////////////////////////////////////////////////////////////////////////////////
// �V�[�N�ʒu����FAT�ԍ�(�N���X�^�ԍ�)���Z�o���鏈��
void fatno_seek_conv(unsigned long *fatno,struct SDFILE_OBJECT *fp)
{
     unsigned int  p ;
     int  i ;

     // �f�[�^�̃V�[�N�ʒu����Ǐo�_���Z�N�^�ʒu���Z�o�����Ԗڂ̘_���N���X�^�ʒu���H
     p = (fp->FileSeekP / SECTER_BYTES) / Cluster1_SectorSU ;
     // FAT�̈���_���N���X�^������ۂ�FAT�ԍ�(�N���X�^�ԍ�)���Z�o����
     *fatno = fp->FirstFatno ;
     for (i=0 ; i<p ; i++) {
          // ���̃`�F�[����FAT�ԍ���Ǎ���ł���
          *fatno = next_fat_read(*fatno,(struct SDFILE_OBJECT *)fp) ;
     }
}
////////////////////////////////////////////////////////////////////////////////
// ���̊֐������ۂɃt�@�C������w�肵���o�C�g�������ǂݍ��ޏ���
// type : 1=SD_Read����̃R�[��  2=SD_fGets����̃R�[��  3=SD_Write����̃R�[��
int sd_rdwr(char *buf,int nbyte,int type,struct SDFILE_OBJECT *fp)
{
     unsigned long dtSP ;
     unsigned long fatno ;
     unsigned int  p , x ;
     int  i , c , ans ;

     // �V�[�N�ʒu����FAT�ԍ�(�N���X�^�ԍ�)���Z�o
     fatno_seek_conv(&fatno,(struct SDFILE_OBJECT *)fp) ;
     if (fatno == 0) return -1 ;                       // FAT�̈�Ǎ��݃G���[
     // �f�[�^�̐擪�Z�N�^�ʒu�̎Z�o
     dtSP = Data_Area_StartP + ((fatno - 2) * Cluster1_SectorSU) ;
     p = (fp->FileSeekP / SECTER_BYTES) % Cluster1_SectorSU ; // �N���X�^���̃Z�N�^�ʒu
     // �f�[�^�̈悩��t�@�C�����e��Ǐo��
     ans = sblock_read(dtSP+p,MMCbuffer) ;
     if (ans == 0) {
          x = fp->FileSeekP % SECTER_BYTES ;
          c = 0 ;
          // �w��o�C�g���Ԃ�J��Ԃ�
          for (i=0 ; i<nbyte ; i++ ) {
               if (type == 3) MMCbuffer[x] = *buf ;    // ������
               else           *buf = MMCbuffer[x] ;    // �Ǎ���
               c++ ;
               x++ ;
               fp->FileSeekP++ ;
               if (fp->FileSeekP >= fp->FileSize) {
                    if (type < 3) break ;              // �Ō�܂œǂݍ���
                    fp->AppendSize++ ;                 // �f�[�^���ǉ����ꂽ�������J�E���g
               }
               if (c >= SECTER_BYTES) break ;          // SECTER_BYTES������������
               if (type == 2 && *buf == 0x0a) break ;  // �k�e�Ȃ�I��
               // ���̃Z�N�^�Ƀf�[�^���܂��������ꍇ�̏���
               if (x >= SECTER_BYTES) {
                    // �����ݗv���Ȃ炱���܂ł̃f�[�^��������
                    if (type == 3) {
                         ans = sblock_write(dtSP+p,MMCbuffer) ;
                         if (ans != 0) {
                              ans = -1 ;               // �f�[�^�̈揑���݃G���[
                              break ;
                         }
                    }
                    p++ ;
                    if (p >= Cluster1_SectorSU) {
                         // ���̃N���X�^�Ƀf�[�^���L��悤�ł���
                         // �V�[�N�ʒu���玟��FAT�ԍ�(�N���X�^�ԍ�)���Z�o
                         fatno_seek_conv(&fatno,(struct SDFILE_OBJECT *)fp) ;
                         if (fatno == 0) {
                              ans = -1 ;               // FAT�̈�Ǎ��݃G���[
                              break ;
                         }
                         // �f�[�^�̐擪�Z�N�^�ʒu�̎Z�o
                         dtSP = Data_Area_StartP + ((fatno - 2) * Cluster1_SectorSU) ;
                         p = (fp->FileSeekP / SECTER_BYTES) % Cluster1_SectorSU ; // �N���X�^���̃Z�N�^�ʒu
                    }
                    // ���̃u���b�N��Ǎ���
                    ans = sblock_read(dtSP+p,MMCbuffer) ;
                    if (ans == 0) x = 0 ;
                    else {
                         ans = -1 ;                    // �f�[�^�̈�Ǎ��݃G���[
                         break ;
                    }
               }
               buf++ ;
          }
          // �����ݗv���Ȃ�f�[�^��������
          if (x != 0 && ans != -1 && type == 3) {
               ans = sblock_write(dtSP+p,MMCbuffer) ;
               if (ans != 0) ans = -1 ;                // �f�[�^�̈揑���݃G���[
          }
          if (ans != -1) ans = c ;                     // �Ǐ����񂾃o�C�g����Ԃ�
     } else ans = -1 ;                                 // �f�[�^�̈�Ǎ��݃G���[

     return ans ;
}
/*******************************************************************************
*  ans = SD_Write(fp,buf,nbyte)                                                *
*    �t�@�C���֎w�肵���o�C�g�������������ޏ���                                *
*    �t�@�C���̐擪���珑���܂�܂��A�����ވʒu��ς���ꍇ��SD_Seek���g�p     *
*    *fp   : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                   *
*    *buf  : �����ރf�[�^���i�[�����z��ϐ����w�肵�܂�                        *
*    nbyte : �����ރf�[�^�̃o�C�g�����w�肵�܂�                                *
*                                                                              *
*    ans  : ���큁�����񂾃o�C�g��  �ُ큁�|�P                                 *
*******************************************************************************/
int SD_Write(struct SDFILE_OBJECT *fp,const char *buf,int nbyte)
{
     char *p ;
     
     p = (char *)buf ;
     if (!(fp->Oflag & (O_APPEND | O_RDWR))) return -1 ; // �����݃I�[�v���łȂ��G���[
     return sd_rdwr(p,nbyte,3,(struct SDFILE_OBJECT *)fp) ;
}
/*******************************************************************************
*  ans = SD_Read(fp,buf,nbyte)                                                 *
*    �t�@�C������w�肵���o�C�g�������ǂݍ��ޏ���                              *
*    �t�@�C���̐擪����Ǎ��܂�܂��A�Ǎ��ވʒu��ς���ꍇ��SD_Seek���g�p     *
*    *fp   : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                   *
*    *buf  : �Ǎ��񂾃f�[�^���i�[����z��ϐ����w�肵�܂�                      *
*    nbyte : �Ǎ��ރf�[�^�̃o�C�g�����w�肵�܂�                                *
*            �t�@�C���̍Ō�(EOF)�̏ꍇ�Anbyte�ɖ����Ȃ��Ă������Œ�~���܂�    *
*                                                                              *
*    ans  : ���큁�Ǎ��񂾃o�C�g��  �ُ큁�|�P  �d�n�e���O                     *
*******************************************************************************/
int SD_Read(struct SDFILE_OBJECT *fp,char *buf,int nbyte)
{
     if (!(fp->Oflag & (O_READ | O_RDWR))) return -1 ; // �Ǎ��݃I�[�v���łȂ��G���[
     if (fp->FileSeekP >= fp->FileSize)    return  0 ; // EOF
     return sd_rdwr(buf,nbyte,1,(struct SDFILE_OBJECT *)fp) ;
}
/*******************************************************************************
*  ans = SD_fGets(fp,buf,nbyte)                                                *
*    �t�@�C������P�s�ǂݍ��ޏ���                                              *
*    0x0A(LF)�ɏo��܂œǍ��݂܂��A������ 0x0d(CR)0x0a(LF) ��OK�ł��B        *
*    CR��LF�̓f�[�^�Ƃ���buf�ɓ���܂��Abuf�̊m�ۂ���f�[�^�T�C�Y�ɒ��ӂł��B  *
*    �܂��ALF�ɏo���Ȃ����nbyte�܂œǍ��݂܂��B                             *
*    *fp   : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                   *
*    *buf  : �Ǎ��ރf�[�^���i�[�����z��ϐ����w�肵�܂�                        *
*    nbyte : �Ǎ��ރf�[�^�̃o�C�g�����w�肵�܂�                                *
*                                                                              *
*    ans  : ���큁�Ǎ��񂾃o�C�g��  �ُ큁�|�P  �d�n�e���O                     *
*******************************************************************************/
int SD_fGets(struct SDFILE_OBJECT *fp,char *buf,int nbyte)
{
     if (!(fp->Oflag & (O_READ | O_RDWR))) return -1 ; // �Ǎ��݃I�[�v���łȂ��G���[
     if (fp->FileSeekP >= fp->FileSize)    return  0 ; // EOF
     return sd_rdwr(buf,nbyte,2,(struct SDFILE_OBJECT *)fp) ;
}
/*******************************************************************************
*  ans = SD_Size(fp)                                                           *
*    �t�@�C���̃T�C�Y�𓾂鏈��(����SD�ɏ����܂�Ă���T�C�Y��Ԃ�)            *
*    *fp  : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                    *
*                                                                              *
*    ans  : ���큁�t�@�C���̃T�C�Y  �ُ큁0xffffffff                           *
*******************************************************************************/
unsigned long SD_Size(struct SDFILE_OBJECT *fp)
{
     if (fp->Oflag == 0) return 0xffffffff ;
     return fp->FileSize + fp->AppendSize ;
}
/*******************************************************************************
*  ans = SD_Position(fp)                                                       *
*    �t�@�C���̓Ǐ����|�C���^�l�𓾂鏈��                                      *
*    *fp  : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                    *
*                                                                              *
*    ans  : ���큁���݈ʒu�t������Ă���t�@�C���|�C���^�l  �ُ큁0xffffffff   *
*******************************************************************************/
unsigned long SD_Position(struct SDFILE_OBJECT *fp)
{
     if (fp->Oflag == 0) return 0xffffffff ;
     return fp->FileSeekP ;
}
/*******************************************************************************
*  ans = SD_Seek(fp,offset,sflag)                                              *
*    �t�@�C���̓Ǐ����|�C���^���ړ������鏈��                                  *
*    *fp    : SDFILE_OBJECT�^�\���̂ւ̃|�C���^�[���w�肵�܂�                  *
*    offset : �ʒu�t������ꏊ(�I�t�Z�b�g�l)���w�肵�܂�                       *
*    sflag  : �ʒu�t������I�t�Z�b�g�l�̕����������t���O���w�肵�܂�           *
*             1 = �t�@�C���̐擪����̃I�t�Z�b�g�l�ł���                       *
*             2 = ���݂̏ꏊ����������Ɉʒu�t����I�t�Z�b�g�l�ł���         *
*             3 = ���݂̏ꏊ����O�����Ɉʒu�t����I�t�Z�b�g�l�ł���           *
*             4 = �t�@�C���Ō�̃o�C�g����������Ɉʒu�t�����܂�             *
*                                                                              *
*    ans  : ���큁�ړ���̃t�@�C���|�C���^�l  �ُ큁0xffffffff                 *
*******************************************************************************/
unsigned long SD_Seek(struct SDFILE_OBJECT *fp,unsigned long offset,int sflag)
{
     unsigned long x , ans ;

     ans = 0xffffffff ;
     if (!(fp->Oflag & (O_READ | O_RDWR))) return ans ; // READ/RDWR�I�[�v���ȊO�̓G���[
     if (fp->AppendSize != 0) return ans ;              // �f�[�^�ǉ����̓V�[�N�o���Ȃ�
     switch (sflag) {
        case 1:// �擪����ʒu�t����
               if (offset < fp->FileSize) ans = fp->FileSeekP = offset ;
               break ;
        case 2:// ���݂̏ꏊ����������Ɉʒu�t����
               x = fp->FileSeekP + offset ;
               if (x < fp->FileSize) ans = fp->FileSeekP = x ;
               break ;
        case 3:// ���݂̏ꏊ����O�����Ɉʒu�t����
               x = fp->FileSize - fp->FileSeekP ;
               if (x >= offset) ans = fp->FileSeekP = fp->FileSeekP - offset ;
               break ;
        case 4:// �t�@�C���̍Ō�̃o�C�g����������Ɉʒu�t�����܂�
               if (fp->FileSize != 0) ans = fp->FileSeekP = (fp->FileSize-1) + offset ;
               else                   ans = fp->FileSeekP = 0 ;
               break ;
     }
     return ans ;
}
