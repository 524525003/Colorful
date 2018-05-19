/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
#include "ap_udisk.h"

/*ȫ�ֱ�������*/

//Ӧ���Լ���Ҫ����ʱ������
#define APP_TIMER_COUNT    2

#define    AP_RADIO_STK_POS  AP_FRONT_LOW_STK_POS

#define    AP_RADIO_PRIO     AP_FRONT_LOW_PRIO

app_timer_t g_mainmenu_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

/*main��Ҫ���������ֵ����ϵͳ��Ա����*/
OS_STK *ptos = (OS_STK *) AP_RADIO_STK_POS;
/* Radio UI ���̵����߳����ȼ�*/
INT8U prio = AP_RADIO_PRIO;

uint32 g_vctl_back;

uint8 g_info_str[52];

//p_adfu_launcher
p_func_entry p_adfu_launcher;

int32 g_pop_up_timer;

uint32 g_pop_up_time_out;

//uint8 g_key_play;

//ϵͳ����
comval_t g_comval;

/******************************************************************************/
/*!
 * \par  Description:
 *      MainMenu ��ȡӦ��VMȫ�ֱ���,��ʼ��ȫ�ֱ���
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _read_var(void)
{
    //��ȡcommon VM����������Ϣ
    com_setting_comval_init(&g_comval);
}

/******************************************************************************/
/*!
 * \par  Description:
 *      applib��ʼ��
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _app_init(void)
{

    /*����VM����*/
    _read_var();

    //set init val
    g_pop_up_timer = -1;

    /*��ʼ��applib�⣬ǰ̨AP*/
    applib_init(APP_ID_UDISK, APP_TYPE_GUI);

    /*��ʼ����ʱ��*/
    init_app_timers(g_mainmenu_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    /*��ʼ�� applib ��Ϣģ��*/
    applib_message_init();

    /*��ʼ��ϵͳ��ʱ��*/
    sys_timer_init();

    com_rcp_init();//������xxx_rcp_var_init֮ǰ

    com_view_manager_init();

}

/******************************************************************************/
/*!
 * \par  Description:
 *      applib�˳�
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _app_exit(void)
{
    com_view_manager_exit();

    /*����ϵͳ��ʱ��*/
    sys_timer_exit();

    /*����VM����*/
    //sys_vm_write(&g_mainmenu_var, VM_AP_MAINMENU);
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    /*ִ��applib���ע������*/
    applib_quit();
}

/******************************************************************************/
/*!
 * \par  Description:
 * ����vdd and ��Ƶ forusb
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void _rise_vdd_and_freq(void)
{
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL11, FREQ_NULL);//max frequency
    sys_enter_high_powered(3);
}

/******************************************************************************/
/*!
 * \par  Description:
 * �ָ�vdd and freq
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void _back_vdd_and_freq(void)
{
    sys_exit_high_powered();
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
}

/******************************************************************************/
/*!
 * \par  Description:
 * ��ȡUSB��ص�������Ϣ
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void _set_usb_info(void)
{
    //uint8 info_str[16];

    //vid
    com_get_config_struct(USB_VID, g_info_str, 6);

    ud_set_config(0, g_info_str, 6);

    //pid
    com_get_config_struct(USB_CARDREADER_PID, g_info_str, 6);

    ud_set_config(1, g_info_str, 6);

    //vendor
    com_get_config_struct(INF_USB_VENDOR, g_info_str, 8);

    ud_set_config(2, g_info_str, 8);

    //product name
    com_get_config_struct(INF_CARDREADER_PRODUCT_NAME, g_info_str, 16);

    ud_set_config(3, g_info_str, 16);

    ////CMD_Inquiry_information_FOB
    com_get_config_struct(INF_INQUIRY_INFORMATION_FOB, g_info_str, 52);

    ud_set_config(4, g_info_str, 52);

}

/******************************************************************************/
/*!
 * \par  Description:
 *  ����Ƿ���usb���룬ȷ���Ƿ����U��Ӧ��
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music
 * \note
 *******************************************************************************/
