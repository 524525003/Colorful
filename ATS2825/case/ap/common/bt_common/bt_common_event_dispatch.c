/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙管理模块事件分发处理。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_btmanager.h"
#include "common_rcp.h"
#include "common_func.h"

extern app_result_e com_btmanager_event_dispatch_bank(btstack_event_t *p_event, bool tts_flag) __FAR__;

app_result_e com_btmanager_event_check_for_tts(btstack_event_t *p_event)
{
    msg_apps_type_e msg_type;
    bool force_tts_stop_flag = FALSE;

    msg_type = p_event->msg.type;
    switch (msg_type)
    {
        case MSG_BTSTACK_AVRCP_RX_OPID:
        case MSG_BTSTACK_AVRCP_VOLUME_ABS:
        case MSG_BTSTACK_HFP_VOLUME_ABS:
        case MSG_BTSTACK_ERR_CONNECTION_TIMEOUT:
        case MSG_BTSTACK_PATCH_DOWNLOAD_FLAG:
        force_tts_stop_flag = FALSE;
        break;

        case MSG_BTSTACK_SPPBLE_RCP_PKG:
        {
            rmt_ctrl_pkg_t tmp_rcp;

            tmp_rcp.rcp_data = p_event->msg.content.addr;
            tmp_rcp.rcp_len = ((uint16) (p_event->reserve[1]) << 8) | p_event->reserve[0];
            //libc_memcpy(&tmp_rcp,p_event->msg.content.addr,sizeof(rmt_ctrl_pkg_t));
            tmp_rcp.rcp_flag = (uint8)p_event->bd_addr[0];
            
            tmp_rcp.rcp_remote = (uint8)p_event->bd_addr[1];
            
            g_rcp_service_type = tmp_rcp.rcp_flag;
            
            g_rcp_remote_number = tmp_rcp.rcp_remote;

            force_tts_stop_flag = com_rcp_check_for_tts(&tmp_rcp);
        }
        break;

        default:
        break;
    }

    if (force_tts_stop_flag == TRUE)
    {
        return RESULT_FORCE_TTS_STOP;
    }
    else
    {
        return RESULT_NULL;
    }
}

app_result_e __section__(".rcode") com_btmanager_event_dispatch(btstack_event_t *p_event, bool tts_flag)
{
    app_result_e result = RESULT_NULL;

    if (p_event->msg.type == MSG_BTSTACK_SPPBLE_RCP_PKG)
    {
        rmt_ctrl_pkg_t tmp_rcp;

        tmp_rcp.rcp_data = p_event->msg.content.addr;
        tmp_rcp.rcp_len = ((uint16) (p_event->reserve[1]) << 8) | p_event->reserve[0];
        //libc_memcpy(&tmp_rcp,p_event->msg.content.addr,sizeof(rmt_ctrl_pkg_t));
        tmp_rcp.rcp_flag = (uint8)p_event->bd_addr[0];
            
        tmp_rcp.rcp_remote = (uint8)p_event->bd_addr[1];   
        
             
        
        g_rcp_service_type = tmp_rcp.rcp_flag;
        
        g_rcp_remote_number = tmp_rcp.rcp_remote;

        result = com_rcp_dispatch(&tmp_rcp);
    }
    else
    {
        result = com_btmanager_event_dispatch_bank(p_event, tts_flag);
    }

    return result;
}

app_result_e com_btmanager_event_dispatch_bank(btstack_event_t *p_event, bool tts_flag)
{
    msg_apps_type_e msg_type;
    app_result_e result = RESULT_NULL;
    uint8 i;

    msg_type = p_event->msg.type;
    switch (msg_type)
    {
        case MSG_BTSTACK_AVRCP_RX_OPID:
        {
            result = com_btmanager_avrcp_ind(p_event->msg.content.data[0], tts_flag);
        }
        break;

        case MSG_BTSTACK_AVRCP_VOLUME_ABS:
        {
            result = com_btmanager_avrcp_set_volume_to_soundbox(p_event->msg.content.data[0], tts_flag);
        }
        break;

        case MSG_BTSTACK_HFP_VOLUME_ABS:
        {
            result = com_btmanager_hfp_set_volume_to_soundbox(p_event->msg.content.data[0], tts_flag);
        }
        break;

        case MSG_BTSTACK_ERR_CONNECTION_TIMEOUT:
        {
            g_bt_auto_connect_ctrl.need_auto_connect = 0x01;
            g_bt_auto_connect_ctrl.auto_connect_type = AUTO_CONNECT_TYPE_TIMEOUT;
            g_need_reset_controller_timeout = 1;
            g_neednot_tts_play_timeout = 1;
            for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
            {
                if (g_bt_auto_connect_ctrl.dev_info[i].conn_flag == 0)
                {
                    g_bt_auto_connect_ctrl.dev_info[i].conn_flag = 1;
                    libc_memcpy(g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes,
                            p_event->bd_addr, 6);
                    g_bt_auto_connect_ctrl.dev_info[i].support_profile = p_event->msg.content.data[0];
                    PRINT_INFO_INT("recv profile:", p_event->msg.content.data[0]);
                    break;
                }
            }
        }
        break;

        case MSG_BTSTACK_PATCH_DOWNLOAD_FLAG:
        {
            g_app_info_state.patch_download_flag = p_event->msg.content.data[0];
            PRINT_INFO_INT("REC_FLAG:",g_app_info_state.patch_download_flag);
        }
        break;

        case MSG_BTSTACK_ERR_HARDWARE_EXCEPTION:
        {
            #if 0
            act_writel((0 << WD_CTL_CLKSEL_SHIFT) | (1 << WD_CTL_WDEN) | (1 << WD_CTL_CLR), WD_CTL);//176ms
            while (1)
            {
                ;//等待重启
            }
            #endif

            //转发给COMMON，由COMMON重新装载BT STACK，且需要恢复回连
            msg_apps_t msg;

            if (g_this_app_info->app_id == APP_ID_BTPLAY)
            {
                msg.type = MSG_BTSTACK_ERR_HARDWARE_EXCEPTION;
            }
            else
            {
                msg.type = MSG_BTSTACK_ERR_HARDWARE;
            }        
            
            broadcast_msg(&msg, TRUE);
        }
        break;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        case MSG_BTSTACK_POWER_OFF:
        {
            //广播关机消息
            msg_apps_t msg;
            msg.type = MSG_POWER_OFF;
            broadcast_msg(&msg, TRUE);
        }
        break;
#endif

        default:
        break;
    }

    return result;
}
