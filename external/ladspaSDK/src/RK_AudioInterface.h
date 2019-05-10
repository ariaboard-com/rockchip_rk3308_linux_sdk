/************************************************************************************
�ļ����ƣ�
�ļ����ܣ�
�����б�
�޸����ڣ�
*************************************************************************************/
#ifndef RK_AUDIOINTERFACE_H
#define RK_AUDIOINTERFACE_H

/*--------------------------------------- include -----------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif


/*--------------------------------------- �궨�� -----------------------------------------*/
#define PARALEN          (614)  // ��������



/*--------------------------------------- ö�ٶ��� ----------------------------------------*/




/*--------------------------------------- �ṹ�嶨�� --------------------------------------*/





/*-------------------------------------- ȫ�ֱ������� --------------------------------------*/






/*----------------------------------------- �������� ---------------------------------------*/
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
