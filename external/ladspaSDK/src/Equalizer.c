/* equalizer.c


*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/inotify.h>
#include "RK_AudioInterface.h"

/*****************************************************************************/

#include "ladspa.h"

/*****************************************************************************/

/* The port numbers for the plugin: */

#define EQ_DRC_CONTROL 0
#define EQ_DRC_INPUT1  1
#define EQ_DRC_OUTPUT1 2
#define EQ_DRC_INPUT2  3
#define EQ_DRC_OUTPUT2 4

/*****************************************************************************/

/************ EQ_DRC param debug**********/

#define EQ_DRC_PARAM_DEBUG_
//#define DEBUG_

/*****************************************************************************/

/* The structure used to hold port connection information and state */

typedef struct {

  /* Ports:
     ------ */

 // unsigned long m_fBufferSize;
 //
 unsigned int m_eqfirstRun;
 unsigned int m_eqsampleRate;

/*************connect port*****************/
  LADSPA_Data * m_pfControlValue;
  LADSPA_Data * m_leftInputBuffer;
  LADSPA_Data * m_leftOutputBuffer;
  LADSPA_Data * m_rightInputBuffer;  /* (Not used for mono) */
  LADSPA_Data * m_rightOutputBuffer; /* (Not used for mono) */
  /***************param debug ***********************/
#ifdef EQ_DRC_PARAM_DEBUG_
  //EQ_DRC_PARAM_DEBUG_
   const char *filename ;
   int fd ;
    int wd;
#endif

#ifdef DEBUG_
FILE *fp_in;
FILE *fp_out;
#endif
} Equalizer;

/***************itoa********************/
static char* rk_itoa(int val, char* dst, int radix)
{
	char *_pdst = dst;
	if (!val)//允许val等于0 
	{
		*_pdst = '0';
		*++_pdst = '\0';
		return dst;
	}
	if (val <0)
	{
		*_pdst++ = '-';
		val = -val;
	}
	char *_first = _pdst;
	char _cov;
	unsigned int _rem;
	while (val > 0)
	{
		_rem = (unsigned int)(val % radix);
		val /= radix;//每次计算一位 ，从低到高
		if (_rem > 9)//16进制
			*_pdst++ = (char)(_rem - 10 + 'a');
		else
			*_pdst++ = (char)(_rem + '0');
	}
	*_pdst-- = '\0';
	do{ //由于数据是地位到高位储存的，需要转换位置
		_cov = *_pdst;
		*_pdst = *_first;
		*_first = _cov;
		_pdst--;
		_first++;
	} while (_first < _pdst);
	return dst;
}



/*****************************************************************************/

/* Construct a new plugin instance. */
LADSPA_Handle
instantiateEqualizer(const LADSPA_Descriptor * Descriptor,
		     unsigned long             SampleRate) {
  Equalizer * psEqualizer;

  printf("============EQ/DRC Version 1.21==================\n");
  psEqualizer = (Equalizer *)malloc(sizeof(Equalizer));
  psEqualizer->m_eqsampleRate = (unsigned int)SampleRate;

  return psEqualizer;
}


/*****************************************************************************/
/* Initialise and activate a plugin instance. */
void
activateEqualizer(LADSPA_Handle Instance) {

  Equalizer * psEqualizer;
  psEqualizer = (Equalizer *)Instance;
  
  psEqualizer->m_eqfirstRun = 0;
  #ifdef DEBUG_
  psEqualizer->fp_in = fopen("/tmp/eq_in.pcm","wb");
  psEqualizer->fp_out = fopen("/tmp/eq_out.pcm","wb");
  #endif

}


/*****************************************************************************/