uint8 _detect_usb_line_status(void)
{
    //uint32 temp_save;
    //private_msg_t pri_msg;
    unsigned char plug_in = 0;
    if (get_usb_port_state() == USB_PORT_HOST)
    {
        //��⵽uhost����
        if (sys_detect_disk(DRV_GROUP_STG_UHOST) == 0)
        {
            return FALSE;
        }

        key_peripheral_detect_handle(PER_DETECT_USB_FORCE);

        sys_os_time_dly(30);
    }

    //get usbcable plug status
    plug_in = (act_readb(DPDMCTRL) & (1 << DPDMCTRL_Plugin));

    if (plug_in != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  �л���adfu�Ĵ���
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      usb
 * \note
 *******************************************************************************/
void _switch_to_adfu(void)
{

    //void (*p_adfu_launcher)(void) = *(uint32 *)0xbfc002a0;

    p_adfu_launcher = *(uint32*) 0xbfc002a0;

    //select hosc
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);

    //rise vcc 3.3v and vdd 1.3v, improve usb signal.xiaomaky.2012-9-11 11:55.
    act_writel(((act_readl(VOUT_CTL) & 0xFFFFFF80) | 0x6A), VOUT_CTL);
    sys_mdelay(3);
#if (SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        led_clear_screen();
    }
#endif
    act_writel(0, INTC_CFG0);
    act_writel(0, INTC_CFG1);
    act_writel(0, INTC_CFG2); //assgin usb intrrupt to IP2

    act_writel(0x58, WD_CTL);

    p_adfu_launcher();

}
/******************************************************************************/
/*!
 * \par  Description:
 *  ɱ����̨����
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music
 * \note
 *******************************************************************************/
void _udevice_close_engine(void)
{
    msg_apps_t msg;
    engine_type_e engine_type = get_engine_type();

    if (engine_type != ENGINE_NULL)
    {
        msg.type = MSG_KILL_APP_SYNC;
        msg.content.data[0] = APP_ID_THEENGINE;

        send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
    }

    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      MainMenu Ӧ�õ���ں����ͳ���������ģ��
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return       the result
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
int main(int argc, const char *argv[])
{
    app_result_e result = RESULT_NULL;

#if (SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        led_clear_screen();
    }
#endif

    /*ap��ʼ��*/
    _app_init();

    _rise_vdd_and_freq();//_app_init֮��

    if (_detect_usb_line_status() != FALSE)
    {

        _config_disk_label();

#if (SUPPORT_LED_DRIVER == 1)
        if (g_comval.support_led_display == 1)
        {
            ud_paint_deal();
        }
#endif

        keytone_set_on_off(FALSE);

        //��������ͼ
        udisk_create_main_view();

        _udevice_close_engine();

        com_tts_state_play((TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK | TTS_MODE_SYS_NORECV), (void *) TTS_ENTER_UDISK);

        //ɱ��������̨
        com_btmanager_exit(FALSE, TRUE);

        //�ȴ�TTS����
        if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
        {
            sys_os_time_dly(10); //�ȴ�100ms���Ա�̰�̧���ܹ����
        }

        //U��ģʽ�²��ܽ���TTS����
        com_tts_state_play_control(TTS_CTRL_IGNORE);

        //usound_open_engine(APP_ID_UENGINE);

        sys_drv_install(DRV_GROUP_UD, 0, "udisk.drv");

        //some config set
        _set_usb_info();

        //disable detect when init
        DISABLE_USB_DETECT();
        //start init
        ud_module_start(NULL);
        //enable detect after init
        ENABLE_USB_DETECT();

        result = get_message_loop();

#if (SUPPORT_OUTER_CHARGE == 1)
        if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
        {
            if ((IS_EXTERN_BATTERY_IN () == 0) && (IS_EXTERN_CAHREGE_IN() == 1))
            {
                result = RESULT_IDLE_MODE; //�������ģʽ
            }
        }
#endif

        //disable keytone
        keytone_set_on_off(TRUE);

        //�˳�U��ģʽ����TTS����
        com_tts_state_play_control(TTS_CTRL_CANCEL_IGNORE);

        //disable detect when exit (need init)
        DISABLE_USB_DETECT();
        //stop and exit
        ud_module_stop();
        //enable detect after init
        ENABLE_USB_DETECT();

        sys_drv_uninstall(DRV_GROUP_UD);

    }

    //switch to adfu and upgrade
    if (result == RESULT_ENTER_UPGRADE)
    {
        _switch_to_adfu();
    }

    if (result == RESULT_ENTER_STUB_DEBUG)
    {
        libc_print("open stub", 0, 0);

        result = com_sys_install_stub();
    }

    if ((result == RESULT_LASTPLAY) || (result == RESULT_NEXT_FUNCTION))
    {
        result = RESULT_NULL;
    }

#if (SUPPORT_RING_FOLDER == 1)
    if ((int8) sys_detect_disk(DRV_GROUP_STG_CARD) != -1)
    {
        g_ap_switch_var.result_alarm_ring_scan_backup = result;
        result = RESULT_ALARM_RING_FOLDER_SCAN;
    }
#endif

    com_ap_switch_deal(result);

    _back_vdd_and_freq();

    _app_exit();

    g_app_info_state.need_update_playlist = UHOST_NEED_MASK | CARD_NEED_MASK;

    //for disk play to clr bk info
    g_ap_switch_var.card_in_out_flag = 1;

    g_ap_switch_var.uhost_in_out_flag = 1;

    return result;
}

