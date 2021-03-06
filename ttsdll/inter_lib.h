
#ifndef  __INTER_LIB_H_
#define  __INTER_LIB_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "HTS_engine.h"

#ifdef  MYLIBDLL
#define MYLIBDLL extern "C" _declspec(dllimport)
#else
#define MYLIBDLL extern "C" _declspec(dllexport)
#endif

const int MAX_WAV_SIZE = 100000000;
const int MAX_PATH_SIZE = 1000;
const int LEN_SIL = 300;


class __declspec(dllexport) TTS
{
public:
    TTS();
    ~TTS();

	/*
	传入的是： model/like-77等 声学模型所在的目录 里面是直接是声学模型
	需要的resource pinyin-dict等 在data目录下  写死了
	*/
    int init(const char *model_dir);
    int line2short_array(const char *line, short *out, int out_size);

	// 其他变量 
	int sampling_rate ;

    int lines2short_array(const char *lines, short *out, int out_size);

	/* Error: output error message */
	void Error(const int error, char *message, ...);

	/* Getfp: wrapper for fopen */
	FILE *Getfp(const char *name, const char *opt);

	/* GetNumInterp: get number of speakers for interpolation from argv */
	int GetNumInterp(int argc, char **argv_search);

private:
    HTS_Engine *engine;
    STable *wt, *ct;
    Table *pw, *pp, *ip;

    FILE *fp_log ;

};





#endif  // __INTER_LIB_H_