/* Connect a port to a data location. */
void 
connectPortToEqualizer(LADSPA_Handle Instance,
		       unsigned long Port,
		       LADSPA_Data * DataLocation) {

  Equalizer * psEqualizer;
  psEqualizer = (Equalizer *)Instance;
  //printf("--------connectPortToEqualizer----\n");
  switch (Port) {
  case EQ_DRC_CONTROL:
    psEqualizer->m_pfControlValue = DataLocation;
    //printf("-------psEqualizer->m_pfControlValue = %f\n",psEqualizer->m_pfControlValue);
    break;
  case EQ_DRC_INPUT1:
    psEqualizer->m_leftInputBuffer = DataLocation;
    break;
  case EQ_DRC_OUTPUT1:
    psEqualizer->m_leftOutputBuffer = DataLocation;
    break;
  case EQ_DRC_INPUT2:
    /* (This should only happen for stereo.) */
    psEqualizer->m_rightInputBuffer = DataLocation;
    break;
  case EQ_DRC_OUTPUT2:
    /* (This should only happen for stereo.) */
    psEqualizer->m_rightOutputBuffer = DataLocation;
    break;
  }
}


/*****************************************************************************/

void 
runMonoEqualizer(LADSPA_Handle Instance,
		 unsigned long SampleCount) {
  
  LADSPA_Data * pfInput;
  LADSPA_Data * pfOutput;
  Equalizer * psEqualizer;
 // unsigned long lSampleIndex;

  static char param_name[100] = "/data/cfg/eq_bin/Para_";//"/data/Para.bin";
  char samp_name[10];
  unsigned int samplerate;
  int pcm_channel = 1;

  float reset_para[PARALEN] = {0};
  FILE *fp = NULL;
  FILE *binFile = NULL;
  

  psEqualizer = (Equalizer *)Instance;

  pfInput = psEqualizer->m_leftInputBuffer;
  pfOutput = psEqualizer->m_leftOutputBuffer;

/******************************init****************************/
/* eq_drc init because the samplecount only be passed in this*/
if(psEqualizer->m_eqfirstRun == 0)
{
    psEqualizer->m_eqfirstRun = 1;
    samplerate = psEqualizer->m_eqsampleRate;
    if((samplerate != 48000)&&(samplerate != 44100) &&(samplerate != 32000)
        &&(samplerate != 16000) && (samplerate != 8000))
    {
        printf("Unsupport samplerate!\n");
        return ;
    }

    strcpy(param_name,"/data/cfg/eq_bin/Para_");
    rk_itoa(samplerate,samp_name,10);
    strcat(param_name,samp_name);
	strcat(param_name,"Hz_1ch.bin");
    printf("para_name  = %s\n",param_name);

    binFile = fopen(param_name,"rb");
    if((binFile == NULL))
    {
        system("cp /usr/lib/eq_bin/ /data/cfg/ -rf");
        printf("copy eq_bin success!!!\n");
    }
    else
    {
        fclose(binFile);
    }


	AudioPost_Init(param_name, SampleCount);

	#ifdef EQ_DRC_PARAM_DEBUG_
    psEqualizer->filename = param_name;//"/data/Para.bin";
   // printf("filename = %s\n",psEqualizer->filename);

	psEqualizer->fd = inotify_init();
	//printf("psEqualizer->fd = %d\n",psEqualizer->fd);
#endif
}
/**************************process*********************************/

  AudioPost_Process(pfInput, pfOutput , pcm_channel, SampleCount);
#ifdef DEBUG_
 fwrite(pfInput,sizeof(LADSPA_Data),SampleCount,psEqualizer->fp_in);
 fwrite(pfOutput,sizeof(LADSPA_Data),SampleCount,psEqualizer->fp_out);
#endif
  /*momitor bin-file was change only debug*/
#ifdef EQ_DRC_PARAM_DEBUG_
   /*adding the "/data/rpc-d/" directory into watch list. 
    *Here, the suggestion is to validate the existence of
    *the directory before adding into monitoring list.
    */
    psEqualizer->wd = inotify_add_watch( psEqualizer->fd, psEqualizer->filename,IN_MODIFY |IN_ACCESS | IN_CREATE | IN_DELETE | IN_OPEN | IN_CLOSE_NOWRITE);
   // printf("psEqualizer->wd = %d,fd = %d\n",psEqualizer->wd,psEqualizer->fd);
   // printf("filename = %s\n",psEqualizer->filename);
    
    if(psEqualizer->wd == IN_MODIFY)//IN_MODIFY
    {
        //printf("file was changed :wd = %d ",wd);
        fp = fopen(psEqualizer->filename,"rb");
        if(fp != NULL)
        {
            fread(reset_para,sizeof(float),PARALEN,fp);
            AudioPost_SetPara(reset_para, SampleCount);//EQ_DRC param reset
            printf("modified the param succedd!!!\n");
        }
        else{
            printf("ERROR:OPNE NEW BIN FILE FAILED!!!\n");
            return ;
        }
        fclose(fp);
        inotify_rm_watch (psEqualizer->fd, psEqualizer->wd);//删除监视
        psEqualizer->fd = inotify_init();//重新初始化监视
    }
  
#endif

/***********************this part must be changed*****/

}

