#include <stdio.h>
#include <stdlib.h>
#include "Table.h"


/*
	// ��wordseq�е� "�л�/a"     "����/b"     "���͹�/c"  posȥ��
	���õ� word ���� 
*/
void trimWordTailTagger(char **wordseq, const int nWords)
{
    int i = 0;
    for(i = 0; i<nWords; i++)
    {
		// ÿһ�� "����/b"
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
	// �� ���л�/a ����/b ���͹�/c�� �� ��ȡ pos���У�a b c 
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
		// �ҵ���һ�� / 
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
	�ӷִ�/���Խ����string��: �л�/a ����/b ���͹�/c
	��ȡ�� word���� �� ��Ӧ��pos���� 
*/
int GetWordSegmentAndPosTagger(char *tline, char **wordseq, char **posseq)
{
    int nWords;

	// wordseg�д洢��Ϊ: "�л�/a"     "����/b"     "���͹�/c"
    Split(tline, wordseq, &nWords);
    
	// �� ���л�/a ����/b ���͹�/c�� �� ��ȡ pos���У�a b c 
	getPosTagger(tline, nWords, posseq);
    
	// ��wordseq�е� "�л�/a"     "����/b"     "���͹�/c"  posȥ��
	trimWordTailTagger(wordseq, nWords);
    
	return nWords;

}
