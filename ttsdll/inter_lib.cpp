#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Vocoder.h"
#include "Table.h"
#include "Char2Pinyin.h"
#include "ProsodicAnalysis.h"
#include "getLabel.h"
#include "str_fun.h"
#include "inter_lib.h"



TTS::TTS()
{
	if (this->engine != NULL)
	{
		delete this->engine;
		this->engine = NULL;
	}

	this->engine = (HTS_Engine *)malloc(sizeof(HTS_Engine));

}

TTS::~TTS()
{
	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}
	if (engine != NULL)
	{
		/* free memory */
		HTS_Engine_clear(engine);
		free(engine);
		engine = NULL;
	}

	if (wt != NULL)
	{

		delete wt;
		wt = NULL;
	}
	
	if (ct != NULL)
	{
		delete ct;
		ct = NULL;
	}

	if (pw != NULL)
	{
		delete pw;
		pw = NULL;
	}
    
	if (pp != NULL)
	{
		delete pp;
		pp = NULL;
	}
    
	if (ip != NULL)
	{

		delete ip;
		ip = NULL;
	}



}

/*
	������ǣ� model/like-77�� ��ѧģ�����ڵ�Ŀ¼ ������ֱ������ѧģ�� 
	��Ҫ��resource pinyin-dict�� ��dataĿ¼��  д���� 
*/
int TTS::init(const char *model_dir)
{
	fp_log = fopen("yl.log", "w");
	int num_argc = 10;
	int i;

	/* number of speakers for interpolation */
	int num_interp = 0;
	double *rate_interp = NULL;

	/* file names of models */
	char **fn_ms_dur;
	char **fn_ms_mgc; // -mm mgc.pdf
	char **fn_ms_lf0; //  -mf lf0.pdf
	char **fn_ms_lpf; // -ml lpf.pdf
	/* number of each models for interpolation */
	int num_ms_dur = 0, num_ms_mgc = 0, num_ms_lf0 = 0, num_ms_lpf = 0; // * * * 1

	/* file names of trees */
	char **fn_ts_dur;
	char **fn_ts_mgc; // -tm tree-mgc.inf
	char **fn_ts_lf0; // -tf tree-lf0.inf 
	char **fn_ts_lpf; // -tl tree-lpf.inf
	/* number of each trees for interpolation */
	int num_ts_dur = 0, num_ts_mgc = 0, num_ts_lf0 = 0, num_ts_lpf = 0; // * * * 1

	/* file names of windows */
	char **fn_ws_mgc; // -dm mgc.win1 -dm mgc.win2 -dm mgc.win3
	char **fn_ws_lf0; // -df lf0.win1 -df lf0.win2 -df lf0.win3
	char **fn_ws_lpf; // -dl lpf.win1
	int num_ws_mgc = 0, num_ws_lf0 = 0, num_ws_lpf = 0; // 3 3 1 

	/* file names of global variance */
	char **fn_ms_gvm = NULL; // -cm gv-mgc.pdf
	char **fn_ms_gvl = NULL;
	char **fn_ms_gvf = NULL;
	int num_ms_gvm = 0, num_ms_gvl = 0, num_ms_gvf = 0; // 1 

	/* file names of global variance trees */
	char **fn_ts_gvm = NULL; // -em tree-gv-mgc.inf 
	char **fn_ts_gvl = NULL;
	char **fn_ts_gvf = NULL;
	int num_ts_gvm = 0, num_ts_gvl = 0, num_ts_gvf = 0; // 1

	/* file name of global variance switch */
	char *fn_gv_switch = NULL;

	// ֡�� 16k��Ӧ80   1/200=5����   
	// ori-105=220   �޸ĳ�80֮�� �����໿졣����
	int fperiod = 200; // -p  44.1k
	//int fperiod = 80; // -p  16k   

	// ori-105=0.55  �ĳ�0.2|0.42֮���������� ��С��  0.6|0.8����ţ
	double alpha = 0.55;  // -a hts�ڲ�����Ĭ��: 16000 80֡�� 0.42 
	
	// ori-105=0  
	// Gamma=-1/stage: if stage=0 then Gamma=0 
	int stage = 0;   

	// -b  ori-105=0.0   hts�ڲ����� -0.8~0.8֮�� 
	// -b  0.2|0.4��ʱ�� �������ź� 
	double beta = 0.0;

	// ���ó�ʼbuffsize �Խ����Ӱ��  ori-105=1600 
	int audio_buff_size = 1600; 
	  
	// ����msd���� ori-105=0.5 
	// ��Ϊ 0.9 ����ģ������ ; 0.3 0.1 ʱ û���Ա仯 
	double uv_threshold = 0.5; 

	// ori-105 ȫ�� 1.0 
	// �޸ĳ� 10.0|2.0 ���������������ǣ��̾��ӵĿ�ͷ���ֶ�ʧ
	// �޸ĳ� 0.3 ���������ƮƮȻ  ��Զ���������� 
	// 1.8 ok 
	double gv_weight_mgc = 1.0;
	double gv_weight_lf0 = 1.0;
	double gv_weight_lpf = 1.0;

	// ��ֵ ���� ֱ�Ӹ�ֵ
	this->sampling_rate = 44100; // -s

	// �Ƿ�ʹ�ö�������  ori-105=TRUE   for LSP������  hts-105ѵ��ʱ��δʹ��LPC2LSP  
	HTS_Boolean use_log_gain = FALSE;


	/* delta window handler for mel-cepstrum */
	fn_ws_mgc = (char **)calloc(num_argc, sizeof(char *));
	/* delta window handler for log f0 */
	fn_ws_lf0 = (char **)calloc(num_argc, sizeof(char *));
	/* delta window handler for low-pass filter */
	fn_ws_lpf = (char **)calloc(num_argc, sizeof(char *));

	/* prepare for interpolation */
	num_interp = 1;
	rate_interp = (double *)calloc(num_interp, sizeof(double));
	for (i = 0; i < num_interp; i++)
	{
		// ori-105=1.0
		rate_interp[i] = 1.0;
	}
		

	fn_ms_dur = (char **)calloc(num_interp, sizeof(char *));
	fn_ms_mgc = (char **)calloc(num_interp, sizeof(char *));
	fn_ms_lf0 = (char **)calloc(num_interp, sizeof(char *));
	fn_ms_lpf = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_dur = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_mgc = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_lf0 = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_lpf = (char **)calloc(num_interp, sizeof(char *));
	fn_ms_gvm = (char **)calloc(num_interp, sizeof(char *));
	fn_ms_gvl = (char **)calloc(num_interp, sizeof(char *));
	fn_ms_gvf = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_gvm = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_gvl = (char **)calloc(num_interp, sizeof(char *));
	fn_ts_gvf = (char **)calloc(num_interp, sizeof(char *));

	// ģ�ͱ�����ֵ 
	char *fn_ts_dur_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ts_dur_tmp, MAX_PATH_SIZE - 1, "%s\\tree-dur.inf", model_dir);
	fn_ts_dur[num_ts_dur++] = fn_ts_dur_tmp; // -td

	char *fn_ts_lf0_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ts_lf0_tmp, MAX_PATH_SIZE - 1, "%s\\tree-lf0.inf", model_dir);
	fn_ts_lf0[num_ts_lf0++] = fn_ts_lf0_tmp; // -tf

	char *fn_ts_mgc_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ts_mgc_tmp, MAX_PATH_SIZE - 1, "%s\\tree-mgc.inf", model_dir);
	fn_ts_mgc[num_ts_mgc++] = fn_ts_mgc_tmp; // -tm

	char *fn_ts_lpf_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ts_lpf_tmp, MAX_PATH_SIZE - 1, "%s\\tree-lpf.inf", model_dir);
	fn_ts_lpf[num_ts_lpf++] = fn_ts_lpf_tmp; // -tl

	char *fn_ms_dur_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ms_dur_tmp, MAX_PATH_SIZE - 1, "%s\\dur.pdf", model_dir);
	fn_ms_dur[num_ms_dur++] = fn_ms_dur_tmp; // -md

	char *fn_ms_lf0_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ms_lf0_tmp, MAX_PATH_SIZE - 1, "%s\\lf0.pdf", model_dir);
	fn_ms_lf0[num_ms_lf0++] = fn_ms_lf0_tmp; // -mf

	char *fn_ms_mgc_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ms_mgc_tmp, MAX_PATH_SIZE - 1, "%s\\mgc.pdf", model_dir);
	fn_ms_mgc[num_ms_mgc++] = fn_ms_mgc_tmp; // -mm

	char *fn_ms_lpf_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ms_lpf_tmp, MAX_PATH_SIZE - 1, "%s\\lpf.pdf", model_dir);
	fn_ms_lpf[num_ms_lpf++] = fn_ms_lpf_tmp; // -ml

	// -dm
	char *fn_ws_mgc_tmp_1 = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_mgc_tmp_1, MAX_PATH_SIZE - 1, "%s\\mgc.win1", model_dir);
	fn_ws_mgc[num_ws_mgc++] = fn_ws_mgc_tmp_1;

	char *fn_ws_mgc_tmp_2 = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_mgc_tmp_2, MAX_PATH_SIZE - 1, "%s\\mgc.win2", model_dir);
	fn_ws_mgc[num_ws_mgc++] = fn_ws_mgc_tmp_2;

	char *fn_ws_mgc_tmp_3 = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_mgc_tmp_3, MAX_PATH_SIZE - 1, "%s\\mgc.win3", model_dir);
	fn_ws_mgc[num_ws_mgc++] = fn_ws_mgc_tmp_3;
	// -df
	char *fn_ws_lf0_tmp_1 = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_lf0_tmp_1, MAX_PATH_SIZE - 1, "%s\\lf0.win1", model_dir);
	fn_ws_lf0[num_ws_lf0++] = fn_ws_lf0_tmp_1;

	char *fn_ws_lf0_tmp_2 = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_lf0_tmp_2, MAX_PATH_SIZE - 1, "%s\\lf0.win2", model_dir);
	fn_ws_lf0[num_ws_lf0++] = fn_ws_lf0_tmp_2;

	char *fn_ws_lf0_tmp_3 = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_lf0_tmp_3, MAX_PATH_SIZE - 1, "%s\\lf0.win3", model_dir);
	fn_ws_lf0[num_ws_lf0++] = fn_ws_lf0_tmp_3;

	char *fn_ws_lpf_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ws_lpf_tmp, MAX_PATH_SIZE - 1, "%s\\lpf.win1", model_dir);
	fn_ws_lpf[num_ws_lpf++] = fn_ws_lpf_tmp; // -dl

	char *fn_ms_gvm_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ms_gvm_tmp, MAX_PATH_SIZE - 1, "%s\\gv-mgc.pdf", model_dir);
	fn_ms_gvm[num_ms_gvm++] = fn_ms_gvm_tmp; // -cm

	char *fn_ms_gvl_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ms_gvl_tmp, MAX_PATH_SIZE - 1, "%s\\gv-lf0.pdf", model_dir);
	fn_ms_gvl[num_ms_gvl++] = fn_ms_gvl_tmp; // -cf 

	char *fn_gv_switch_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_gv_switch_tmp, MAX_PATH_SIZE - 1, "%s\\gv-switch.inf", model_dir);
	fn_gv_switch = fn_gv_switch_tmp; // -k

	char *fn_ts_gvm_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ts_gvm_tmp, MAX_PATH_SIZE - 1, "%s\\tree-gv-mgc.inf", model_dir);
	fn_ts_gvm[num_ts_gvm++] = fn_ts_gvm_tmp; // -em

	char *fn_ts_gvl_tmp = new char[MAX_PATH_SIZE];
	_snprintf(fn_ts_gvl_tmp, MAX_PATH_SIZE - 1, "%s\\tree-gv-lf0.inf", model_dir);
	fn_ts_gvl[num_ts_gvl++] = fn_ts_gvl_tmp; // -ef 

	/* number of models,trees check */
	printf("interp=%d\tts_dur=%d\tts_mgc=%d\tts_lf0=%d\tms_dur=%d\tms_mgc=%d\tms_lf0=%d",
		num_interp, num_ts_dur, num_ts_mgc, num_ts_lf0, num_ms_dur, num_ms_mgc, num_ms_lf0);
	
	if (num_interp != num_ts_dur || num_interp != num_ts_mgc ||
	num_interp != num_ts_lf0 || num_interp != num_ms_dur ||
	num_interp != num_ms_mgc || num_interp != num_ms_lf0) {
	Error(1, "hts_engine: specify %d models(trees) for each parameter.\n",
	num_interp);
	}
	
	if (num_ms_lpf > 0 || num_ts_lpf > 0) {
		if (num_interp != num_ms_lpf || num_interp != num_ts_lpf) {
			Error(1, "hts_engine: specify %d models(trees) for each parameter.\n",
				num_interp);
		}
	}

	// initialize (stream[0] = spectrum, stream[1] = lf0, stream[2] = low-pass filter) 
	// �������ʼ����һ�� engine ��Ҫ�Ĳ��� :
	if (num_ms_lpf > 0 || num_ts_lpf > 0) {
		HTS_Engine_initialize(engine, 3);
	}
	else {
		HTS_Engine_initialize(engine, 2);
	}

	// load duration model  durģ������  
	printf("fn_ts_dur=%s\t", fn_ts_dur[0]);
	// engine | dur.pdf | tree-dur.inf | num_interp=1 
	// �ڲ����� HTS_Model_load_tree �Ӻ��� ���� tree-dur.inf 
	HTS_Engine_load_duration_from_fn(engine, fn_ms_dur, fn_ts_dur, num_interp);

	/* load stream[0] (spectrum model) */
	HTS_Engine_load_parameter_from_fn(engine, fn_ms_mgc, fn_ts_mgc, fn_ws_mgc,
		0, FALSE, num_ws_mgc, num_interp);
	/* load stream[1] (lf0 model) */
	HTS_Engine_load_parameter_from_fn(engine, fn_ms_lf0, fn_ts_lf0, fn_ws_lf0,
		1, TRUE, num_ws_lf0, num_interp);
	/* load stream[2] (low-pass filter model) */
	if (num_ms_lpf > 0 || num_ts_lpf > 0)
		HTS_Engine_load_parameter_from_fn(engine, fn_ms_lpf, fn_ts_lpf,
		fn_ws_lpf, 2, FALSE, num_ws_lpf,num_interp);

	/* load gv[0] (GV for spectrum) */
	if (num_interp == num_ms_gvm) {
		if (num_ms_gvm == num_ts_gvm)
			HTS_Engine_load_gv_from_fn(engine, fn_ms_gvm, fn_ts_gvm, 0,num_interp);
		else
			HTS_Engine_load_gv_from_fn(engine, fn_ms_gvm, NULL, 0, num_interp);
	}
	/* load gv[1] (GV for lf0) */
	if (num_interp == num_ms_gvl) 
	{
		if (num_ms_gvl == num_ts_gvl)
			HTS_Engine_load_gv_from_fn(engine, fn_ms_gvl, fn_ts_gvl, 1,num_interp);
		else
			HTS_Engine_load_gv_from_fn(engine, fn_ms_gvl, NULL, 1, num_interp);
	}
	/* load gv[2] (GV for low-pass filter) */
	if (num_interp == num_ms_gvf && (num_ms_lpf > 0 || num_ts_lpf > 0)) 
	{
		if (num_ms_gvf == num_ts_gvf)
			HTS_Engine_load_gv_from_fn(engine, fn_ms_gvf, fn_ts_gvf, 0,num_interp);
		else
			HTS_Engine_load_gv_from_fn(engine, fn_ms_gvf, NULL, 2, num_interp);
	}
	/* load GV switch */
	if (fn_gv_switch != NULL)
		HTS_Engine_load_gv_switch_from_fn(engine, fn_gv_switch);

	/* set parameter */
	HTS_Engine_set_sampling_rate(engine, this->sampling_rate);
	HTS_Engine_set_fperiod(engine, fperiod);  
	HTS_Engine_set_alpha(engine, alpha);
	HTS_Engine_set_gamma(engine, stage);
	HTS_Engine_set_log_gain(engine, use_log_gain);  
	HTS_Engine_set_beta(engine, beta);
	HTS_Engine_set_audio_buff_size(engine, audio_buff_size);

	/* set voiced/unvoiced threshold for stream[1] */
	// ����msd����  0-1֮��  
	HTS_Engine_set_msd_threshold(engine, 1, uv_threshold);      

	HTS_Engine_set_gv_weight(engine, 0, gv_weight_mgc);
	HTS_Engine_set_gv_weight(engine, 1, gv_weight_lf0);
	if (num_ms_lpf > 0 || num_ts_lpf > 0)
		HTS_Engine_set_gv_weight(engine, 2, gv_weight_lpf);

	for (i = 0; i < num_interp; i++) 
	{
		HTS_Engine_set_duration_interpolation_weight(engine, i, rate_interp[i]);
		HTS_Engine_set_parameter_interpolation_weight(engine, 0, i,
			rate_interp[i]);
		HTS_Engine_set_parameter_interpolation_weight(engine, 1, i,
			rate_interp[i]);
		if (num_ms_lpf > 0 || num_ts_lpf > 0)
			HTS_Engine_set_parameter_interpolation_weight(engine, 2, i,
			rate_interp[i]);
	}

	if (num_interp == num_ms_gvm)
	{
		for (i = 0; i < num_interp; i++)
			HTS_Engine_set_gv_interpolation_weight(engine, 0, i, rate_interp[i]);
	}

	
	if (num_interp == num_ms_gvl)
	{
		for (i = 0; i < num_interp; i++)
			HTS_Engine_set_gv_interpolation_weight(engine, 1, i, rate_interp[i]);
	}
	
	
	if (num_interp == num_ms_gvf && (num_ms_lpf > 0 || num_ts_lpf > 0))
	{
		for (i = 0; i < num_interp; i++)
			HTS_Engine_set_gv_interpolation_weight(engine, 2, i, rate_interp[i]);
	}
	

	
	/////////////////////////////////////// label.txt /////////////////////////////////////////////
	// ����ģ���ʼ�� TN �ִ� ���Ա�ע ���ɱ�ע
	//���ɴʡ����ɶ���������ģ��
	char *wm = new char[MAX_PATH_SIZE];
	_snprintf(wm, MAX_PATH_SIZE - 1, "data/resource/ProsodicWordModel.txt");
	pw = ReadTable(wm);   //���ɴ�

	char *ppm = new char[MAX_PATH_SIZE];
	_snprintf(ppm, MAX_PATH_SIZE - 1, "data/resource/ProsodicPhraseModel.txt");
	pp = ReadTable(ppm);  //���ɶ���

	char *ipm = new char[MAX_PATH_SIZE];
	_snprintf(ipm, MAX_PATH_SIZE - 1, "data/resource/IntPhraseModel.txt");
	ip = ReadTable(ipm);       //�������

	char *dict = new char[MAX_PATH_SIZE];
	_snprintf(dict, MAX_PATH_SIZE - 1, "data/resource/dict.txt");
	wt = ReadSTable(dict);   //�ֵ�

	char *c2p = new char[MAX_PATH_SIZE];
	_snprintf(c2p, MAX_PATH_SIZE - 1, "data/resource/char2pinyin.txt");
	ct = ReadSTable(c2p);  //�������ձ�

	TTS_Label_Init(); //��ʼ��Labelģ��


	///////////////////////////////////////  delete  ////////////////////////////////////////////////
	free(rate_interp);
	free(fn_ws_mgc);
	free(fn_ws_lf0);
	free(fn_ws_lpf);
	free(fn_ms_mgc);
	free(fn_ms_lf0);
	free(fn_ms_lpf);
	free(fn_ms_dur);
	free(fn_ts_mgc);
	free(fn_ts_lf0);
	free(fn_ts_lpf);
	free(fn_ts_dur);
	free(fn_ms_gvm);
	free(fn_ms_gvl);
	free(fn_ms_gvf);
	free(fn_ts_gvm);
	free(fn_ts_gvl);
	free(fn_ts_gvf);

	if (fn_ts_dur_tmp){ delete fn_ts_dur_tmp; fn_ts_dur_tmp = NULL; }
	if (fn_ts_lf0_tmp){ delete fn_ts_lf0_tmp; fn_ts_lf0_tmp = NULL; }
	if (fn_ts_mgc_tmp){ delete fn_ts_mgc_tmp; fn_ts_mgc_tmp = NULL; }
	if (fn_ts_lpf_tmp){ delete fn_ts_lpf_tmp; fn_ts_lpf_tmp = NULL; }
	if (fn_ms_dur_tmp){ delete fn_ms_dur_tmp; fn_ms_dur_tmp = NULL; }
	if (fn_ms_lf0_tmp){ delete fn_ms_lf0_tmp; fn_ms_lf0_tmp = NULL; }
	if (fn_ms_mgc_tmp){ delete fn_ms_mgc_tmp; fn_ms_mgc_tmp = NULL; }
	if (fn_ms_lpf_tmp){ delete fn_ms_lpf_tmp; fn_ms_lpf_tmp = NULL; }
	if (fn_ws_mgc_tmp_1){ delete fn_ws_mgc_tmp_1; fn_ws_mgc_tmp_1 = NULL; }
	if (fn_ws_mgc_tmp_2){ delete fn_ws_mgc_tmp_2; fn_ws_mgc_tmp_2 = NULL; }
	if (fn_ws_mgc_tmp_3){ delete fn_ws_mgc_tmp_3; fn_ws_mgc_tmp_3 = NULL; }
	if (fn_ws_lf0_tmp_1){ delete fn_ws_lf0_tmp_1; fn_ws_lf0_tmp_1 = NULL; }
	if (fn_ws_lf0_tmp_2){ delete fn_ws_lf0_tmp_2; fn_ws_lf0_tmp_2 = NULL; }
	if (fn_ws_lf0_tmp_3){ delete fn_ws_lf0_tmp_3; fn_ws_lf0_tmp_3 = NULL; }
	if (fn_ws_lpf_tmp){ delete fn_ws_lpf_tmp; fn_ws_lpf_tmp = NULL; }
	if (fn_ms_gvl_tmp){ delete fn_ms_gvl_tmp; fn_ms_gvl_tmp = NULL; }
	if (fn_ts_gvl_tmp){ delete fn_ts_gvl_tmp; fn_ts_gvl_tmp = NULL; }

	// ���� �ʵ� ���� 
	if (wm){ delete wm; wm = NULL; }
	if (ppm){ delete ppm; ppm = NULL; }
	if (ipm){ delete ipm; ipm = NULL; }
	if (c2p){ delete c2p; c2p = NULL; }
	if (dict){ delete dict; dict = NULL; }
	

    return 0;
}