/*****************************************************************************/

/*****************************************************************************/

void runStereoEqualizer(LADSPA_Handle Instance,
		   unsigned long SampleCount) {

  LADSPA_Data * pfInput;
  LADSPA_Data * pfOutput;
  Equalizer * psEqualizer;
  unsigned long lSampleIndex;


  static char param_name[100] =  "/data/cfg/eq_bin/Para_";//"/data/Para.bin";
  char samp_name[10];
  unsigned int samplerate;
  int pcm_channel = 2;

  float reset_para[PARALEN] = {0};
  FILE *fp = NULL;
  FILE *binFile = NULL;

  psEqualizer = (Equalizer *)Instance;
/******************************init****************************/
/* eq_drc init because the samplecount only be passed in this*/

if(psEqualizer->m_eqfirstRun == 0)
{
    psEqualizer->m_eqfirstRun = 1;
    samplerate = psEqualizer->m_eqsampleRate;
    if((samplerate != 48000)&&(samplerate != 44100) &&(samplerate != 32000)
        &&(samplerate != 16000) && (samplerate != 8000))
    {
        printf("Unsupport samplerate!\n");
        return ;
    }

    strcpy(param_name,"/data/cfg/eq_bin/Para_");
    rk_itoa(samplerate,samp_name,10);
    strcat(param_name,samp_name);
	strcat(param_name,"Hz_2ch.bin");
    printf("para_name  = %s\n",param_name);

    //detect eq_bin file
    binFile = fopen(param_name,"rb");
    if((binFile == NULL))
    {
        system("cp /usr/lib/eq_bin/ /data/cfg/ -rf");
         printf("copy eq_bin...\n");
    }
    else {
        fclose(binFile);
    }
	AudioPost_Init(param_name, SampleCount);

#ifdef EQ_DRC_PARAM_DEBUG_
    psEqualizer->filename = param_name;
   // printf("filename = %s\n",psEqualizer->filename);

	psEqualizer->fd = inotify_init();
	//printf("psEqualizer->fd = %d\n",psEqualizer->fd);
#endif
}

/***********************this part must be changed**********/

 pfInput = (LADSPA_Data *)malloc(2 * SampleCount * sizeof(LADSPA_Data));
 memset(pfInput,0.0,2 * SampleCount);
 pfOutput = (LADSPA_Data *)malloc(2 * SampleCount * sizeof(LADSPA_Data));
 memset(pfOutput,0.0,2 * SampleCount);
 
 for (lSampleIndex = 0; lSampleIndex <2 * SampleCount; lSampleIndex = lSampleIndex + 2) 
{
#if 0
    printf("*(pfInput) = %f\n",*(pfInput));
    *(pfInput++) =  *(psEqualizer->m_leftInputBuffer);
    psEqualizer->m_leftInputBuffer ++;
    printf("*(pfInput) = %f\n",*(pfInput));
    *(pfInput++) =  *(psEqualizer->m_rightInputBuffer);
    psEqualizer->m_rightInputBuffer ++;
    #else
    pfInput[lSampleIndex] = psEqualizer->m_leftInputBuffer[lSampleIndex/2];
    pfInput[lSampleIndex + 1] = psEqualizer->m_rightInputBuffer[lSampleIndex/2];
    //printf("*(pfInput) = %f %f\n",(pfInput[lSampleIndex]),pfInput[lSampleIndex + 1]);
    #endif
}

AudioPost_Process(pfInput, pfOutput, pcm_channel, SampleCount);
#ifdef DEBUG_
fwrite(pfInput,sizeof(LADSPA_Data),2*SampleCount,psEqualizer->fp_in);
fwrite(pfOutput,sizeof(LADSPA_Data),2*SampleCount,psEqualizer->fp_out);
#endif
 for (lSampleIndex = 0; lSampleIndex <2 * SampleCount; lSampleIndex = lSampleIndex + 2)
{
#if 0
     //printf("*(pfOutput) = %f\n",*(pfOutput));
     *(psEqualizer->m_leftInputBuffer ++) = *(pfOutput++);
   // printf("*(pfOutput) = %f\n",*(pfOutput));
     *(psEqualizer->m_rightInputBuffer ++) = *(pfOutput++);
     #else
     psEqualizer->m_leftOutputBuffer[lSampleIndex/2] = pfOutput[lSampleIndex];
     psEqualizer->m_rightOutputBuffer[lSampleIndex/2] = pfOutput[lSampleIndex +1];
 
     #endif
}


#ifdef EQ_DRC_PARAM_DEBUG_
   /*adding the "/data/rpc-d/" directory into watch list. 
    *Here, the suggestion is to validate the existence of
    *the directory before adding into monitoring list.
    */
    psEqualizer->wd = inotify_add_watch( psEqualizer->fd, psEqualizer->filename,IN_MODIFY |IN_ACCESS | IN_CREATE | IN_DELETE | IN_OPEN | IN_CLOSE_NOWRITE);
   // printf("psEqualizer->wd = %d,fd = %d\n",psEqualizer->wd,psEqualizer->fd);
   // printf("filename = %s\n",psEqualizer->filename);
    
    if(psEqualizer->wd == IN_MODIFY)//IN_MODIFY
    {
        //printf("file was changed :wd = %d ",wd);
        fp = fopen(psEqualizer->filename,"rb");
        if(fp != NULL)
        {
            fread(reset_para,sizeof(float),PARALEN,fp);
            AudioPost_SetPara(reset_para, SampleCount);//EQ_DRC param reset
            printf("modified the param succedd!!!\n");
        }
        else{
            printf("ERROR:OPNE NEW BIN FILE FAILED!!!\n");
            return ;
        }
        fclose(fp);
        inotify_rm_watch (psEqualizer->fd, psEqualizer->wd);//删除监视
        psEqualizer->fd = inotify_init();//重新初始化监视
    }
       
#endif


#if 0

  pfInput = psEqualizer->m_leftInputBuffer;
  pfOutput = psEqualizer->m_leftOutputBuffer;

  for (lSampleIndex = 0; lSampleIndex < SampleCount; lSampleIndex++) 
    *(pfOutput++) = *(pfInput++);//demo原样输出

  pfInput = psEqualizer->m_rightInputBuffer;
  pfOutput = psEqualizer->m_rightOutputBuffer;
  for (lSampleIndex = 0; lSampleIndex < SampleCount; lSampleIndex++) 
    *(pfOutput++) = *(pfInput++);
#endif
free(pfInput);
free(pfOutput);
}



