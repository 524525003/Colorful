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
/*!
 * \file     music_channel_test.c
 * \brief    ͨ������ʵ��ģ��
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"
#include "ap_autotest_channel_test.h"
#include "fm_interface.h"

//16����
const uint16 pcm_table[]=
{
    0x6ba,  0x371f, 0x5f22, 0x78a6, 0x7fce, 0x7328, 0x55c9, 0x2ab2,
    0xf945, 0xc8df, 0xa0de, 0x8757, 0x802f, 0x8c7f, 0xaa63, 0xd550
};


//ʹ��1K�Ļ��� 16*16*2*2=1k
void init_dac_buffer(void)
{
    uint8 i, j;

    uint16 *p_dac_buffer = (uint16 *) DAC_BUFFER;

    for (j = 0; j < 16; j++)
    {
        for (i = 0; i < 16; i++)
        {
            //��������ʹ����ͬ������
            *p_dac_buffer = pcm_table[i];
            p_dac_buffer++;
            *p_dac_buffer = pcm_table[i];
            p_dac_buffer++;
        }
    }

    return;
}

void sco_dac_dma_config(void)
{
    act_writel(0, DMA0CTL);

    sys_udelay(10);

    //separated stored in the memory, 0 for interleaved stored, 1 for separated stored
    act_writel(act_readl(DMA0CTL) & (~(1 << DMA0CTL_AUDIOTYPE)), DMA0CTL);

    //data width 16bit
    act_writel(act_readl(DMA0CTL) | (1 << DMA0CTL_DATAWIDTH_SHIFT), DMA0CTL);

    //source addr type:memory
    act_writel(act_readl(DMA0CTL) | (0 << DMA0CTL_SRCTYPE_SHIFT), DMA0CTL);

    //dest addr type:dac fifo
    act_writel(act_readl(DMA0CTL) | (0x0b << DMA0CTL_DSTTYPE_SHIFT), DMA0CTL);

    //reload enable
    act_writel(act_readl(DMA0CTL) | (1 << DMA0CTL_reload), DMA0CTL);

    //source address
    act_writel(DAC_BUFFER, DMA0SADDR0);

    //dma length
    act_writel(1024 / 2 / 2, DMA0FrameLen);

#if 0
    //enable dma0 half & full trans interupt
    //act_writel(act_readl(DMAIE)|0x00000011, DMAIE);

    //DAC FIFO Input Select:from Special DMA :DMA to DAC
    act_writel(act_readl(DAC_FIFOCTL) & ~(DAC_FIFOCTL_DAF0IS_MASK), DAC_FIFOCTL);
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0IS_SHIFT), DAC_FIFOCTL);

    //enalbe dac fifo
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0RT), DAC_FIFOCTL);

#else

    //DAC FIFO Input Select:from Special DMA :DMA to DAC
    act_writel(act_readl(DAC_FIFOCTL) & ~(DAC_FIFOCTL_DAF0IS_MASK), DAC_FIFOCTL);
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0IS_SHIFT), DAC_FIFOCTL);

    //both left&right channel
    act_writel((act_readl(DAC_ANACTL) | (1 << DAC_ANACTL_DAENL)), DAC_ANACTL);
    act_writel((act_readl(DAC_ANACTL) | (1 << DAC_ANACTL_DAENR)), DAC_ANACTL);

    //enalbe DAC FIFO Empty DRQ
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0EDE), DAC_FIFOCTL);

    //enable transfer
    act_writel(act_readl(DMA0CTL) | (1 << DMA0CTL_DMA0START), DMA0CTL);

    sys_udelay(10);
#endif
}
/******************************************************************************/
/*!
 * \par  Description:
 * \����ADC�����Ĳ����Լ�DMA������
 * \param[in]    dst_addr �ɼ����ݴ�ŵĻ�������ַ
 * \param[in]    block_length �ɼ������ݳ���
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      music_module_test.c
 * \note
 */
