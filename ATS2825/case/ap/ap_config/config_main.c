/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_main.c
 * \brief    ���ػ��Ĵ���
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_config.h"
#include "uhost.h"

/*ȫ�ֱ�������*/
//Ӧ���Լ���Ҫ����ʱ������
#define APP_TIMER_COUNT     1

/*!
 *  \brief
 *  app_timer_vector��Ӧ����ʱ������ָ�룬ָ��Ӧ�ÿռ����ʱ������
 */
app_timer_t conf_app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];
//ϵͳ����
comval_t g_comval;
//configӦ�ñ���
g_config_var_t g_config_var;
//wav¼�������д�ļ�ͷ�ṹ��
record_writehead_t g_writehead;
//�Ƿ��һ���ϵ��־
bool g_first_boot = FALSE;

/*main��Ҫ���������ֵ����ϵͳ��Ա����*/
OS_STK *ptos = (OS_STK *) AP_FRONT_LOW_STK_POS;
INT8U prio = AP_FRONT_LOW_PRIO;

bool usb_in_flag = FALSE;
bool uhost_in_flag = FALSE;
bool adaptor_in_flag = FALSE;
bool sd_in_flag = FALSE;
bool linein_in_flag = FALSE;
bool rtcalarm_flag = FALSE;
bool g_config_standby_exit = FALSE;
bool g_config_bt_flag = FALSE;
bool g_config_esd_restart = FALSE;

/******************************************************************************/
/*!
 * \par  Description:
 *  ��ȡӦ��VMȫ�ֱ���,��ʼ��ȫ�ֱ���
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
void __section__(".bank_var") _read_var(void)
{
    //��ȡcommon VM����������Ϣ
    com_setting_comval_init(&g_comval);

    sys_vm_read(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));
    //��ʼ��config��VM����
    if ((g_config_var.magic != VRAM_MAGIC(VM_AP_CONFIG)) || ((uint8) ((act_readl(RTC_BAK0) \
        & (0xff << MY_RTC_FUNC_INDEX)) >> MY_RTC_FUNC_INDEX) == APP_FUNC_INVALID))
    {
        //��һ���ϵ�
        g_first_boot = TRUE;
        g_config_var.magic = VRAM_MAGIC(VM_AP_CONFIG);

        g_config_var.esd_flag = FALSE;

        sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));

        //���S3BT��־
        act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_S3BT)), RTC_BAK0);
        config_flush_rtc_reg();
        
        act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_BT_FLAG)), RTC_BAK0);
        config_flush_rtc_reg();
        
        //���ESD RESTART��־
        act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_ESD_FLAG)), RTC_BAK0);
        config_flush_rtc_reg();

        //��һ���ϵ���Ҫ����˯�߻��ѷ�ʽ
        en_play_wake_up(FALSE);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  applib��ʼ��
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
bool _app_init(void)
{
    //��ʼ��applib�⣬ǰ̨AP
    applib_init(APP_ID_CONFIG, APP_TYPE_GUI);

    //��ʼ����ʱ��
    init_app_timers(conf_app_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    //��ʼ�� applib ��Ϣģ��
    applib_message_init();

    /*��ʼ��ϵͳ��ʱ��*/
    sys_timer_init();

    com_rcp_init();//������xxx_rcp_var_init֮ǰ

    com_view_manager_init();

    return TRUE;
}

