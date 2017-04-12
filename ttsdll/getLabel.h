#ifndef _TTS_GETCHARINFO_H_
#define _TTS_GETCHARINFO_H_

typedef struct _PinyinInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
}PinyinInfo;

/*
	// label �ļ���һ�еĸ�ʽ 
	// pinyin:ƴ��
	// shengmu:��ĸ
	// yunmu:��ĸ
	// yindiao:����
	
*/
typedef struct _TtsLabelCharInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
    short yindiao;

    short CharInPwPos; // : ���������ɴ�λ�ã�������
    short CharInPpPos; // : ���������ɶ���λ�ã�������
    short CharInIpPos; // : �������������λ�ã�������
    short CharInSentPos; // : �����ھ�����λ�ã�������

    short PwInPpPos; // ���ɴ������ɶ���λ��
    short PwInIpPos; // ���ɴ����������λ��
    short PwInSentPos; // ���ɴ��ھ���λ��

    short PpInIpPos;  // ���ɶ������������λ��
    short PpInSentPos; // ���ɶ����ھ�����λ��
    short IpInSentPos; // ��������ھ���λ��

    short CharInPwNum;
    short CharInPpNum;
    short CharInIpNum;
    short CharInSentNum; // ���Ӱ���������

    short PwInPpNum;
    short PwInIpNum;
    short PwInSentNum; // ���Ӱ������ɴʸ���

    short PpInIpNum;
    short PpInSentNum;  // ���Ӱ������ɶ������
    short IpInSentNum;  // ���Ӱ�������������

}TtsLabelCharInfo;

/*
���ֲ��������е� pinyin ��Ӧ������
*/
short getPinyinInfoID(char *pinyin);

/*
��ȡ���pinyin��Ӧ�ģ���ĸ ��ĸ ����
*/
void getShengYun(char *pinyin,char *shengmu,char *yunmu,short *yindiao);

/*
char **pinyinSeq: sNum ��ƴ��
short *tag :	   sNum ��pos���(01234)
���õ�label������Ҫ����Ϣ �洢�� cif��
*/
void TtsLabel_ObtainLabelCharSeq(TtsLabelCharInfo * cif,char **pinyinSeq,short sNum,short * tag);
void TTS_Label_Init();
void PrintLabel(TtsLabelCharInfo * cif,short sNum,char *fname);

#endif