/*******************************************************************************/
void init_dma_to_adc(uint32 dst_addr, uint32 block_length)
{
    uint32 status;

    uint8 loop_cnt = 0;

    //������������ɼ�����
    act_writel(act_readl(ADC_ANACTL) | (1 << ADC_ANACTL_ADLEN), ADC_ANACTL);

    //������������ɼ�����
    act_writel(act_readl(ADC_ANACTL) | (1 << ADC_ANACTL_ADREN), ADC_ANACTL);

    //abort DMA stransfer and reset config
    *((REG32)(DMA1CTL)) = 0x00;

    //data width 16bit
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DATAWIDTH_SHIFT), DMA1CTL);

    //source addr type:adc fifo
    act_writel(act_readl(DMA1CTL) | (0x0b << DMA1CTL_SRCTYPE_SHIFT), DMA1CTL);

    //dest addr type:memory
    act_writel(act_readl(DMA1CTL) | (0 << DMA1CTL_DSTTYPE_SHIFT), DMA1CTL);

#ifdef PRINT_CHANNEL_DATA
    //reload enable
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_reload), DMA1CTL);
#endif

    //dest address
    act_writel(dst_addr, DMA1DADDR0);

    //dma length
    act_writel(block_length / 2, DMA1FrameLen);

    //enable transfer
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DMA1START), DMA1CTL);

#ifdef PRINT_CHANNEL_DATA
    while(1)
    {
        int i;

        DEBUG_ATT_PRINT("\n**************\n", 0, 0);

        for(i = 0; i < 128; i += 4)
        {
            DEBUG_ATT_PRINT(0, *(uint16 *)(dst_addr + i), 1);
            DEBUG_ATT_PRINT(0, *(uint16 *)(dst_addr + i + 2), 1);
        }

        sys_mdelay(500);
    }
#endif

    while (1)
    {
        status = act_readl(DMA1CTL);

        if ((status & 0x01) == 0)
        {
            loop_cnt++;

            if (loop_cnt < 2)
            {
                act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DMA1START), DMA1CTL);
            }
            else
            {
                //abort DMA stransfer and reset config
                *((REG32)(DMA1CTL)) = 0x00;
                break;
            }

        }
    }

}

void play_pcm_init(uint8 ain_type)
{
    DEBUG_ATT_PRINT("start channel test", ain_type, 2);

    set_dac_rate(ADC_SAMPLE_RATE, 0);

    enable_dac(DAFIS_DMA, DAF0_EN);

    //��ʹ��ģ�����룬������ADC�����ʣ����ʹ��ADC
    if (ain_type == AI_SOURCE_LINEIN)
    {
        enable_ain(AI_SOURCE_LINEIN, 6);
    }
    else if (ain_type == AI_SOURCE_ASEMIC)
    {
        //��mic����ͨ��
        enable_ain(AI_SOURCE_ASEMIC, 6);
    }
    else
    {
        ;//FM�����ݲ�֧��
    }

    set_adc_rate(ADC_SAMPLE_RATE);

    enable_adc(ADFOS_DMA, 0);

    init_dac_buffer();

    sco_dac_dma_config();

    if (ain_type == AI_SOURCE_ASEMIC)
    {
        //���������ȼ�Ϊ5��������������������mic,linein�Ĳ���Ҫ��
        set_pa_volume(25, (0xb0));
    }
    else
    {
        set_pa_volume(25, (0xbf));
    }

    sys_os_time_dly(20);
}

void play_pcm_exit(void)
{
    //ֹͣ���ݷ���
    act_writel(0, DMA0CTL);

    act_writel(0, DMA1CTL);

    disable_dac(DAF0_EN);

    disable_adc();
}