/*****************************************************************************/

/* Throw away a simple delay line. */
void cleanupEqualizer(LADSPA_Handle Instance) {

/****************Equalizer deinit*************************/
  Equalizer * psEqualizer;
  psEqualizer = (Equalizer *)Instance;

  #ifdef DEBUG_
  fclose(psEqualizer->fp_in);
  fclose(psEqualizer->fp_out);
  #endif

  #ifdef EQ_DRC_PARAM_DEBUG_
    inotify_rm_watch (psEqualizer->fd, psEqualizer->wd);
  #endif

  if(psEqualizer->m_eqfirstRun == 1)
  {
       AudioPost_Destroy();
       psEqualizer->m_eqfirstRun = 0;
  }

  free(Instance);
}

/*****************************************************************************/

LADSPA_Descriptor * g_eqMonoDescriptor = NULL;
LADSPA_Descriptor * g_eqStereoDescriptor = NULL;

/*****************************************************************************/
/* _init() is called automatically when the plugin library is first
   loaded. */
void 
_init() {

  char ** pcPortNames;
  LADSPA_PortDescriptor * piPortDescriptors;
  LADSPA_PortRangeHint * psPortRangeHints;

  g_eqMonoDescriptor
    = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  g_eqStereoDescriptor 
    = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

  if (g_eqMonoDescriptor) {
  
    g_eqMonoDescriptor->UniqueID
      = 1060;
    g_eqMonoDescriptor->Label
      = strdup("eq_drc_mono");
    g_eqMonoDescriptor->Properties
      = LADSPA_PROPERTY_HARD_RT_CAPABLE;
    g_eqMonoDescriptor->Name 
      = strdup("Mono Equalizer");
    g_eqMonoDescriptor->Maker
      = strdup("Richard Furse (LADSPA example plugins)");
    g_eqMonoDescriptor->Copyright
      = strdup("None");
    g_eqMonoDescriptor->PortCount
      = 3;
    piPortDescriptors
      = (LADSPA_PortDescriptor *)calloc(3, sizeof(LADSPA_PortDescriptor));
    g_eqMonoDescriptor->PortDescriptors
      = (const LADSPA_PortDescriptor *)piPortDescriptors;
    piPortDescriptors[EQ_DRC_CONTROL]
      = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    piPortDescriptors[EQ_DRC_INPUT1]
      = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[EQ_DRC_OUTPUT1]
      = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    pcPortNames
      = (char **)calloc(3, sizeof(char *));
    g_eqMonoDescriptor->PortNames 
      = (const char **)pcPortNames;
    pcPortNames[EQ_DRC_CONTROL]
      = strdup("Gain");
    pcPortNames[EQ_DRC_INPUT1]
      = strdup("Input");
    pcPortNames[EQ_DRC_OUTPUT1]
      = strdup("Output");
    psPortRangeHints = ((LADSPA_PortRangeHint *)
			calloc(3, sizeof(LADSPA_PortRangeHint)));
    g_eqMonoDescriptor->PortRangeHints
      = (const LADSPA_PortRangeHint *)psPortRangeHints;
    psPortRangeHints[EQ_DRC_CONTROL].HintDescriptor
      = (LADSPA_HINT_BOUNDED_BELOW 
	 | LADSPA_HINT_LOGARITHMIC
	 | LADSPA_HINT_DEFAULT_1);
    psPortRangeHints[EQ_DRC_CONTROL].LowerBound 
      = 0;
    psPortRangeHints[EQ_DRC_INPUT1].HintDescriptor
      = 0;
    psPortRangeHints[EQ_DRC_OUTPUT1].HintDescriptor
      = 0;
    g_eqMonoDescriptor->instantiate 
      = instantiateEqualizer;
    g_eqMonoDescriptor->connect_port 
      = connectPortToEqualizer;
    g_eqMonoDescriptor->activate
      = activateEqualizer;
    g_eqMonoDescriptor->run
      = runMonoEqualizer;
    g_eqMonoDescriptor->run_adding
      = NULL;
    g_eqMonoDescriptor->set_run_adding_gain
      = NULL;
    g_eqMonoDescriptor->deactivate
      = NULL;
    g_eqMonoDescriptor->cleanup
      = cleanupEqualizer;
  }
  
  if (g_eqStereoDescriptor) {
    
    g_eqStereoDescriptor->UniqueID
      = 1061;
    g_eqStereoDescriptor->Label
      = strdup("eq_drc_stereo");
    g_eqStereoDescriptor->Properties
      = LADSPA_PROPERTY_HARD_RT_CAPABLE;
    g_eqStereoDescriptor->Name 
      = strdup("Stereo Equalizer");
    g_eqStereoDescriptor->Maker
      = strdup("Richard Furse (LADSPA example plugins)");
    g_eqStereoDescriptor->Copyright
      = strdup("None");
    g_eqStereoDescriptor->PortCount
      = 5;
    piPortDescriptors
      = (LADSPA_PortDescriptor *)calloc(5, sizeof(LADSPA_PortDescriptor));
    g_eqStereoDescriptor->PortDescriptors
      = (const LADSPA_PortDescriptor *)piPortDescriptors;
    piPortDescriptors[EQ_DRC_CONTROL]
      = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    piPortDescriptors[EQ_DRC_INPUT1]
      = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[EQ_DRC_OUTPUT1]
      = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[EQ_DRC_INPUT2]
      = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[EQ_DRC_OUTPUT2]
      = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    pcPortNames
      = (char **)calloc(5, sizeof(char *));
    g_eqStereoDescriptor->PortNames 
      = (const char **)pcPortNames;
    pcPortNames[EQ_DRC_CONTROL]
      = strdup("Gain");
    pcPortNames[EQ_DRC_INPUT1]
      = strdup("Input (Left)");
    pcPortNames[EQ_DRC_OUTPUT1]
      = strdup("Output (Left)");
    pcPortNames[EQ_DRC_INPUT2]
      = strdup("Input (Right)");
    pcPortNames[EQ_DRC_OUTPUT2]
      = strdup("Output (Right)");
    psPortRangeHints = ((LADSPA_PortRangeHint *)
			calloc(5, sizeof(LADSPA_PortRangeHint)));
    g_eqStereoDescriptor->PortRangeHints
      = (const LADSPA_PortRangeHint *)psPortRangeHints;
    psPortRangeHints[EQ_DRC_CONTROL].HintDescriptor
      = (LADSPA_HINT_BOUNDED_BELOW 
	 | LADSPA_HINT_LOGARITHMIC
	 | LADSPA_HINT_DEFAULT_1);
    psPortRangeHints[EQ_DRC_CONTROL].LowerBound 
      = 0;
    psPortRangeHints[EQ_DRC_INPUT1].HintDescriptor
      = 0;
    psPortRangeHints[EQ_DRC_OUTPUT1].HintDescriptor
      = 0;
    psPortRangeHints[EQ_DRC_INPUT2].HintDescriptor
      = 0;
    psPortRangeHints[EQ_DRC_OUTPUT2].HintDescriptor
      = 0;
    g_eqStereoDescriptor->instantiate 
      = instantiateEqualizer;
    g_eqStereoDescriptor->connect_port 
      = connectPortToEqualizer;
    g_eqStereoDescriptor->activate
      = activateEqualizer;
    g_eqStereoDescriptor->run
      = runStereoEqualizer;
    g_eqStereoDescriptor->run_adding
      = NULL;
    g_eqStereoDescriptor->set_run_adding_gain
      = NULL;
    g_eqStereoDescriptor->deactivate
      = NULL;
    g_eqStereoDescriptor->cleanup
      = cleanupEqualizer;
  }
}


