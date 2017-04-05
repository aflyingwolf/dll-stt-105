
#include <stdio.h>
#include <stdlib.h>
#include "Table.h"


/*
// ��wordseq�е� "�л�/a"     "����/b"     "���͹�/c"  posȥ��
���õ� word ����
*/
void trimWordTailTagger(char **wordseq, const int nWords);

/*
// �� ���л�/a ����/b ���͹�/c�� �� ��ȡ pos���У�a b c
*/
void getPosTagger(const char *line, int nWords, char **tagseq);

/*
�ӷִ�/���Խ����string��: �л�/a ����/b ���͹�/c
��ȡ�� word���� �� ��Ӧ��pos����
*/
int GetWordSegmentAndPosTagger(char *tline, char **wordseq, char **posseq);


