#include <stdio.h>
#include <stdlib.h>
#include "Table.h"


/*
	// 将wordseq中的 "中华/a"     "人民/b"     "共和国/c"  pos去掉
	最后得到 word 序列 
*/
void trimWordTailTagger(char **wordseq, const int nWords)
{
    int i = 0;
    for(i = 0; i<nWords; i++)
    {
		// 每一个 "人民/b"
        int j=0;
        while(1)
        {
            if(wordseq[i][j]=='/')
            {
                wordseq[i][j] = 0;
                j = 0;
                break;
            }
            j++;
        }
    }
}

/*
	// 从 “中华/a 人民/b 共和国/c” 中 获取 pos序列：a b c 
*/
void getPosTagger(const char *line, int nWords, char **tagseq)
{
    int i=0, j=0;
    const char* tmp = line;

    for(; *tmp!=0; tmp++)
    {
        if( *tmp != '/')
        {
            continue;
        }
		// 找到第一个 / 
        tmp++;//skip '/'
        do{
            tagseq[i][j++] = *tmp;
            tmp++;
        }while(*tmp!=' ');

        tagseq[i][j] = 0;
        i++;
        j=0;
    }
}

/*
	从分词/词性结果的string中: 中华/a 人民/b 共和国/c
	提取出 word序列 和 对应的pos序列 
*/
int GetWordSegmentAndPosTagger(char *tline, char **wordseq, char **posseq)
{
    int nWords;

	// wordseg中存储的为: "中华/a"     "人民/b"     "共和国/c"
    Split(tline, wordseq, &nWords);
    
	// 从 “中华/a 人民/b 共和国/c” 中 获取 pos序列：a b c 
	getPosTagger(tline, nWords, posseq);
    
	// 将wordseq中的 "中华/a"     "人民/b"     "共和国/c"  pos去掉
	trimWordTailTagger(wordseq, nWords);
    
	return nWords;

}
