/*******************************************************************************
*  skSDlib.h - MMC/SDC�̃A�N�Z�X���s���֐��̃w�b�_�t�@�C��                     *
*                                                                              *
* ============================================================================ *
*   VERSION  DATE        BY             CHANGE/COMMENT                         *
* ---------------------------------------------------------------------------- *
*   1.00     2012-04-30  ���ޒ��H�[     Create                                 *
*   2.00     2016-02-29  ���ޒ��H�[     FlashAir��iSDIO�@�\�ꕔ�Ή��ɂ��ύX  *
*   4.00     2017-02-22  ���ޒ��H�[     XC16�ƕ��������I�[�v���ɑΉ�           *
*   4.10     2017-04-01  ���ޒ��H�[     FlashAir��iSDIO�֘A�ŏC��              *
*******************************************************************************/
#ifndef _SKSDLIB_H_
#define _SKSDLIB_H_

// __delay_ms��__delay_us�ׂ̈ɕK�v
#if defined(__PIC24E__)
 #ifndef FCY
  // Unless already defined assume 60MHz(60MIPS) system frequency
  #define FCY 60000000UL      // Fosc/2(60MIPS)
  #include <libpic30.h>
 #endif
#else
 // PIC12F PIC16F PIC18F �p
 #ifndef _XTAL_FREQ
  // Unless already defined assume 32MHz system frequency
  #define _XTAL_FREQ 32000000 // �g�p����PIC�ɂ�蓮����g���l��ݒ肷��
 #endif
#endif


////////////////////////////////////////////////////////////////////////////////
// �r�o�h�֘A

#define CS     LATCbits.LATC2           // �J�[�h�I��M��

////////////////////////////////////////////////////////////////////////////////
// �l�l�b�^�r�c�b�A�N�Z�X�֘A

#define CMD0   0x00                     // �J�[�h�ւ̃��Z�b�g�R�}���h
#define CMD1   0x01                     // MMC�ւ̏������R�}���h
#define CMD8   0x08                     // ����d���̊m�F��SDC�̃o�[�W�����`�F�b�N
#define CMD12  0x0C                     // �f�[�^�Ǎ��݂��~������R�}���h
#define CMD13  0x0D                     // �����݂̏�Ԃ�₢���킹��R�}���h
#define CMD16  0x10                     // �u���b�N�T�C�Y�̏����l�ݒ�R�}���h
#define CMD17  0x11                     // �V���O���u���b�N�Ǎ��ݗv���R�}���h
#define CMD24  0x18                     // �V���O���u���b�N�����ݗv���R�}���h
#define ACMD41 0x29                     // SDC�ւ̏������R�}���h
#define CMD55  0x37                     // ACMD41/ACMD23�ƃZ�b�g�Ŏg�p����R�}���h
#define CMD58  0x3A                     // OCR�̓Ǐo���R�}���h

#define SECTER_BYTES  512               // �P�Z�N�^�̃o�C�g��

////////////////////////////////////////////////////////////////////////////////
// �e�`�s�֘A

#define O_READ    0x01                  // �t�@�C���̓Ǎ��݃I�[�v��
#define O_RDWR    0x02                  // �t�@�C���̓Ǎ��݂Ə����݃I�[�v��
#define O_APPEND  0x04                  // �t�@�C���̒ǉ������݃I�[�v��

// �t�@�C���̃A�N�Z�X���
struct SDFILE_OBJECT {
     unsigned int  Oflag ;              // �A�N�Z�X����I�[�v���t���O��ۑ�
     unsigned int  DirEntryIndex ;      // �f�B���N�g���G���g���̌��������ꏊ�̈ʒu
     unsigned long FileSize ;           // �t�@�C���̃T�C�Y
     unsigned long FileSeekP ;          // �t�@�C���̎��ǂݏo���ʒu
     unsigned long AppendSize ;         // �ǉ������݂����t�@�C���̃T�C�Y
     unsigned long FirstFatno ;         // �f�[�^�i�[���FAT�ԍ�
} ;