/*****************************************************************************/

void
deleteDescriptor(LADSPA_Descriptor * psDescriptor) {
  unsigned long lIndex;
   
  if (psDescriptor) {
    free((char *)psDescriptor->Label);
    free((char *)psDescriptor->Name);
    free((char *)psDescriptor->Maker);
    free((char *)psDescriptor->Copyright);
    free((LADSPA_PortDescriptor *)psDescriptor->PortDescriptors);
    for (lIndex = 0; lIndex < psDescriptor->PortCount; lIndex++)
      free((char *)(psDescriptor->PortNames[lIndex]));
    free((char **)psDescriptor->PortNames);
    free((LADSPA_PortRangeHint *)psDescriptor->PortRangeHints);
    free(psDescriptor);
  }
}


/*****************************************************************************/

/* _fini() is called automatically when the library is unloaded. */
void
_fini() {

    
  deleteDescriptor(g_eqMonoDescriptor);
  deleteDescriptor(g_eqStereoDescriptor);
}

/*****************************************************************************/


/* Return a descriptor of the requested plugin type. There are two
   plugin types available in this library (mono and stereo). */
const LADSPA_Descriptor * 
ladspa_descriptor(unsigned long Index) {
  /* Return the requested descriptor or null if the index is out of
     range. */
  switch (Index) {
  case 0:
    return g_eqMonoDescriptor;
  case 1:
    return g_eqStereoDescriptor;
  default:
    return NULL;
  }
}

/*****************************************************************************/

/* EOF */









