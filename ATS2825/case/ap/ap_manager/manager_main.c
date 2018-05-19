/*******************************************************************************
 *                              US212A
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_main.c
 * \brief    manager��main����ģ��
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "manager.h"
#include  <card.h>
#include  <sysdef.h>

//#define FT_MODE
OS_STK *ptos = (OS_STK *) AP_PROCESS_MANAGER_STK_POS;
INT8U prio = AP_PROCESS_MANAGER_PRIO;
test_share_info_t *g_p_test_share_info;

extern app_result_e manager_message_loop(void)__FAR__;
extern bool globe_data_init(void) __FAR__;

void system_config(void);

typedef void (*handler_ker)(void);

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
static void _read_var(void)
{

}

#ifdef FT_MODE
void delay_ms(uint8 dly_ms)
{
    uint8 i, j;
    while (dly_ms != 0)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 255; j++)
            {
                ; //do nothing
            }
        }
        dly_ms--;
    }
}
#endif


/******************************************************************************/
/*!
 * \par  Description:
 *    Ӧ�õ���ں���
 * \param[in]    δʹ��
 * \param[out]   none
 * \return       ��Զ�����˳�
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
    system_config();

    //��װNOR UD����
    sys_drv_install(DRV_GROUP_STG_BASE, 0, "nor_ud.drv");

    //��ʼ��globeȫ�����ݣ����������ýű� config.bin
    globe_data_init_applib();
    
	ft_test_mode();

#ifdef FT_MODE
        //act_writel((act_readl(HOSC_CTL) & 0xffff0000), HOSC_CTL);
        delay_ms(20);
        //act_writel((act_readl(HOSC_CTL) | 0x4646), HOSC_CTL);
        delay_ms(10);
        act_writel(act_readl(0xc01b0000) | 0x00000014, 0xc01b0000); //ft test mode
        delay_ms(20);  
        act_writel((act_readl(WD_CTL) & (~0x10)), WD_CTL);
        libc_print("FT MODE",0,0);
        while(1)
        {
          ; 
        }
#else 
  
    PRINT_INFO_INT("WAKE_TYPE:", argc);

    //config_hosc_freq();
    //config pcmram1 to cpu
    //reg_writel((act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_PCMRAM1CLKSEL_MASK)), CMU_MEMCLKSEL);
    //config asrc buffer 
    reg_writel((act_readl(CMU_MEMCLKEN) | (1<<CMU_MEMCLKEN_ASRCBUF1CLKEN)), CMU_MEMCLKEN);
    reg_writel((act_readl(CMU_MEMCLKSEL) & (~(1<<CMU_MEMCLKSEL_ASRCBUF1CLKSEL))), CMU_MEMCLKSEL);
   
    install_key_drv();

    adjust_freq_set_level(AP_PROCESS_MANAGER_PRIO, FREQ_LEVEL8, FREQ_NULL);//�ӿ쿪���ٶ�

#ifdef WAVES_ASET_TOOLS
    //���ʹ��WAVES��˾�㷨��DSPƵ������Ϊ180Mhz���ݲ����ǹ���

    //����ϵͳƵ��
    sys_adjust_clk(85 | (180 << 8), 0);
        
    //����Ƶ��Ϊ��MIPS -> 85M, DSP -> 180
    sys_lock_adjust_freq(85 | (180 << 8));
    
#endif

    pa_thread_open(NULL);

    //��ʼ��applib�⣬ǰ̨AP
    applib_init(APP_ID_MANAGER, APP_TYPE_PMNG);

    //��ʼ�� applib ��Ϣģ��
    sys_mq_flush(MQ_ID_MNG);

    //��ʼ��Ӧ�ñ���
    _read_var();

    //����configӦ��
    if(argc)
    {
        sys_exece_ap("config.ap", 0, 0);    //�������������
    }
    else
    {
        sys_exece_ap("config.ap", 0, 5);    //�ϵ翪��
    }

    manager_message_loop();

    return 0;
#endif	
}

#if 0
/******************************************************************************/
/*!
 * \par  Description:
 *  ����NOR��������
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
uint32 nor_set_drv_power(uint8 level)
{
    uint32 tmp_pad_drv;

    switch(level)
    {
        case 1:
        tmp_pad_drv = 0x00;
        break;

        case 2:
        tmp_pad_drv = 0x55;
        break;

        case 3:
        tmp_pad_drv = 0xaa;
        break;

        case 4:
        tmp_pad_drv = 0xff;
        break;

        //the default value
        default:
        tmp_pad_drv = 0x55;
        break;
    }

    act_writel(act_readl(PAD_DRV2) & (~0xff), PAD_DRV2);
    act_writel(act_readl(PAD_DRV2) | tmp_pad_drv, PAD_DRV2);
    //act_setl(tmp_pad_drv, PAD_DRV2);
    return 1;
}
#endif
/******************************************************************************/
/*!
 * \par  Description:
 *    system config
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
void system_config(void)
{
    /*for card configuration.  xiaomaky.  2013/2/19 13:49:32.*/
    /*
     bit0: 0--1_line, 1--4_line;
     bit1(sdvccout power): 0--direct, 1--GPIO;
     bit2(DAT3 pull high): 0--hardware pull, 1--GPIO pull;
     bit3(write_fast):0--not continuous, 1--continuous write;
     */


