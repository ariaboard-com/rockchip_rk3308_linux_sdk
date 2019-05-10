/************************************************************************************
文件名称：
文件功能：
函数列表：
修改日期：
*************************************************************************************/
#ifndef RK_AUDIOINTERFACE_H
#define RK_AUDIOINTERFACE_H

/*--------------------------------------- include -----------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif


/*--------------------------------------- 宏定义 -----------------------------------------*/
#define PARALEN          (614)  // 参数个数



/*--------------------------------------- 枚举定义 ----------------------------------------*/




/*--------------------------------------- 结构体定义 --------------------------------------*/





/*-------------------------------------- 全局变量声明 --------------------------------------*/






/*----------------------------------------- 函数声明 ---------------------------------------*/
extern void AudioPost_Init(char       *pcParaName, 
	                       signed int swFrmLen);
extern void AudioPost_Destroy();
extern void AudioPost_Process(float            *pfIn, 
	                          float            *pfOut, 
							  signed short int shwChannelNum, 
							  signed int       swFrmLen);
extern void AudioPost_SetPara(float      *pfPara, 
							  signed int swFrmLen);
extern void AudioPost_GetPara(float *pfPara);

#ifdef __cplusplus
} 
#endif

#endif
