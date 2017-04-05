#ifndef _TTS_GETCHARINFO_H_
#define _TTS_GETCHARINFO_H_

typedef struct _PinyinInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
}PinyinInfo;

typedef struct _TtsLabelCharInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
    short yindiao;
    short CharInPwPos;
    short CharInPpPos;
    short CharInIpPos;
    short CharInSentPos;
    short PwInPpPos;
    short PwInIpPos;
    short PwInSentPos;
    short PpInIpPos;
    short PpInSentPos;
    short IpInSentPos;
    short CharInPwNum;
    short CharInPpNum;
    short CharInIpNum;
    short CharInSentNum;
    short PwInPpNum;
    short PwInIpNum;
    short PwInSentNum;
    short PpInIpNum;
    short PpInSentNum;
    short IpInSentNum;
}TtsLabelCharInfo;

/*
二分查找数组中的 pinyin 对应的索引
*/
short getPinyinInfoID(char *pinyin);

/*
获取这个pinyin对应的：声母 韵母 音调
*/
void getShengYun(char *pinyin,char *shengmu,char *yunmu,short *yindiao);

/*
char **pinyinSeq: sNum 个拼音
short *tag :	   sNum 个pos标记(01234)
最后得到label序列需要的信息 存储在 cif中
*/
void TtsLabel_ObtainLabelCharSeq(TtsLabelCharInfo * cif,char **pinyinSeq,short sNum,short * tag);
void TTS_Label_Init();
void PrintLabel(TtsLabelCharInfo * cif,short sNum,char *fname);

#endif
