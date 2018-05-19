/*******************************************************************************
 *                              us212A
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-10-8 11:21     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_config.c
 * \brief    Ӧ�����ýű������ӿ�ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�׼򵥵�Ӧ�����ýӿڣ�������ȡ������ȡ�
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���sd �ļ�ϵͳ�ӿڽ��ж�λ�Ͷ�ȡ��
 * \version 1.0
 * \date  2011-10-8
 *******************************************************************************/

#include  "manager.h"
#include  "stub_interface.h"

// config �ű��ļ�����
const char cfg_file[] = "config.bin";

/* ���� stub �������Ҫ������ʱ���ز���פ */
static const char stub_name[] = { "UartStub.al" };

bool globe_data_init_applib(void)
{
    uint8 config_file[12];
    uint8 i, ret;

    for (i = 0; i < MAX_APP_COUNT; i++)
    {
        libc_memset(&(g_app_info_vector[i]), 0x00, sizeof(app_info_t));
        g_app_info_vector[i].app_id = APP_ID_MAX;
    }

    libc_memcpy(config_file, cfg_file, sizeof(cfg_file) + 1);
    ret = (uint8) com_open_config_file(config_file);

    libc_memset(&g_app_info_state_all, 0x00, sizeof(g_app_info_state_all));

    libc_memset(&g_app_info_state, 0x00, sizeof(g_app_info_state));
    //��0��ʼ��
    g_app_info_state.bat_value = BATTERY_GRADE_MAX;
    g_app_info_state.cur_func_index = 0xff; //for ap switch infor index invalid value
    g_app_info_state.need_update_playlist = UHOST_NEED_MASK | CARD_NEED_MASK;
    g_app_info_state.bt_power_on = TRUE;

    //����UART PRINT����
    if (ret == TRUE)
    {
        uint8 uart_enable;

        uart_enable = (uint8) com_get_config_default(SETTING_UART_PRINT_ENABLE);
        sys_set_sys_info(uart_enable, SYS_PRINT_ONOFF);
        if (uart_enable == TRUE)
        { //ʹ��UART TX
            reg_writel((act_readl(GPIOAOUTEN) & (~(1 << 23))), GPIOAOUTEN);
            reg_writel((act_readl(GPIOAINEN) & (~(1 << 23))), GPIOAINEN);
            reg_writel((act_readl(MFP_CTL0) & (~MFP_CTL0_GPIOA23_MASK)) | (6 << MFP_CTL0_GPIOA23_SHIFT), MFP_CTL0);
        }
    }

    g_app_info_state.stub_phy_type = com_get_config_default(SETTING_STUB_PHY_INTERFACE);
    if(g_app_info_state.stub_phy_type == STUB_PHY_UART)
    {
        /* ���� UART stub ����� */
        sys_load_mmm(stub_name, FALSE);
        /* �������״̬ */
        stub_get_connect_state(1);	
    }

    //ʹ���ڲ����ݣ���Ҫ����
    if (com_get_config_default(SETTING_HARDWARE_26MHz_CYRSTAL_CAPACITOR) == 0)
    {
        manager_config_hosc_freq();
    }

    return ret;
}
