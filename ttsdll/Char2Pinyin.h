#ifndef CHAR2PINYIN_H_INCLUDED
#define CHAR2PINYIN_H_INCLUDED

#include <vector>
#include <string>


void Char2Pinyin(char **wordseq,char **pinyinseq,int nword,int *npinyin,STable *wt,STable *ct);

int biandiao(const char *input, char *output, int out_size);
int yibu_biandiao(const char *input_word, const char *input_py, char *output, int out_size);

/*
// nChar 个pinyin和ptag    150个长300
//word_seq: 汶川的这次大地震是一次浅源地震
// pinyin: wen4 chuan1 de1 zhe4 ci4 da4 di4 zhen4  shi4 yi1 ci4 qian3 yuan2 di4 zhen4
// ptag:   0      0     2   0    2   0   0    2      1   0   2    0     1     0    4
*/
int py_bd(const char *word_seq, char **py_sq, short *ptag, int nChar);

/*
// 从输入的汉子字符串(GBK)中获取每个字。
*/
int get_word(const char *word_seq, std::vector<std::string> &vec_word);

/*
input_py: wen4 chuan1 da4 di4 zhen4
*/
int get_py_vec(const char *input_py, std::vector<std::string> &vec_py);

#endif // CHAR2PINYIN_H_INCLUDED
