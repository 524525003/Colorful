#include "app_linein_eg.h"

//����ͨ��ʱ�Ĳ���
bool play(void)
{
    int result;

    bool bret = TRUE;
       
    //��TTS�����̣߳�������������Ƶ��
    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL2);

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);
    
    //ֹͣ or ��ͣʱ ���Ͳ�������
    if (g_eg_status_p->play_status == linein_play_status_stop)
    {
        //��ʼ���м����
        mmm_pp_cmd(&g_mmm_pp_handle, MMM_PP_OPEN, (unsigned int) NULL);

        //set sound open
        com_set_sound_out(SOUND_OUT_RESUME);

        g_eg_status_p->play_status = linein_play_status_play; //���ò���״̬

        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_OPEN, (unsigned int) &g_ainout_param);

	    //�����������ѡ��
	    //#define OUTPUT_CHANNEL_NORMAL      0 //�������м�����ʼ��Ϊ����
	    //#define OUTPUT_CHANNEL_L_R_SWITCH  1 //������������ L = R0, R = L0
	    //#define OUTPUT_CHANNEL_L_R_MERGE   2 //���������ϲ���������������������� L = R = (L0 + R0) / 2
	    //#define OUTPUT_CHANNEL_L_R_ALL_L   3 //������������������� L = R = L0
        //#define OUTPUT_CHANNEL_L_R_ALL_R   4 //������������������� L = R = R0
	    //mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_OUTPUT_CHANNEL, (unsigned int) OUTPUT_CHANNEL_NORMAL);
#ifdef WAVES_ASET_TOOLS
         //���Ͳ�������     
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, g_support_waves_pc_tools);
#else
        //���Ͳ�������
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) NULL);     
#endif

        if (result != 0)
        {
            bret = FALSE;
            _error_handle();
        }
        else
        {
            mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_ENERGY_INFO, (unsigned int) &(g_app_info_state_all.p_energy_value));
            g_app_info_state_all.energy_available_flag = TRUE;

            bret = TRUE;
        }
    }

    return bret;
}

//����ͨ��ʱ��ֹͣ
bool stop(void)
{
    bool bret = TRUE;
    int result = 0;
    
    //mmm_pp_status_t play_status;

    g_app_info_state_all.energy_available_flag = FALSE;
    
    /*mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_STATUS, (unsigned int) &play_status); //��ȡ����ʱû��

    if(play_status.status == MMM_PP_ENGINE_STOPPED)
    {
    	 //return bret;
    	 g_eg_status_p->play_status = linein_play_status_stop;
    }*/

    //�������ŲŻ�ֹͣ
    if (g_eg_status_p->play_status == linein_play_status_play)
    {
#ifdef WAVES_ASET_TOOLS
        //ֹͣ����
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, g_support_waves_pc_tools);     
#else
        //ֹͣ����
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) NULL);
#endif
        if (result != 0)
        {
            bret = FALSE;
        }
        else
        {           
            bret = TRUE;
        }

        /*if (bret == FALSE)
        {
        }*/ 
        //����״̬
        g_eg_status_p->play_status = linein_play_status_stop;
        //�ر������豸
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_CLOSE, (unsigned int) NULL);
        //�ر��м���豸
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_CLOSE, (unsigned int) NULL);

        //�ر���Ƶ���
        com_set_sound_out(SOUND_OUT_PAUSE);
    }

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL2, FREQ_NULL);
    adjust_freq_add_value(0, 0);

    return bret;
}