//    handler_ker detect_card_isr_ker = (handler_ker) GLOBAL_DETECT_CARD_ISR_ADDR;//ָ��kernel�̻��ӿ� detect_card_isr

    /* bit1----if is 0(default), if exception, will reset; if 1, not reset.  */
    /* bit2----if is 0(default), card-gpio-detec; if 1, card-cmd-detect for sys_detect_disk().
            must not to request timer irq to detect card.  */
    /* bit3----only for US280A, if is 0(default), disable isr_card-cmd-detec; if 1, enable isr_card-cmd-detec.*/
    // GLOBAL_KERNEL_BACKDOOR_FLAG |= 0x06;

    // GLOBAL_KERNEL_DETECT_CARD_IN_HANDER = GLOBAL_KSD_DETECT_CARD_ISR_ADDR;

    g_card_para = 0x00;

    g_card_type = 0;

    /* for GPIOA19 card-detect.  need to enable PU as soon for be stable.   */
    //act_writel((~(0x01 << 19)) & act_readl(GPIO_AOUTEN), GPIO_AOUTEN);  /* disable output.  */
    //act_writel((0x01 << 19) | act_readl(GPIO_AINEN), GPIO_AINEN);       /* enable input.  */
    //act_writel((~(0x01 << 19)) & act_readl(GPIO_APDEN), GPIO_APDEN);    /* disable 50K PD.  */
    //act_writel((0x01 << 19) | act_readl(GPIO_AOUTEN), GPIO_APUEN);      /* 50K PU Enable, ����û�п�ʱĬ��Ϊ��.  */

    // sys_set_irq_timer1(detect_card_isr_ker, 10);

    //���жϼ��ʱ����̫����config��Ҫ���˿�������Ϣ��������һ�ο���⺯��
    //  detect_card_isr_ker();

    //����rtc�жϼ�⿨����
    //GLOBAL_KERNEL_BACKDOOR_FLAG |= 0x08;

    //Ĭ�����ü��usb���Ƿ����
    //sys_set_usb_detect_mode(1);
    DISABLE_UHOST_DETECT();
    ENABLE_USB_DETECT();

    /* for uart-gpio, 0x9fc19a88--uart_tx_gpio_cfg_mask,  0x9fc19a8c--uart_tx_gpio_cfg_value.  */
    //GLOBAL_UART_TX_GPIO_CFG_MASK = 0xFFFF1FFF;//GPIO A21
    //GLOBAL_UART_TX_GPIO_CFG_VALUE = 0x00006000; //GPIO A21
    //GLOBAL_UART_TX_GPIO_CFG_MASK = 0xFFFFFFC7;//GPIO A17
    //GLOBAL_UART_TX_GPIO_CFG_VALUE = 0x00000020; //GPIO A17

    /*ϵͳ��Ƶʱ�����ٵ���VCC��ѹ��ϵͳ��ʼ��Ϊ3.1V��Ӧ�ÿ��Ը�������
     ������������ʵ�λ�����磬Ϊ��EJTAG���ԣ���Ϊ3.3V.*/
	//���ݹ����������󣬽�VDD��ѹ������3.2V
    act_writel((0xFFFFFF8F & act_readl(VOUT_CTL)) | 0x50, VOUT_CTL);

    /*for EMI setting.  */
    //    act_writel(0x5c92 | (0xffff0000 & act_readl(MCUPLL_DEBUG)), MCUPLL_DEBUG);   /* 0x5c12\0x5c92\0x5d12\0x5d92.*/
    //    /*disable MCUPLL-EMI setting.  */
    //    act_writel(act_readl(MCUPLL_DEBUG)&0xffffbfff, MCUPLL_DEBUG);
    //    /* bit0----if is 1, for pass EMI, not change nand_pad_drv according to frequency;   */
    //    GLOBAL_KERNEL_BACKDOOR_FLAG = GLOBAL_KERNEL_BACKDOOR_FLAG | 0x01;
    //    /* bit4----if is 1, for pass EMI, Nand/Card clock is fixed HOSC, not according to MCUPLL;    */
    //    GLOBAL_KERNEL_BACKDOOR_FLAG = GLOBAL_KERNEL_BACKDOOR_FLAG | 0x10;
    //    /* bit5----if is 1, for pass EMI, UDisk AP set clock and USB LIB not;  */
    //    GLOBAL_KERNEL_BACKDOOR_FLAG = GLOBAL_KERNEL_BACKDOOR_FLAG | 0x20;
    //    ����NOR��������
    //    nor_set_drv_power(3);
}

