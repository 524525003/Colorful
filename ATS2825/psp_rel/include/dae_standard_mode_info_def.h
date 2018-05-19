/********************************************************************************
 *                              GL5116
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 *      History:
 *      <author>            <time>           <version >             <des>
 *     zhuangyongkang    2016-05-26 10:00       1.0             Standard Sound Models
 ********************************************************************************/

#ifndef _DAE_PARA_STANDARD_MODE_INFO_DEF_H
#define _DAE_PARA_STANDARD_MODE_INFO_DEF_H

#include <typeext.h>
#include <attr.h>
#include <dae_info_def.h>

//NOTE:��׼��Чģ��
#define EQ_NUM_BANDS_STANDARD_MODE             EQ_NUM_BANDS
#define MDRC_NUM_BANDS_STANDARD_MODE            3

struct eq_band_setting_standard_mode    
{
    short cutoff;                       //����Ƶ�ʣ�20:1:24000Hz
    short q;                            //0.3:0.1:3����ֵ����10��������Χ-3:1:30
    short gain;                         //-20:0.1:12dB����ֵ����10��������Χ-200:1:120
    short type;                         ///*�˲������ͣ�0����Ч��1�ǳ���EQ�˲�����2�Ǹ�ͨ�˲���������ֵΪreserve*/
};

struct mdrc_band_setting_standard_mode
{
    short threshold1;
    short compress_ratio1;
	short threshold2;
    short compress_ratio2;
	short tav_time;
    short attack_time;
    short release_time;
	short reserve;//ȷ��4�ֽڶ���
};

typedef struct
{

    int sample_rate;            //�����ʣ�֧��44100,48000Hz����
    int channels;               //Ŀǰ���ǰ�˫ͨ������
    int block_size;             //֡����ĿǰΪ128�㳤��
	
	int fade_in_time_ms;        //���볤�ȣ�[50 100 200 300 400 500]ms
    int fade_out_time_ms;       //�������ȣ�[50 100 200 300 400 500]ms
    int mute_time_ms;           //�������ȣ�����û�����ƣ���λms

    int fade_in_flag;           //�����־λ����1��ʾ��һ֡��ʼ����
    int fade_out_flag;          //������־λ����1��ʾ��һ֡��ʼ����
    int mute_flag;              //������־λ����1��ʾ��һ֡��ʼ����
	
    int DAE_init_flag;          //��ʼ����ʶλ
    int DAE_change_flag;        //DAEȫ�������ı��ʶλ
    short  output_channel_config;//1:������������ 2: ���˶���Ӧ��(L=R=ԭʼ��(L/2 + R/2)), 0 or ����ֵ, ��ԭʼ����������һ�£��ù�������Ч�޹أ����� BYPASS Ӱ��
    short  bypass;               //bypass,1��ʾֱͨ��0��ʾDAE����
    int precut;                 //Ԥ˥������ʼ��Ϊ0��precut ���� BYPASS Ӱ��
	/***********************************/

//ȷ��4�ֽڶ���
    short Vbass_enable;         //Vbass����
    short Surround_enable;      //Surround����
    short TrebleEnhance_enable; //Treble����
    short Peq_enable;           //Peq����
    short SpeakerCompensation_enable;//SpeakerCompensation����
	short Compressor_enable;
    short MultibandDRC_enable;  //MDRC����
    short reserve_0;

//ȷ��4�ֽڶ���
    /***********************************/
    int Vbass_low_cut_off_frequency;//vbass����Ƶ�ʣ���ʼ������80hz
    int Vbass_gain;             //vbass����,��λΪ0.1dB��-120��10��120dB����ʼ������60dB
    int Vbass_type;             //�������������
    /***********************************/
    int Surround_angle;         //10:10:60��,˫����������ͷ�ļнǣ���ʼ��Ϊ10��
    int Surround_gain;          //���������棬-60:1:0dB����ʼ��Ϊ-3dB
    /***********************************/
    int Treble_frequency;       //������ǿ��ʼƵ�ʣ�8000:1000:20000Hz����ʼ��Ϊ10000Hz
    int Treble_gain;            //������ǿ����,��λΪ0.1dB��0:10:150dB����ʼ��Ϊ100dB
    /***********************************/
    //int MDRC_change_flag;     //MDRC�����Ĳ����ı��ʶλ��MDRC�����ı�ʱ��1��ͬʱDAE_change_flag��1
    int crossover_freqency[MDRC_NUM_BANDS_STANDARD_MODE-1];
    short MDRC_compensation_peak_detect_attack_time;	//default: 0.5ms, step: 0.01ms, setting: 5*10
	short MDRC_compensation_peak_detect_release_time;	//default: 500ms, step: 1ms, setting: 500
	short MDRC_compensation_threshold;				//default: -100,step 0.1dB,setting: -10*10 = -100
	short MDRC_compensation_filter_Q;					//default: 1,step 0.1, setting:0.1*10 = 1, 0.1~0.9
    /***********************************/
    int Peq_change_flag;        //peq�����Ĳ����ı��ʶλ��peq�����ı�ʱ��1��ͬʱDAE_change_flag��1
    struct eq_band_setting_standard_mode band_settings[EQ_NUM_BANDS_STANDARD_MODE]; //peqƵ���������band_settings���ĸ��������иı�ʱPeq_change_flag��DAE_change_flagͬʱ��1
    short band_enable_type[EQ_NUM_BANDS_STANDARD_MODE]; //peq���ͺ�enableѡ�1��ʾSpeaker PEQ��2��ʾPost PEQ��0��ʾ��ִ��
    /***********************************/
    short SpeakerCompensationChangeFlag;
    short FilterOrder;
    short FilterQvalLeft;
    short FilterQvalRight;
    /***********************************/
    int makeup_gain;            //makeup gain �������µ����ã��������������ȣ�С������Ϊ0
	int makeup2;                //���źŲ���0db    
    /***********************************/
    short energy_detect_enable; //�ź��������ʹ��
    short period;               //ͳ������ʱ������1msΪ��λ
    short period_count;         //��Ч������
    short preadjust_db;         //�źŴ�С���ͽ϶������Ԥ�����ƣ���0.1dbΪ��λ
    short preadjust_count;      //��ʼԤ�������źŴ�С����������Ȼ��ÿ�����ڽ�����Ӧ����
//signal_energy_inner�������energyǰһ��λ��
    short signal_energy_inner;  //����������ȥ����ֵ���� �л��źż��ʱ��ų�ʼֵ
    /***********************************/
    short  energy;     //����֡pcmƽ��ֵ
    short  energy_max; //����֡pcm����ֵ���ֵ
//����3����Ա���������
    short  max_vol_flag;
    short  reserve_2;//ȷ��4�ֽڶ���
    short  reserve_3;
    short  reserve_4;
    short  reserve_5;
    short  reserve_6;
    short  reserve_7;
    short  reserve_8;

    /***********************************/
	int Compressor_threshold1;
    int Compressor_ratio1;
	int Compressor_threshold2;
    int Compressor_ratio2;
	int Compressor_tav;
	int Compressor_attack_time;
	int Compressor_release_time;
    
    struct mdrc_band_setting_standard_mode mdrc_band_settings[MDRC_NUM_BANDS_STANDARD_MODE]; //MDRC����
  
}DAE_para_info_standard_mode_t;


#endif //_DAE_PARA_H
