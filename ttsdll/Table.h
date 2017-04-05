#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

/*
����Ϊ�ַ���: �л�/a ����/b ���͹�/c
���ݿո��з� �õ�3��ʵ��
*/
int Split(char *str,char **words,int *nWords);

typedef struct{
    char key[30];
    double val;
}TableNode;

/*
	�洢 nNodes �� key value ��� 
*/
typedef struct{
    TableNode *tn;
    int nNodes;
}Table;

/*
��ȡ ���ɴ� ���ɶ��� ������� model
*/
Table * ReadTable(char *fname);

/*
��Table�в��� key������ֵ  û�з���0
*/
double GetTableValue(Table *tab,char * key);


typedef struct{
    char key[300];
    char str[300];
}STableNode;

typedef struct{
    STableNode *tn;
    int nNodes;
}STable;

char * GetSTableValue(STable *tab,char * key);
STable * ReadSTable(char *fname);

/*
ȥ����β�� \n
*/
void dropReturnTag(char *c);

#endif // TABLE_H_INCLUDED
