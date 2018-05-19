/*******************************************************************************
 *                              US212A
 *                            Module: Manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_msg_handle.c
 * \brief    ��Ϣ����ģ��
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "manager.h"

extern void manager_msg_callback_sub(private_msg_t *pri_msg);

/******************************************************************************/
/*!
 * \par  Description:
 *    ��Ϣ����
 * \param[in]    ˽����Ϣָ��
 * \param[out]   none
 * \return       none
 * \retval
 * \retval
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
void manager_msg_callback(private_msg_t *pri_msg)
{
    uint8 ap_name[12];
    uint8 app_id;
    uint8 ap_param;

    switch (pri_msg->msg.type)
    {
        //����ǰ̨Ӧ��
        case MSG_CREAT_APP:
        {
            adjust_freq_set_level(AP_PROCESS_MANAGER_PRIO,FREQ_LEVEL7,FREQ_NULL);
            
            app_id = pri_msg->msg.content.data[0];
            ap_param = pri_msg->msg.content.data[1];
            
            wait_ap_exit();
    
            _get_app_name(ap_name, app_id, APP_TYPE_GUI);
            
            sys_free_ap(FALSE);
            sys_exece_ap(ap_name, 0, (int32) ap_param);
            
            adjust_freq_set_level(AP_PROCESS_MANAGER_PRIO,FREQ_LEVEL3,FREQ_NULL);
        }
        break;

        //������̨Ӧ��
        case MSG_CREAT_APP_SYNC:
        {
            app_id = pri_msg->msg.content.data[0];
            ap_param = pri_msg->msg.content.data[1];
            
            //Ӧ��Լ�������һ����̨���ڣ���Ҫ�Ȱѵ�ǰ��̨ɱ����Ȼ���ٴ�����Ҫ�ĺ�̨
            if (g_app_info_vector[APP_TYPE_CONSOLE].used == 0)
            {
                _get_app_name(ap_name, app_id, APP_TYPE_CONSOLE);
                
                //wait for cur app exit
                libc_waitpid(0, 1);
    
                sys_free_ap(1);
                if (sys_exece_ap(ap_name, 1, (int32) ap_param) >= 0)
                {
                    pri_msg->reply->type = MSG_REPLY_SUCCESS;
                }
                else
                {
                    pri_msg->reply->type = MSG_REPLY_FAILED;
                }
                libc_sem_post(pri_msg->sem);
            }
            else
            //�����̨���ڲŴ���
            {
                pri_msg->reply->type = MSG_REPLY_SUCCESS;
                libc_sem_post(pri_msg->sem);
            }
        }
        break;

        case MSG_KILL_APP_SYNC:
        //�����̨���ڲŴ���
        {
            app_id = pri_msg->msg.content.data[0];
            ap_param = pri_msg->msg.content.data[1];
            if (app_id == APP_ID_THEENGINE)
            {
                app_id = get_engine_appid_by_type(get_engine_type());
            }
            
            if (app_id == APP_ID_MAX)
            {
                goto kill_over;
            }
            
            if (((app_id == APP_ID_BTSTACK) && (g_app_info_vector[APP_TYPE_BTSTACK].used != 0))
             || ((app_id != APP_ID_BTSTACK) && (g_app_info_vector[APP_TYPE_CONSOLE].used != 0)))
            {
                //ɱ����̨
                kill_app(app_id, ap_param);
                if (app_id == APP_ID_BTSTACK)
                {
                    sys_free_ap(AP_BTSTACK_FILE);
                }
                else
                {
                    sys_free_ap(1);
                }
            }
            
            //���Ӧ���Ѿ������ڣ�ֱ�ӷ��سɹ�������ǰ̨�ղ�����Ӧ������
            kill_over:
            pri_msg->reply->type = MSG_REPLY_SUCCESS;
            libc_sem_post(pri_msg->sem);
        }
        break;

        default:
        manager_msg_callback_sub(pri_msg);
        break;
    }

}