// �e�`�s�t�@�C���V�X�e��(FAT12/FAT16)�̃p�����[�^�\����(512�o�C�g)
struct FAT_PARA {
     unsigned char jump[3] ;            // �u�[�g�p�̃W�����v�R�[�h
     unsigned char oemId[8] ;           // �t�H�[�}�b�g���̃{�����[����
     unsigned int  BytesPerSector ;     // �P�Z�N�^������̃o�C�g���A�ʏ��512�o�C�g
     unsigned char SectorsPerCluster ;  // �P�N���X�^������̃Z�N�^��
     unsigned int  ReservedSectorCount ;// �u�[�g�Z�N�^�ȍ~�̗\��̈�̃Z�N�^��
     unsigned char FatCount ;           // FAT�̑g��(�o�b�N�A�b�vFAT��)�A�ʏ�͂Q�g
     unsigned int  RootDirEntryCount ;  // �f�B���N�g���̍쐬�\���A�ʏ��512��
     unsigned int  TotalSectors16 ;     // �S�̈�̃Z�N�^�[����(FAT12/FAT16�p)
     unsigned char MediaType ;          // FAT�̈�̐擪�̒l�A�ʏ��0xF8
     unsigned int  SectorsPerFat16 ;    // �P�g��FAT�̈悪��߂�Z�N�^��(FAT12/FAT16�p)
     unsigned int  SectorsPerTrack ;    // �P�g���b�N������̃Z�N�^��
     unsigned int  HeadCount ;          // �w�b�h��
     unsigned long HidddenSectors ;     // �B�����ꂽ�Z�N�^��
     unsigned long TotalSectors32 ;     // �S�̈�̃Z�N�^�[����(FAT32�p)
     unsigned long SectorsPerFat32 ;    // �P�g��FAT�̈悪��߂�Z�N�^��(FAT32�p)
     unsigned int  FlagsFat32 ;         // FAT�̗L���������̏��t���O
     unsigned int  VersionFat32 ;
     unsigned long RootDirClusterFat32 ;// �f�B���N�g���̃X�^�[�g�N���X�^(FAT32�p)
     unsigned char Dumy[6] ;
     unsigned char FileSystemType[8] ;  // FAT�̎��("FAT12/16")(FAT32��20�o�C�g���ɗL��)
     unsigned char BootCode[448] ;      // �u�[�g�R�[�h�̈�
     unsigned char BootSectorSig0 ;     // 0x55
     unsigned char BootSectorSig1 ;     // 0xaa
#if defined __XC32__ || defined __XC16__
} __attribute__ ((packed)) ;
#else
} ;
#endif

// �f�B���N�g���G���g���[�̍\����(32�o�C�g)
struct DIR_ENTRY {
     unsigned char FileName[11] ;       // �t�@�C����(8)+�g���q(3)
     unsigned char Attributes ;         // �t�@�C���̑���
     unsigned char ReservedNT ;         // Windows NT �p �\��̈�
     unsigned char CreationTimeTenths ; // �t�@�C���쐬���Ԃ�1/10�b�P�ʂ�����킷
     unsigned int  CreationTime ;       // �t�@�C���̍쐬����(hhhhhmmmmmmsssss)
     unsigned int  CreationDate ;       // �t�@�C���̍쐬��(yyyyyyymmmmddddd)
     unsigned int  LastAccessDate ;     // �ŏI�̃A�N�Z�X��
     unsigned int  FirstClusterHigh ;   // �f�[�^�i�[���FAT�ԍ���ʂQ�o�C�g
     unsigned int  LastWriteTime ;      // �ŏI�̃t�@�C�������ݎ���
     unsigned int  LastWriteDate ;      // �ŏI�̃t�@�C�������ݓ�
     unsigned int  FirstClusterLow ;    // �f�[�^�i�[���FAT�ԍ����ʂQ�o�C�g
     unsigned long FileSize ;           // �t�@�C���̃T�C�Y
} ;

////////////////////////////////////////////////////////////////////////////////
// �֐��̃v���g�^�C�v�錾

int SD_Init() ;
int SD_Open(struct SDFILE_OBJECT *fp,const char *filename,int oflag) ;
void SD_Close(struct SDFILE_OBJECT *fp) ;
int SD_Write(struct SDFILE_OBJECT *fp,const char *buf,int nbyte) ;
int SD_Read(struct SDFILE_OBJECT *fp,char *buf,int nbyte) ;
int SD_fGets(struct SDFILE_OBJECT *fp,char *buf,int nbyte) ;
unsigned long SD_Size(struct SDFILE_OBJECT *fp) ;
unsigned long SD_Position(struct SDFILE_OBJECT *fp) ;
unsigned long SD_Seek(struct SDFILE_OBJECT *fp,unsigned long offset,int sflag) ;

// skiSDIOlib�ł��ȉ��̊֐��𗘗p����̂Œǉ�����  Rev2.00/4.10
extern char MMCbuffer[SECTER_BYTES] ;
int ready_check() ;
void cs_select(int flag) ;
int send_command(unsigned char cmd, unsigned long arg) ;


#endif
