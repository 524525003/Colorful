#ifndef __WAVES_DAE_DEF__
#define __WAVES_DAE_DEF__
#include <psp_includes.h>

/*********WAVES PC���ߺ�С������ʱ����Ҫ����ʱ�������ݿռ�*********/
//0x9fc3afe0-9fc3afff(32�ֽ�)��ַ���ϵ����
#define WAVES_ASET_COEFF_ADDR                     (0x9fc3afe0)                        
//waves��˾�㷨����Ҫռ��DSP4k�Ŀռ� 0x9fc3b000-0x9fc3bfff
#define WAVES_ASET_SHARE_MEM_START_ADDR           (0x9fc3b000)
#define WAVES_ASET_SHARE_MEM_END_ADDR             (0x9fc3bfff)
/******************************************************************/

/*********waves�㷨���غ��������ݷ��ʵ�RAM�ռ�*********************/
//��ַ:0x9fc388b8-0x9fc3a416(0x2c45c-0x2d20b)


/*****С����WAVES PC���߽�����ʱ�����������մ�ŵ���ʼ��ַ��*****/
/*****0x9fc388b8������С��512�ֽڣ�Ϊ���⹤�ߵ���ʱ,�л�������*****/
/*****Ч����ʧЧ����0x9fc388b8��ʼ��ַ���512�ֽ�Ӧ������**********/
#define WAVES_PC_TOOLS_DATA_START_ADDR            (0x9fc388b8)
#define WAVES_PC_TOOLS_DATA_END_ADDR              (0x9fc3a416) 

#define WAVES_PC_TOOLS_DATA_LENGTH                (512) 

#define VM_WAVES_AL_DATA1                         (0x00310000)
#define VM_WAVES_AL_DATA2                         (0x00320000) 
#define VM_WAVES_AL_DATA3                         (0x00330000) 
#define VM_WAVES_AL_DATA4                         (0x00340000) 
#define VM_WAVES_AL_DATA5                         (0x00350000) 
#define VM_WAVES_AL_DATA6                         (0x00360000) 
#define VM_WAVES_AL_DATA7                         (0x00370000) 
#define VM_WAVES_AL_DATA8                         (0x00380000) 
#define VM_WAVES_AL_DATA9                         (0x00390000) 
#define VM_WAVES_AL_DATA10                        (0x003a0000) 
#define VM_WAVES_AL_DATA11                        (0x003b0000) 
#define VM_WAVES_AL_DATA12                        (0x003c0000) 
#define VM_WAVES_AL_DATA13                        (0x003d0000) 
#define VM_WAVES_AL_DATA14                        (0x003e0000) 


typedef struct
{
    uint8  data[WAVES_PC_TOOLS_DATA_LENGTH];
}waves_dae_para_t;
#endif


typedef struct
{
    uint32  update_flag;            //1��ʾPC���߸�����ϵ������Ҫ���ݵ�DSP LIB
    uint32  length;                 //�˴�Ҫ���µ�ϵ����ĳ���
    uint32  memory_addr;            //��Ҫ�����ڴ�ĵ�ַ       
}coeff_property_t;

typedef struct
{
    int DAE_change_flag;
    int parameter_address;
    int parameter_length;   
    int reserve[5];       
}coeff_t;

typedef struct
{    
    coeff_t  coeff_table;                 
    uint8    data[248]; 
}waves_para_vram_t;


typedef enum
{
    /*!����ϵ����*/
    SET_WAVES_COEFF_PROPERTY,
    /*!������Ч���ò���*/
    SET_WAVES_EFFECT_PARAM,
    /*!��Ч�������ڴ�д��VRAM*/
    WAVES_DAE_WRITE_VRAM,
    /*!��Ч�������ڴ�д��VRAM*/
    WAVES_DAE_READ_VRAM,
} set_waves_effect_type_e;



