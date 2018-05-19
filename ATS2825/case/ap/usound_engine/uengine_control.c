/*******************************************************************************
 *                              US211A
 *                            Module: music engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     fiona.yang     2011-09-06 15:00     1.0             build this file
 *******************************************************************************/
#include "app_uengine.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \int32 uegine_player_open(void)
 * \���м��
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
int32 uegine_player_open(void)
{
    int open_ret = 0;
    if(0 == g_uspeaker_24bit)
    {
        //bool init_fsel_ret;
        //char *mmm_name =
        //{ "mmm_pp.al" };

        //�����м������
        //sys_load_mmm(mmm_name, TRUE);//������

        if (g_mmm_pp_handle != NULL)
        {
            return 0;
        }

        adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL10, FREQ_LEVEL10);

        adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

        //��ʼ���м����
        open_ret = mmm_pp_cmd(&g_mmm_pp_handle, MMM_PP_OPEN, (unsigned int) NULL);

        if (open_ret != -1)
        {
            open_ret = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_OPEN, (unsigned int) &g_ain_out_setting);
        }

		//�����������ѡ��
		//#define OUTPUT_CHANNEL_NORMAL      0 //�������м�����ʼ��Ϊ����
		//#define OUTPUT_CHANNEL_L_R_SWITCH  1 //������������ L = R0, R = L0
		//#define OUTPUT_CHANNEL_L_R_MERGE   2 //���������ϲ���������������������� L = R = (L0 + R0) / 2
		//#define OUTPUT_CHANNEL_L_R_ALL_L   3 //������������������� L = R = L0
		//#define OUTPUT_CHANNEL_L_R_ALL_R   4 //������������������� L = R = R0
		//mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_OUTPUT_CHANNEL, (unsigned int) OUTPUT_CHANNEL_NORMAL);

        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) NULL);

        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_ENERGY_INFO, (unsigned int) &(g_app_info_state_all.p_energy_value));

        g_app_info_state_all.energy_available_flag = TRUE;

        //start get data timer
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_START_SEND, (unsigned int) NULL);

    	ud_set_cmd(SET_ADJUST_TIMER, 1);
    }
    else
    {
        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_SAMPLE_RATE);
        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_PLAY);
    }
    return open_ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void uegine_player_close(void)
 * \�ر��м��
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/

