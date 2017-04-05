#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

/*
输入为字符串: 中华/a 人民/b 共和国/c
根据空格切分 得到3个实体
*/
int Split(char *str,char **words,int *nWords);

typedef struct{
    char key[30];
    double val;
}TableNode;

/*
	存储 nNodes 个 key value 结点 
*/
typedef struct{
    TableNode *tn;
    int nNodes;
}Table;

/*
读取 韵律词 韵律短语 语调短语 model
*/
Table * ReadTable(char *fname);

/*
从Table中查找 key特征的值  没有返回0
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
去掉行尾的 \n
*/
void dropReturnTag(char *c);

#endif // TABLE_H_INCLUDED