static int32 caculate_power_value(int16 *dac_buffer, uint32 data_length)
{
    uint32 i;
    uint32 power_value = 0;
    uint32 power_sample_value = 0;

    //���㵥��������������
    for (i = 0; i < (data_length * 2); i += 2)
    {
        if (dac_buffer[i] >= 0)
        {
            power_sample_value = (uint32)(dac_buffer[i]);
        }
        else
        {
            power_sample_value = (uint32)(-dac_buffer[i]);
        }

        power_value += power_sample_value;
    }

    power_value = power_value / data_length;

    att_write_test_info("power value:", power_value, 1);

    if (power_value < POWER_LOW_THRESHOLD)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

int32 analyse_sound_data_sub(uint16 *dac_buffer, uint32 data_length, uint8 mode)
{
    int32 ret_val;

    ret_val = caculate_power_value(&dac_buffer[0], data_length / 4);

    if (ret_val == FALSE)
    {
        return ret_val;
    }

    //����linein��Ҫ������·���
    if (mode == 1)
    {
        ret_val = caculate_power_value(&dac_buffer[1], data_length / 4);

        if (ret_val == FALSE)
        {
            return ret_val;
        }
    }

    return ret_val;
}

int32 test_linein_channel(void)
{
    int32 i;
    int32 test_result;

    for (i = 0; i < MAX_SAMPLE_COUNT; i++)
    {
        //��ʼͨ��lineinͨ���ɼ�����
        init_dma_to_adc(LINEIN_SOUND_DATA_ADDR, SOUND_DATA_LEN);

        test_result = analyse_sound_data_sub(LINEIN_SOUND_DATA_ADDR, SOUND_DATA_LEN, 1);

        if (test_result == FALSE)
        {
            break;
        }
    }
#ifdef DEBUG_WRITE_CHANNEL_DATA
    write_temp_file(1, LINEIN_SOUND_DATA_ADDR, SOUND_DATA_LEN);
#endif
    //�ɼ��������ر�ͨ��
    disable_ain(AI_SOURCE_LINEIN);

    return test_result;
}

int32 test_mic_channel(void)
{
    int32 i;
    int32 test_result;

    for (i = 0; i < MAX_SAMPLE_COUNT; i++)
    {
        //��ʼͨ��micͨ���ɼ�����
        init_dma_to_adc(MIC_SOUND_DATA_ADDR, SOUND_DATA_LEN);

        test_result = analyse_sound_data_sub(MIC_SOUND_DATA_ADDR, SOUND_DATA_LEN, 1);

        if (test_result == FALSE)
        {
            break;
        }
    }
#ifdef DEBUG_WRITE_CHANNEL_DATA
    write_temp_file(0, MIC_SOUND_DATA_ADDR, SOUND_DATA_LEN);
#endif
    //�ɼ��������ر�ͨ��
    disable_ain(AI_SOURCE_ASEMIC);

    //ֹͣ���ݷ���
    act_writel(0, DMA0CTL);

    return test_result;
}

bool test_fm_exit(void)
{
    int ret;

    ret = fm_close();

    disable_ain(AI_SOURCE_FM);

    disable_adc();

    sys_drv_uninstall(DRV_GROUP_FM);

    if (ret != 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

bool test_fm_init(void)
{
    int32 ret;
    uint8 band_info, level;
    uint32 freq = 10800;
    ret = sys_drv_install(DRV_GROUP_FM, 0, "drv_fm.drv");
    if (ret != 0)
    {
        goto test_fm_fail0;
    }
    band_info = 0; //Ĭ��Ƶ��
    level = 8; //Ĭ����̨����

    ret = fm_open(band_info, level, 0);
    if (ret != 0)
    {
        goto test_fm_fail0;
    }
    if ((freq < 87500) || (freq > 108000))
    {
        freq = 87500;
    }
    ret = fm_set_freq(freq);
    test_fm_fail0: if (ret != 0)
    {
        test_fm_exit();
        return FALSE;
    }
    else
    {
        fm_mute(0);
        return TRUE;
    }
}

void init_fm_channel(void)
{
    ///PMP mode AA MUTE
    *((REG32)(DAC_ANACTL)) &= (~(1 << DAC_ANACTL_AIN2DMT));

    //enable ADC Module
    *((REG32)(MRCR)) |= (1 << MRCR_AudioIOReset);

    //set ADC clock
    *((REG32)(CMU_DEVCLKEN)) |= 1 << CMU_DEVCLKEN_ADCCLKEN;

    //Analog In enable L & R channel
    *((REG32)(ADC_ANACTL)) |= 1 << ADC_ANACTL_ADLEN;
    *((REG32)(ADC_ANACTL)) |= 1 << ADC_ANACTL_ADREN;

    //set source input gain
    set_ain_gain(AI_SOURCE_FM, 5);

    //delay
    sys_mdelay(150);

    //PMP mode AA MUTE disable
    *((REG32)(DAC_ANACTL)) |= 1 << DAC_ANACTL_AIN2DMT;

}

int32 test_fm_channel(void)
{
    int32 test_result;

    test_result = test_fm_init();

    if (test_result == 0)
    {
        test_result = TEST_FM_FAIL;
    }

    //��FM�ɼ����ݵĲ���ͨ��
    init_fm_channel();

    //��ʼͨ��FMͨ���ɼ�����
    init_dma_to_adc(FM_SOUND_DATA_ADDR, SOUND_DATA_LEN);

    test_result = analyse_sound_data_sub(FM_SOUND_DATA_ADDR, SOUND_DATA_LEN, 1);

    //�ɼ��������ر�ͨ��
    //disable_ain(AI_SOURCE_FM);

    //ֹͣ���ݷ���
    act_writel(0, DMA0CTL);

    test_fm_exit();

    //ret:
    return test_result;
}

void act_test_report_channel_result(uint16 test_id, int32 ret_val)
{
    return_result_t *return_data;
    uint16 trans_bytes = 0;

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = test_id;

        return_data->test_result = ret_val;

        //��ӽ�����
        //return_data->return_arg[trans_bytes++] = 0x0000;

        act_test_report_result(return_data, 4);
    }
    else
    {
        if (ret_val == FALSE)
        {
            led_flash_fail();
        }
    }
}

test_result_e act_test_linein_channel_test(void *arg_buffer)
{
    int32 result;

    //��ʼ��ADC,DAC,��ʼ�����DMA
    play_pcm_init(AI_SOURCE_LINEIN);

    result = test_linein_channel();

    play_pcm_exit();

    if (result == 0)
    {
        att_write_test_info("linein channel test failed", 0, 0);
    }
    else
    {
        att_write_test_info("linein channel test ok", 0, 0);
    }

    act_test_report_channel_result(TESTID_LINEIN_CH_TEST, result);

    return result;
}

test_result_e act_test_mic_channel_test(void *arg_buffer)
{
    int32 result;

    //��ʼ��ADC,DAC,��ʼ�����DMA
    play_pcm_init(AI_SOURCE_ASEMIC);

    result = test_mic_channel();

    play_pcm_exit();

    if (result == 0)
    {
        att_write_test_info("mic channel test failed", 0, 0);
    }
    else
    {
        att_write_test_info("mic channel test ok", 0, 0);
    }

    act_test_report_channel_result(TESTID_MIC_CH_TEST, result);

    return result;
}

test_result_e act_test_fm_channel_test(void *arg_buffer)
{
    int32 result;

    //��ʼ��ADC,DAC,��ʼ�����DMA
    play_pcm_init(AI_SOURCE_FM);

    result = test_fm_channel();

    play_pcm_exit();

    if (result == 0)
    {
        ;
    }
    else
    {
        ;
    }

    act_test_report_channel_result(TESTID_FM_CH_TEST, result);

    return result;
}

#ifdef DEBUG_WRITE_CHANNEL_DATA
static const uint8 att_mic_file[] = "MIC.PCM";
static const uint8 att_linein_file[] = "LINEIN.PCM";

static uint8 tmp_buffer[30] _BANK_DATA_ATTR_;

/*! \cond UI_DRIVER_API */
/******************************************************************************/
/*!
 * \par  Description:
 *    �����ַ���תunicode�ַ���
 * \param[in]   src��Դ�ַ���ָ��
 * \param[in]   len��Ҫת���������ַ�������
 * \param[out]  dest��Ŀ���ַ���ָ��
 * \return      �Ƿ�ת���ɹ�
 * \retval      TRUE ת���ɹ�
 * \retval      FALSE ת��ʧ��
 * \ingroup     string
 * \note        Ŀ���ַ������������û���֤�����������������С��Ҫ���Ͻ�������
 *******************************************************************************/
bool char_to_unicode(uint8 *dest, uint8 *src, uint16 len)
{
    int s_cnt = 0;
    int d_cnt = 0;
    uint16 font_code;

    while (1)
    {
        if ((s_cnt >= len) || (src[s_cnt] == 0))//��β���߽�����

        {
            break;
        }

        if ((uint8) src[s_cnt] >= 0x80)
        {
            font_code = 0x3f;
        }
        else
        {
            font_code = src[s_cnt];
        }
        s_cnt++;

        dest[d_cnt] = *((uint8*) &font_code); //���ֽ�
        d_cnt++;
        dest[d_cnt] = *((uint8*) &font_code + 1); //���ֽ�
        d_cnt++;
    }
    dest[d_cnt] = 0;
    d_cnt++;
    dest[d_cnt] = 0;
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ����������ļ���ת��ΪUnicode�����ļ�����
 * \param[in]    file_name ��������ļ���
 * \param[out]   file_name ���Unicode�����ļ���
 * \return       none
 * \ingroup      common_func
 * \par          exmaple code
 * \code
 ����1������������ļ���ת��ΪUnicode�����ļ���
 const char const_rec_name[] = "rec_001.wav";
 uint8 new_rec_name[26];

 libc_memcpy(new_rec_name, const_rec_name, 12);
 com_ansi_to_unicode(new_rec_name);
 //���ţ��Ϳ���ʹ�� new_rec_name ����������Ϊ rec_001.wav ���ļ���
 * \endcode
 * \note
 * \li  ���ڴ����ļ�ǰ���ļ���ת��ΪUnicode���룬����Ӧ exFat �ļ�ϵͳ��
 * \li  �û����Լ���֤ file_name �����Unicode�����ļ��������� Ӣ�������ļ���
 *      rec_001.wav��ת��Unicode�����ļ�������Ҫ��������СΪ 26 ���ֽڣ���
 *      Unicode�����־�ַ�0xfffe 2�ֽ� + 11���ַ� 2 * 11�ֽ� + ������0x0 0x0 2�ֽڡ�
 * \li  �����ڸ�����������С��52�ֽڣ����ļ������ܳ��� 24 ���ַ����ȡ�
 *******************************************************************************/
void com_ansi_to_unicode(uint8 *file_name)
{
    uint16 ansi_len = (uint16) libc_strlen(file_name);
    uint16 i;

    //�����ƶ�2�ֽڣ�ǰ����� 0xfffe
    for (i = ansi_len; i > 0; i--)
    {
        file_name[(i - 1) + 2] = file_name[i - 1];
    }
    file_name[0] = 0xff;
    file_name[1] = 0xfe;
    file_name[ansi_len + 2] = 0x0;

    //���ļ���ת��ΪUnicode���� com_name_buffer
    char_to_unicode(tmp_buffer, file_name + 2, ansi_len);

    //����Unicode����
    libc_memcpy(file_name + 2, tmp_buffer, ansi_len * 2 + 2);
}

void write_temp_file(uint8 file_index, uint8 *write_buffer, uint32 write_len)
{
    int file_handle;

    int ret_val;

    uint8 filename[64];

    libc_memset(filename, 0, 64);

    if(file_index == 0)
    {
        libc_memcpy(filename, att_mic_file, sizeof(att_mic_file));
    }
    else
    {
        libc_memcpy(filename, att_linein_file, sizeof(att_linein_file));
    }

    com_ansi_to_unicode(filename);

    //��λ����Ŀ¼
    vfs_cd(g_file_sys_id, CD_ROOT, 0);

    ret_val = vfs_file_dir_exist(g_file_sys_id, filename, 1);

    if(ret_val == 0)
    {
        file_handle = vfs_file_create(g_file_sys_id, filename, write_len);

        if(file_handle == 0)
        {
            libc_print("create recrod file error!\n", 0, 0);

            while(1);
        }

        vfs_file_write(g_file_sys_id, write_buffer, write_len, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }
    else
    {
        file_handle = vfs_file_open(g_file_sys_id, filename, OPEN_MODIFY);

        if(file_handle == 0)
        {
            libc_print("open file error!\n", 0, 0);

            while(1);
        }

        vfs_file_write(g_file_sys_id, write_buffer, write_len, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }

    return;

}
#endif

