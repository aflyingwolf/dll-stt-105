#ifndef _TTS_GETCHARINFO_H_
#define _TTS_GETCHARINFO_H_

typedef struct _PinyinInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
}PinyinInfo;

/*
	// label 文件中一行的格式 
	// pinyin:拼音
	// shengmu:声母
	// yunmu:韵母
	// yindiao:音调
	
*/
typedef struct _TtsLabelCharInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
    short yindiao;

    short CharInPwPos; // : 音节在韵律词位置（左数）
    short CharInPpPos; // : 音节在韵律短语位置（左数）
    short CharInIpPos; // : 音节在语调短语位置（左数）
    short CharInSentPos; // : 音节在句子语位置（左数）

    short PwInPpPos; // 韵律词在韵律短语位置
    short PwInIpPos; // 韵律词在语调短语位置
    short PwInSentPos; // 韵律词在句子位置

    short PpInIpPos;  // 韵律短语在语调短语位置
    short PpInSentPos; // 韵律短语在句子语位置
    short IpInSentPos; // 语调短语在句子位置

    short CharInPwNum;
    short CharInPpNum;
    short CharInIpNum;
    short CharInSentNum; // 句子包含音节数

    short PwInPpNum;
    short PwInIpNum;
    short PwInSentNum; // 句子包含韵律词个数

    short PpInIpNum;
    short PpInSentNum;  // 句子包含韵律短语个数
    short IpInSentNum;  // 句子包含语调短语个数

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