bool _app_exit(void)
{
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    com_view_manager_exit();

    //����ϵͳ��ʱ��
    sys_timer_exit();

    //ִ��applib���ע������
    applib_quit();

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    configӦ�õ���ں���
 * \param[in]    ���ػ�������0-������1-�ػ���S4��2-�͵�ػ���S4��3-�͹��Ľ�S3
 * \param[out]   none
 * \return       the result
 * \retval
 * \retval
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note �ػ�˵������ع���ʱ���ػ������stanby������ʱ������ϵͳ
 ��Դ����ʱ���ػ�ֻ�ǹ���ϵͳ���ٿ���ʱ��������
 *******************************************************************************/
int main(int argc, const char *argv[])
{
    app_result_e result = RESULT_NULL;

    bool power_on_flag = 0;
    if(((uint32) argc & 0xff) == POWER_ON)
    {
        power_on_flag = 1;
        argc &= 0xff00;
    }
    
    //��������
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL8, FREQ_NULL);

    _read_var(); //g_comval �ĳ�ʼ�������� config_globe_data_init ֮ǰ

    if (((uint32) argc & 0xff) == SWITCH_ON)
    {
        //DAE���û����ѳ�ʼ���õı�־��ȷ��������DAE���û�����ʼ����֮ǰ����DAE��������
        g_comval.dae_inited_flag = FALSE;

        //��ʼ������ǰ̨Ӧ�ù����ȫ�ֱ�������Щȫ�ֱ���������ֻ�ܳ�ʼ��һ�Σ�
        config_globe_data_init(); //������ _app_init ֮ǰ����

        if ((act_readl(RTC_BAK0) & (1 << MY_RTC_S3BT)) != 0)
        {
            if ((act_readl(RTC_BAK0) & (1 << MY_RTC_BT_FLAG)) != 0)
            {
                g_config_bt_flag = TRUE;
            }

            //���S3BT��־
            act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_S3BT)), RTC_BAK0);
            config_flush_rtc_reg();
            act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_BT_FLAG)), RTC_BAK0);
            config_flush_rtc_reg();

            g_config_standby_exit = TRUE;
            if(power_on_flag)       //����ϵ�
            {
                libc_print("UNEXP SHUT DOWN",0,0);
                g_config_bt_flag = FALSE;
                g_config_standby_exit = FALSE;
            }
        }
        else if (((act_readl(RTC_BAK0) & (1 << MY_RTC_ESD_FLAG)) != 0) || (g_config_var.esd_flag == TRUE))
        {
            //����¼��ap func index�Ƿ�Ϸ������Ϸ���ap func index���й���
            //��ֹ�����ֹػ�
            if(com_check_ap_func_index_valid() == TRUE)
            {
                g_customer_state |= CUSTOMER_ESD_WAKEUP;
                //�����ESD RESTART��־����AP�Լ����
                g_config_esd_restart = TRUE;
            }
        }
        else
        {
            ;//nothing for qac
        }
    }
    
    g_config_var.esd_flag = TRUE;
    
    sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));

    //applib initial
    _app_init();

    if (((uint32) argc & 0xff) == SWITCH_ON)
    {
        config_sys_init_display();

        //������ǰ����BT STACK
        if ((g_config_standby_exit == TRUE) && (g_config_bt_flag == TRUE))
        {
            com_btmanager_init(TRUE);//pathcode�Ѽ��أ������ټ���
        }

        //������ʱ�ӣ���������������
        card_set_drv_power();

        config_key_module();

        //ÿ�ο���������ĳЩDAE���������
        com_reset_dae_config(&g_comval);
        //��ʼ��DAE���ò��������ڴ�
        com_init_dae_config(&g_comval);

        if ((g_comval.hard_support_uhost == 1) && (g_comval.support_uhostplay == 1))
        {
            UHOST_POWER_OPEN();
            UHOST_POWER_ENABLE();
            //set uhost cfg
            g_uhost_cfg = (uint8) com_get_config_default(SETTING_UHOST_USE_DELAY);
        }

        config_sys_init();

        if (g_config_standby_exit == TRUE)
        {
            PRINT_INFO("EXIT S3BT");

            if ((g_comval.hard_support_uhost == 1) && (g_comval.support_uhostplay == 1))
            {
                if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) == 0)
                {
                    //�л���UHOST����ʱ�ȴ�ϵͳ���
                    key_peripheral_detect_handle(PER_DETECT_UHOST_FORCE);
                }
            }

            result = RESULT_SYSTEM_EXIT_S3;
        }
        else
        {
            result = config_poweron_option();

            if (g_config_esd_restart == TRUE)
            {
                PRINT_INFO("ESD RESTART");
                result = RESULT_ESD_RESTART; //ESD RESTART ����Ὺ��ѡ��ִ�п���ѡ�������Ϊ�˱�֤����״̬��ȷ
            }

            if (result == RESULT_APP_QUIT)
            {
                goto config_exit;
                //�Զ����ԣ����Լ�����Ӧ�ã��������˳�
            }

            if ((result != RESULT_LOW_POWER) && (result != RESULT_ESD_RESTART)) //�͵粻���κ�����ػ�
            {
                config_record_writehead();
            }
        }

        //����������
        keytone_set_on_off(TRUE);

        Reset_start:
#if (SUPPORT_OUTER_CHARGE == 1)
        if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
        {
            if(result == RESULT_USB_TRANS)
            {
                if (IS_EXTERN_BATTERY_IN () == 1)
                {
                    result = RESULT_NULL;
                }
            }
        }
#endif
        config_power_on_dispatch(result);
    }
    else //�����ǹػ�����
    {
        if (((uint32) argc & 0xff) == SWITCH_LOWPOWER)
        {
            //��������
            com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NORECV_ALL_EV_YES, (void *) TTS_BATTERY_LOW);
            sys_os_time_dly(50);
        }

        /*�����ػ�*/
        if ((((uint32) argc & 0xff) == SWITCH_OFF) || (((uint32) argc & 0xff) == SWITCH_LOWPOWER))
        {
            if (g_ap_switch_var.rtcalarm_poweroff == FALSE) //������Ϻ�ػ�������TTS
            {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
                //tws��ֻ������ſ��Բ����ػ�
                if(g_bt_stack_cur_info.dev_role!=TWS_SLAVE)
#endif
                {
                    com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NORECV_ALL_EV_YES, (void *) TTS_POWEROFF);
                }
                
            }
        }

        //����S3BT
        if (((uint32) argc & 0xff) == SWITCH_STANDBY)
        {
            //������ʱ���յ�PD��־λ�������˳�����
            if((act_readl(INTC_PD) & 0x01) != 0)
            {
                PRINT_INFO("exit config");
                com_ap_switch_deal(RESULT_SYSTEM_EXIT_S3);  
                goto config_exit;
            }
            else
            {
            	//BTSTACK �˳�
            	g_config_var.esd_flag = FALSE;          //���ESD���
                act_writel(act_readl(RTC_BAK0) | (1 << MY_RTC_BT_FLAG), RTC_BAK0);
                config_flush_rtc_reg();
                com_btmanager_exit(TRUE, FALSE);
                result = sys_power_off(TRUE);
            }
        }
        else
        {
            g_config_var.esd_flag = FALSE;          //���ESD���
            sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));

            //BTSTACK �˳�
            com_btmanager_exit(FALSE, TRUE);
            sys_os_time_dly(50);

            //�������ģʽ
            if (((uint32) argc & 0xff) == SWITCH_IDLE_MODE)
            {
                result = config_idle_mode_loop();
            }
            //�ϵ�ػ�
            else
            {
                result = sys_power_off(FALSE);
            }
        }

        goto Reset_start;
    }

    config_exit: _app_exit();

    return RESULT_NULL;
}