void uegine_player_close(void)
{
    if(0 == g_uspeaker_24bit)
    {
        uint32 fade_out_use = 0;

        ud_set_cmd(SET_ADJUST_TIMER, 0);

        if (g_mmm_pp_handle == NULL)
        {
            return;
        }

        g_app_info_state_all.energy_available_flag = FALSE;

        if (g_status_data.start_play == 0)
        {
            mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_FADE_OUT_TIME, (unsigned int) fade_out_use);
        }

        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) NULL);

        //stop get data timer
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP_SEND, (unsigned int) NULL);

        //�ر������豸
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_CLOSE, (unsigned int) NULL);

        //�ر��м���豸
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_CLOSE, (unsigned int) NULL);

        g_mmm_pp_handle = NULL;

        adjust_freq_add_value(0, 0);

        adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);

        //ж�ؽ�������
        //sys_free_mmm(TRUE);
    }
    else
    { 
        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_PAUSE);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void uengine_status_deal(void)
 * \����״̬����,����ǰ���ֲ���״̬
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
void uengine_status_deal(void)
{
    ;//do nothing
}
/******************************************************************************/
/*!
 * \par  Description:
 * \void uegine_check_status(void)
 * \�������״̬��ʱ�����������ñ�־λ
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
void uegine_check_status(void)
{
    g_check_status_flag = TRUE;
}
//�����������������Ϊ�˽��win7�´󲿷ֲ���������ͣʱ���ᷢ��20��m���ҵ�ISO���ݹ���
//����С��û�취�������ŵ�ǰ�Ĳ���״̬���Ӷ����ֲ��źͲ������ҵ�����
void check_energy_info(void)
{
    short *p_energy_mean;
    short *p_energy_max;

    if (g_app_info_state_all.energy_available_flag == TRUE)
    {
        p_energy_mean = g_app_info_state_all.p_energy_value;
        p_energy_max = g_app_info_state_all.p_energy_value + 1;
        if ((*p_energy_mean == 0) && (*p_energy_max < 5))
        {
            energy_count++;
            if (energy_count == MAX_ENERGY_NUM)
            {
                //���������ͱ�־����֪������ǰ������ͣ״̬
                ud_set_cmd(SET_PLAY_FLAG, 3);
            }
        }
        else
        {
            if (energy_count >= MAX_ENERGY_NUM)
            {
                //��������ͱ�־���ָ�����״̬����
                ud_set_cmd(SET_PLAY_FLAG, 4);
            }
            energy_count=0;
        }
        //libc_print("\n energy_mean:", *p_energy_mean,2);
        //libc_print("\n energy_max:", *p_energy_max,2);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void uengine_control_block(void)
 * \���湦�ܴ�����
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
app_result_e uengine_control_block(void)
{
    //mmm_pp_status_t play_status;

    private_msg_t cur_msg_val;

    uint32 hid_count = 0;

    app_result_e uengine_retval = RESULT_NULL;

    uint8 i = 0;

    //250ms����hid ����
    //g_hid_timer = set_app_timer(APP_TIMER_ATTRB_CONTROL, 500, uegine_deal_hid);
    uint32 auto_play_count = 0;

    uint8 auto_play_flag = 0;
    
    while (1)
    {
        //��ѯ��ǰ����״̬
        ud_get_status(&g_status_data);
        if (g_status_data.start_play == 0)
        {
            g_stop_count++;

            if (g_stop_count >= 5)
            {
                g_cur_play_status_p->play_status = 0;
            }
        }
        else
        {
            g_stop_count = 0;

            g_cur_play_status_p->play_status = 1;
        }

        g_cur_play_status_p->line_status = g_status_data.line_sta;

        //���¹����ڴ�
        g_cur_play_status_p = sys_share_query_update(APP_ID_UENGINE);

        if (((g_volume_syn_cfg & 0x02) != 0) && (g_status_data.volume_chg != 0))
        {

            ud_set_cmd(SET_VOLUME_FLAG, 0);

            //change volume
            uengine_sync_volume(g_status_data.volume_value);
           
        }

        //hid change

        hid_count++;

        if ((g_status_data.hid_idle_time != 0) && (g_status_data.hid_idle_time < (hid_count * 10)))
        {
            hid_count = 0;

            ud_set_cmd(SET_HID_CHANGE, 0);
        }

        //play flag need set sample rate
        if ((g_status_data.start_play == 0x01) && (g_status_data.set_sample == 1))
        {
            if (g_mmm_pp_handle != NULL)
            {
	            //set adjust
           	 	ud_set_cmd(SET_ADJUST_TIMER, 0);

                mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_PARAM, (unsigned int) &g_status_data.sample_rate);

                g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;

                g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;

                //clr flag
                ud_set_cmd(SET_SAMPLE_RATE, 0);

            	ud_set_cmd(SET_ADJUST_TIMER, 1);
            }
            else if(1 == g_uspeaker_24bit)
            {
                if(g_ain_out_setting.dac_sample != (int32) g_status_data.sample_rate)
                {
                    ccd_i2s_pa_set_clock(g_status_data.sample_rate);
                }
                
                g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;
                
                g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;

                //ccd_i2s_pa_set_clock(0xff);
                
                ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_SAMPLE_RATE);
            }
            else
            {
                ;//for QAC
            }
        }

        if ((g_status_data.start_record == 0x01) && (g_status_data.set_sample == 1))
        {
            //mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_PARAM, (unsigned int) &g_status_data.sample_rate);
            if (g_mmm_pp_handle != NULL)
            {
                mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_PARAM, (unsigned int) &g_status_data.sample_rate);

                g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;

                g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;

                //clr flag
                ud_set_cmd(SET_SAMPLE_RATE, 0);
            }
        }

        //��ȡ��������ϸ
        if (get_app_msg(&cur_msg_val) == TRUE)
        {
            uengine_retval = uengine_message_deal(&cur_msg_val);
        }

        if (uengine_retval == RESULT_APP_QUIT)
        {
            break;
        }
        check_energy_info();
        if (i == 10)
        {
            ud_hid_deal();
            i = 0;
        }
        if (auto_play_count >= 100)
        {
            auto_play_flag = 1;
            auto_play_count = 0;
            if((g_status_data.start_play == 0)&&(g_auto_play_flag == 1))
            {
                ud_set_cmd(SET_HID_OPERS, 0x08);
                g_auto_play_flag = 0;
            }
            else
            {
                g_auto_play_flag = 0;
            }
        }
        //����10ms �����񽻻�
        sys_os_time_dly(1);
        
        if(auto_play_flag == 0)
        {
            auto_play_count++;
        }
        
        i++;

    }

    //kill_app_timer(g_hid_timer);

    return uengine_retval;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_reply_msg(void* msg_ptr, bool ret_vals)
 * \�ظ�ͬ����Ϣ
 * \param[in]    void  msg_ptr ��Ϣ�ṹ��
 * \param[in]    bool  ret_vals ����¼������� TRUE �ɹ� FALSE ʧ��
 * \param[out]   none
 * \return       app_result_E
 * \retval       RESULT_IGNOR ����
 * \ingroup      uengine_control.c
 * \note  �ظ���Ϣʱ������ret_vals�Ľ��������Ӧ��ɹ���ʧ��
 */
/*******************************************************************************/
void uengine_reply_msg(void* msg_ptr, bool ret_vals)
{
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    if (ret_vals == TRUE)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }

    //�ظ�ͬ����Ϣ(�����ź���)
    libc_sem_post(data_ptr->sem);

    //return;
}