int TTS::line2short_array(const char *line, short *out, int out_size)
{
	double f;
	int ret = 0;
	const int MAX_LINE_SIZE = 10000;
	int len, i, msd_frame, nWord, nChar;
	short temp;

	int NUM_WORD = 200; // ȥ��ԭ�� ptag �е�60
	int NUM_SEQ = 500; // ����ԭ���� 150 
	int NUM_LEN = 1000; // ����ԭ���� 300

	// ����lab�ļ���		���raw		trace
	char *labfn = NULL, rawfn = NULL, tracefn = NULL;
	FILE *wavfp = NULL, *rawfp = NULL, *tracefp = NULL;	
	FILE *durfp = NULL, *mgcfp = NULL, *lf0fp = NULL, *lpffp = NULL;

	// ����/��� �ļ� 
	labfn = "./label.txt";
	//rawfp = Getfp("test.raw", "wb");
	tracefp = Getfp("log\\test.trace", "wt");
	durfp = Getfp("log\\test.dur", "wt");
	mgcfp = Getfp("log\\test.mgc", "wt");
	lf0fp = Getfp("log\\test.lf0", "wt");
	lpffp = Getfp("log\\test.lpf", "wt");

    if(line == NULL || out == NULL || out_size < 1)
    {
        printf("line==NULL || out == NULL || out_size < 1\n");
        return -1;
    }
	
    char tline[MAX_LINE_SIZE]={0};
    _snprintf(tline, MAX_LINE_SIZE, "%s", line);
    dropReturnTag(tline);
	fprintf(fp_log, "tline=%s\n",tline);
	fflush(fp_log);

	if (strlen(tline) == 0)
	{
		return 0;
	}


 
	TtsLabelCharInfo *cif = (TtsLabelCharInfo *)malloc(sizeof(TtsLabelCharInfo)*NUM_LEN);

    int *pwr,*ppr;
    short *ptag;
    char *teof;
    pwr =   (int *)malloc(sizeof(int)* NUM_WORD);
    ppr =   (int *)malloc(sizeof(int)* NUM_WORD);
    ptag=   (short *)malloc(sizeof(short)* NUM_WORD);

    char **wordseq, **posseq, **pinyinseq;
	wordseq = (char**)malloc(sizeof(char *)* NUM_SEQ);
	posseq = (char**)malloc(sizeof(char *)*NUM_SEQ);
	pinyinseq = (char**)malloc(sizeof(char *)*NUM_SEQ);
	for (i = 0; i<NUM_SEQ; i++)
    {
		wordseq[i] = (char *)malloc(sizeof(char)* NUM_LEN);  //�������У�ÿһ��wordseq[i]�洢һ������
		posseq[i] = (char *)malloc(sizeof(char)*NUM_LEN);   //��������
		pinyinseq[i] = (char *)malloc(sizeof(char)*NUM_LEN); //����ת����õ���ƴ������
    }


    TTS_Label_Init(); //��ʼ��Labelģ��
	fprintf(fp_log, "TTS_Label_Init ok!\n");
	fflush(fp_log);

	//�ӷִ�/���Խ����string��: �л�/a ����/b ���͹�/c
	//��ȡ�� word���� �� ��Ӧ��pos����	
    nWord = GetWordSegmentAndPosTagger(tline,wordseq,posseq);
	fprintf(fp_log, "GetWordSegmentAndPosTagger ok!\n");
	fprintf(fp_log, "nWord=%d\n", nWord);

	for (i = 0; i<nWord; i++)
	{
		fprintf(fp_log, "%s/%s ", wordseq[i], posseq[i]);
	}
	fflush(fp_log);

	//���ɴʷ���    �õ�pwr
    ProsodicWordAnalysis(wordseq, posseq, nWord, pwr, pw);  
    
	//һ�����������ɶ���   �õ�ppr
	ProsodicPhraseAnalysis(wordseq, posseq, nWord, pwr, ppr, pp, ip); 

	// ͨ�� pwr��ppr �õ�ÿ��pinyin��Ӧ��ptagֵ
	//ͳһת����ʽ������ptag��ptag����0, 1��2��3,4
	// 0 ��ʾ��ǰchar �� ����char ��ͬ������ɴ� �������ɴ�
	// 1 ��ʾ ���ɴʻ��ִ� ;  2 ��ʾ ���ɶ��� ��  3 ��ʾ ������  4 Ĭ�Ͼ�β
    GetProsodicTag(wordseq, nWord, pwr, ppr, ptag);     
    
	fprintf(fp_log, "GetProsodicTag ok!\n");
	fflush(fp_log);

    //����ת��  nChar ���ֵĸ���  nWord�Ǵʸ��� ƴ���Ǹ��ֶ�Ӧ�� 
    Char2Pinyin(wordseq, pinyinseq, nWord, &nChar, wt, ct);
	fprintf(fp_log, "Char2Pinyin ok!\n");
	fprintf(fp_log, "\nnChar=%d\n", nChar);
	for (i = 0; i<nChar; i++)
	{
		fprintf(fp_log, "pinyinseq[%d]=%s\t", i, pinyinseq[i]);
		fprintf(fp_log, "ptag[%d]=%d\n", i, ptag[i]);

	}
	fflush(fp_log);

    //��������ת����Ϣ��������Ϣ������ÿ���ֵ�������Ϣ����д��cif
	/*
	fprintf(fp_log, "\nwt_Nodes=%d\n", wt->nNodes);
	for (i = 0; i<wt->nNodes; i++)
	{
		fprintf(fp_log, "%s/\t%s\n", wt->tn[i].key, wt->tn[i].str);
	}
	fprintf(fp_log, "\nct_Nodes=%d\n", ct->nNodes);
	for (i = 0; i<ct->nNodes; i++)
	{
		fprintf(fp_log, "%s/\t%s\n", ct->tn[i].key, ct->tn[i].str);
	}
	*/

	//// �������  һ����� 
	//// bug1: �Ƴ�ȥ����   "�� ��ȥ  �� ��" �е� "����"
	//char word_seq_temp[1000] = { 0 };
	//for (i = 0; i < nWord; i++)
	//{
	//	strcat(word_seq_temp, wordseq[i]);
	//}
	//
	//ret = py_bd(word_seq_temp, pinyinseq, ptag, nChar);
	//if (ret < 0)
	//{	
	//	fprintf(fp_log, "py_bd error!!!!!!! ret %d\n",ret);
	//	fflush(fp_log);
	//}
	//
	//fprintf(fp_log, "py_bd is over !\n");
	//fprintf(fp_log, "\nnChar=%d\n", nChar);
	//for (i = 0; i<nChar; i++)
	//{
	//	fprintf(fp_log, "pinyinseq[%d]=%s\t", i, pinyinseq[i]);
	//	fprintf(fp_log, "ptag[%d]=%d\n", i, ptag[i]);	
	//	
	//}
	//fflush(fp_log);

	// ʹ��ÿ���ֵ�pinyin + ptag(���ɱ��)   ��ȡ label���е���Ϣ 
    TtsLabel_ObtainLabelCharSeq(cif, pinyinseq, nChar, ptag);
	fprintf(fp_log, "TtsLabel_ObtainLabelCharSeq ok!\n");
	fflush(fp_log);

    //��ӡ����׼HTS��ʽlabel�ļ�����д��label.txt  Windows CR LF ��β ? 
    PrintLabel(cif, nChar, "./label.txt");
	fprintf(fp_log, "PrintLabel ok!\n");
	fflush(fp_log);

    //�������������Ϣ����������Ա����
    for(i=0;i<nWord;i++)
    {
        fprintf(fp_log, "%s", wordseq[i]);
        if(ppr[i]==1)
        {
            fprintf(fp_log, "|");
        }
        else if(ppr[i]==2)
        {
            fprintf(fp_log,"$");
        }
        else if(pwr[i]==1)
		{
                fprintf(fp_log, " ");
        }
        fflush(fp_log);
    }
    fprintf(fp_log, "\n\n");
	fflush(fp_log);

	///////////////////////////////  �ϳ�  /////////////////////////////////
	
	// // ori-105 =0.0 
	// = 0.5ʱ��ɫ��ĸ���Ů���ˡ�����
	double half_tone = 0.0;    

	// ori-105=FALSE  ��ʾ�����label�����Ƿ����ʱ����Ϣ �ĳ�TRUE �ޱ仯 
	HTS_Boolean phoneme_alignment = FALSE; 
	 
	// ori-105=1.0  �ĳ�1.2�� ���ӿ�ͷ����ȱʧ������
	double speech_speed = 1.0; 



	/* synthesis */
	HTS_Engine_refresh(engine);  // �¼� 

	//// load label file   ����label�ļ� 
 	HTS_Engine_load_label_from_fn(engine, labfn);  

	if (phoneme_alignment)       /* modify label */
		HTS_Label_set_frame_specified_flag(&(engine->label), TRUE);
	
	if (speech_speed != 1.0)     // modify label  ori-105
	//if (abs(speech_speed-1.0) > 1e-6)     // modify label mod-szm 
		HTS_Label_set_speech_speed(&(engine->label), speech_speed);
	
	//�����滮����
	// parse label and determine state duration 
	HTS_Engine_create_sstream(engine);  
	
	// modify f0 
	//if (abs(half_tone) > 1e-6) // mod-szm
	if (half_tone != 0.0) // ori-105
	{      
		for (i = 0; i < HTS_SStreamSet_get_total_state(&(engine->sss)); i++) 
		{
			f = HTS_SStreamSet_get_mean(&(engine->sss), 1, i, 0);
			f += half_tone * log(2.0) / 12;
			if (f < log(10.0))
				f = log(10.0);
			HTS_SStreamSet_set_mean(&(engine->sss), 1, i, 0, f);
		}
	}
	HTS_Engine_create_pstream(engine);  /* generate speech parameter vector sequence */
	HTS_Engine_create_gstream(engine);  /* synthesize speech */

	
	// htsԭʼ �洢short���� 
	int len_short = HTS_Engine_speech2short(engine, out, out_size);

	/*
	// vocoder !!!
	double *gen, *lf0;
	lf0 = (double*)malloc(sizeof(double)* 20000);
	for (i = 0, msd_frame = 0; i<engine->pss.total_frame; i++)
	{
		if (engine->pss.pstream[1].msd_flag[i])
		{
			lf0[i] = engine->pss.pstream[1].par[msd_frame][0];
			msd_frame++;
		}
		else
		{
			lf0[i] = 0;
		}
	}
	//�ϳɹ���
	gen = LPCSynth(engine->pss.pstream[0].par + 5, lf0 + 5,
		engine->pss.pstream[0].static_length - 1, engine->pss.total_frame - 5, &len);

	// �� gen������456�������� ���� 
	short short0 = 0;
	for (i = 0; i<len - 456; i++)
	{
		temp = (short)(gen[i] * 32700);
		if (i < out_size)
		{
			out[i] = temp;
		}
		else
		{
			printf("out size too small!\n");
			return -1;
		}
	}

	// out���� LEN_SIL=300�������� ���Ͼ��� 
	for (i = 0; i<LEN_SIL; i++)
	{
		if (i + len - 456 < out_size)
		{
			out[i + len - 456] = short0;
		}
		else
		{
			printf("out size too small!\n");
			return -1;
		}

	}

	// end vocoder 
	*/

	//// ���̾��Ӽ�ľ�������  ǰ�������1���� 
	//int len_del = this->sampling_rate * 0.2;	
	//for (int ii = len_del; ii < len_short; ii++)
	//{
	//	out[ii - len_del] = out[ii];
	//}
	//len_short -= 2*len_del;
	
	// ��ӡlog 
	if (tracefp != NULL)
		HTS_Engine_save_information(engine, tracefp);
	if (durfp != NULL)
		HTS_Engine_save_label(engine, durfp);
	if (rawfp)
		HTS_Engine_save_generated_speech(engine, rawfp);
	if (wavfp)
		HTS_Engine_save_riff(engine, wavfp);
	if (mgcfp)
		HTS_Engine_save_generated_parameter(engine, mgcfp, 0);
	if (lf0fp)
		HTS_Engine_save_generated_parameter(engine, lf0fp, 1);
	if (lpffp)
		HTS_Engine_save_generated_parameter(engine, lpffp, 2);

	fprintf(fp_log, "LPCSynth ok!\n");
	fflush(fp_log);

	/* free */
	HTS_Engine_refresh(engine);



	/* close files */
	if (durfp != NULL)
		fclose(durfp);
	if (mgcfp != NULL)
		fclose(mgcfp);
	if (lf0fp != NULL)
		fclose(lf0fp);
	if (lpffp != NULL)
		fclose(lpffp);
	if (wavfp != NULL)
		fclose(wavfp);
	if (rawfp != NULL)
		fclose(rawfp);
	if (tracefp != NULL)
		fclose(tracefp);
	

	//if (lf0 != NULL)
	//{
	//	free(lf0);
	//	lf0 = NULL;
	//}
	if (cif != NULL)
	{
		free(cif);
		cif = NULL;
	}

	if (pwr != NULL)
	{
		free(pwr);
		pwr = NULL;
	}
	if (ppr != NULL)
	{
		free(ppr);
		ppr = NULL;
	}
	if (ptag != NULL)
	{
		free(ptag);
		ptag = NULL;
	}
	for (i = 0; i<NUM_SEQ; i++)
	{
		free(wordseq[i]); 
		free(posseq[i]);
		free(pinyinseq[i]);
	}
	free (wordseq);
	free(posseq);
	free(pinyinseq);
	
	return len_short;
	//return len - 456 + LEN_SIL;

}

