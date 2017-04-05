
#include <stdio.h>
#include <stdlib.h>
#include "Table.h"


/*
// 将wordseq中的 "中华/a"     "人民/b"     "共和国/c"  pos去掉
最后得到 word 序列
*/
void trimWordTailTagger(char **wordseq, const int nWords);

/*
// 从 “中华/a 人民/b 共和国/c” 中 获取 pos序列：a b c
*/
void getPosTagger(const char *line, int nWords, char **tagseq);

/*
从分词/词性结果的string中: 中华/a 人民/b 共和国/c
提取出 word序列 和 对应的pos序列
*/
int GetWordSegmentAndPosTagger(char *tline, char **wordseq, char **posseq);