int TTS::lines2short_array(const char *lines, short *out, int out_size)
{


    int pos = 0;
    int size_wav = 0;
    int size_tmp = 0;
    short *p_out = out;

    printf("###lines:%s###",lines);
    std::string str_lines(lines);
    std::string line;


    pos = str_lines.find_first_of("\n\r");
    while(pos >= 0)
    {
        line = str_lines.substr(0,pos);
        printf("inline:%s:::",line.c_str());

        size_tmp = line2short_array(line.c_str(), p_out, MAX_WAV_SIZE-size_wav);
        if(size_tmp < 0)
        {
            printf("line2short_array error!\n");
            return -1;
        }
        size_wav += size_tmp;
        p_out += size_tmp;

        str_lines = str_lines.substr(pos+1);
        pos = str_lines.find_first_of("\n\r");
    }

    line =  str_lines;
    if(line.length() >= 2)
    {
        printf("lastline:%s:::", line.c_str());

        size_tmp = line2short_array(line.c_str(), p_out, MAX_WAV_SIZE-size_wav);
        if(size_tmp <= 0)
        {
            printf("line2short_array error!\n");
            return -1;
        }
        size_wav += size_tmp;
        p_out += size_tmp;
    }

    return size_wav;
}


/* Error: output error message */
void TTS::Error(const int error, char *message, ...)
{
	va_list arg;

	fflush(stdout);
	fflush(stderr);

	if (error > 0)
		fprintf(stderr, "\nError: ");
	else
		fprintf(stderr, "\nWarning: ");

	va_start(arg, message);
	vfprintf(stderr, message, arg);
	va_end(arg);

	fflush(stderr);

	if (error > 0)
		exit(error);
}

/* Getfp: wrapper for fopen */
FILE *TTS::Getfp(const char *name, const char *opt)
{
	FILE *fp = fopen(name, opt);

	if (fp == NULL)
	{
		return NULL;
	//	Error(2, "Getfp: Cannot open %s.\n", name);
	}
	return (fp);
}

/* GetNumInterp: get number of speakers for interpolation from argv */
int TTS::GetNumInterp(int argc, char **argv_search)
{
	int num_interp = 1;
	while (--argc) {
		if (**++argv_search == '-') {
			if (*(*argv_search + 1) == 'i') {
				num_interp = atoi(*++argv_search);
				if (num_interp < 1) {
					num_interp = 1;
				}
				--argc;
			}
		}
	}
	return (num_interp);
}

