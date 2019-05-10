/*  --------------------------------------------------------------------------------------------------------
 *  File:   custom_log.h 
 *
 *  Desc:   ChenZhen ƫ�õ�, �� Android log �����Ķ�������. 
 *
 *          -----------------------------------------------------------------------------------
 *          < ϰ�� �� ������ > : 
 *
 *          -----------------------------------------------------------------------------------
 *
 *  Usage:	���ñ� log ������ .c or .h �ļ�, ��Ҫʹ������� log ����, 
 *          ������� inclue ���ļ�֮ǰ, "#define ENABLE_DEBUG_LOG" ��. 
 *          
 *          ͬ log.h һ��, client �ļ��� inclue ���ļ�֮ǰ, "���" #define LOG_TAG <module_tag>
 *          "module_tag" �ǵ�ǰ�����Ե� ģ��ı�ʶ. 
 *          
 *          ����, ����ʹ�� V() ����� verbose �� log, �������� "#define ENABLE_VERBOSE_LOG". 
 *
 *          ���û�Ҫ dump ���� mem ����, ��ʱ�д������ٵ� log ���, ���ܵ��� host �˽���������, 
 *          ��ʱ�û����Զ���� MASSIVE_LOG, ͨ�����ض� log �ӿ�����ʱ, ��֤ host �˽��յ����� log. 
 *
 *  Note:
 *
 *  Author: ChenZhen
 *
 *  --------------------------------------------------------------------------------------------------------
 *  Version:
 *          v1.0
 *  --------------------------------------------------------------------------------------------------------
 *  Log:
	----Tue Mar 02 21:30:33 2010            v.10
 *        
 *  --------------------------------------------------------------------------------------------------------
 */


#ifndef __CUSTOM_LOG_H__
#define __CUSTOM_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------------------------------------
 *  Include Files
 * ---------------------------------------------------------------------------------------------------------
 */

#include <cutils/log.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
    
/* ---------------------------------------------------------------------------------------------------------
 *  Macros Definition 
 * ---------------------------------------------------------------------------------------------------------
 */

/** 
 * ������ macro �б�����, dump �ڴ��� log �ӿ�ÿ��ִ��֮�� sleep ����ʱ��. 
 */
// #define MASSIVE_LOG 

/**
 * �������� MASSIVE_LOG, dump �ڴ��� log �ӿ�ÿ�� sleep ��ʱ�� us Ϊ��λ. 
 */
#define RESPITE_TIME_FOR_MASSIVE_LOG_IN_US      (10 * 1000)

/** ������ macro �б�����, �� ʹ�� log ��� : �μ� "Usage". */ 
// #define ENABLE_DEBUG_LOG

/** .! : ����Ҫȫ�ֵعر� D log, ����ʹ������Ĵ���. */
/*
#undef ENABLE_DEBUG_LOG
#warning "debug log is disabled globally!"
*/

/** �Ƿ� log Դ�ļ� ·����Ϣ. */
#define LOG_FILE_PATH

/*-----------------------------------*/

#ifndef LOGV
#define LOGV ALOGV
#endif

#ifndef LOGD
#define LOGD ALOGD
#endif

#ifndef LOGI
#define LOGI ALOGI
#endif

#ifndef LOGW
#define LOGW ALOGW
#endif

#ifndef LOGE
#define LOGE ALOGE
#endif

/*-----------------------------------*/
#ifdef ENABLE_VERBOSE_LOG

#undef ENABLE_DEBUG_LOG
#define ENABLE_DEBUG_LOG
    
// #error
#ifdef LOG_FILE_PATH
#define V(fmt, args...) \
    { D(fmt, ## args); }
    // { LOGV("[File] : %s; [Line] : %d; [Func] : %s; " fmt , __FILE__, __LINE__, __FUNCTION__, ## args); }

#else
#define V(fmt, args...) \
    { D(fmt, ## args); }
    // { LOGV("[Line] : %d; [Func] : %s; " fmt , __LINE__, __FUNCTION__, ## args); }
#endif

#else
#define V(...)  ((void)0)
#endif

/*-----------------------------------*/
#ifdef ENABLE_DEBUG_LOG

#ifdef LOG_FILE_PATH
#define D(fmt, args...) \
    { LOGD("[File] : %s; [Line] : %d; [Func] : %s;\n" fmt , __FILE__, __LINE__, __FUNCTION__, ## args); }

#else
#define D(fmt, args...) \
    { LOGD("[Line] : %d; [Func] : %s; " fmt , __LINE__, __FUNCTION__, ## args); }
#endif

#else
#define D(...)  ((void)0)
#endif


/*-----------------------------------*/
// #ifdef ENABLE_DEBUG_LOG

#ifdef LOG_FILE_PATH
#define I(fmt, args...) \
    { LOGI("[File] : %s; [Line] : %d; [Func] : %s;\n" fmt , __FILE__, __LINE__, __FUNCTION__, ## args); }
#else
#define I(fmt, args...) \
    { LOGI("[Line] : %d; [Func] : %s; " fmt , __LINE__, __FUNCTION__, ## args); }
#endif

/*
#else       // #ifdef ENABLE_DEBUG_LOG
    
#define I(fmt, args...) \
    { LOGI(fmt, ## args); }

#endif
*/

/*-----------------------------------*/
#ifdef LOG_FILE_PATH
#define W(fmt, args...) \
    { LOGW("[File] : %s; [Line] : %d; [Func] : %s;\n" fmt , __FILE__, __LINE__, __FUNCTION__, ## args); }
#else
#define W(fmt, args...) \
    { LOGW("[Line] : %d; [Func] : %s; " fmt , __LINE__, __FUNCTION__, ## args); }
#endif

/*-----------------------------------*/
#ifdef LOG_FILE_PATH
#define E(fmt, args...) \
    { LOGE("[File] : %s; [Line] : %d; [Func] : %s;\n" fmt , __FILE__, __LINE__, __FUNCTION__, ## args); }
#else
#define E(fmt, args...) \
    { LOGE("[Line] : %d; [Func] : %s; " fmt , __LINE__, __FUNCTION__, ## args); }
#endif

/*-----------------------------------*/
/** 
 * �������ظ����е���ǰλ�õĴ������� threshold ���ߵ�һ�ε���, ���ӡָ���� log ��Ϣ. 
 */
#ifdef ENABLE_DEBUG_LOG
#define D_WHEN_REPEAT(threshold, fmt, args...) \
    do { \
        static int count = 0; \
        if ( 0 == count || (count++) == threshold ) { \
            D(fmt, ##args); \
            count = 1; \
        } \
    } while (0)
#else
#define D_WHEN_REPEAT(...)  ((void)0)
#endif
    
/*-------------------------------------------------------*/
/** ʹ�� D(), ��ʮ���Ƶ���ʽ��ӡ���� 'var' �� value. */
#define D_DEC(var) \
{ \
    long long num = (var); \
    D(#var " = %lld.", num); \
}

/** ʹ�� D(), ��ʮ�����Ƶ���ʽ��ӡ���� 'var' �� value. */
#define D_HEX(var) \
{ \
    long long num = (var); \
    D(#var " = 0x%llx.", num); \
}

/** ʹ�� D(), ��ʮ�����Ƶ���ʽ ��ӡָ�����ͱ��� 'ptr' �� value. */
#define D_PTR(ptr)  D(#ptr " = %p.", ptr);

/** ʹ�� D(), ��ӡ char �ִ�. */
#define D_STR(pStr) \
{\
    if ( NULL == pStr )\
    {\
        D(#pStr" = NULL.");\
    }\
    else\
    {\
        D(#pStr" = '%s'.", pStr);\
    }\
}

/*---------------------------------------------------------------------------*/

/** ʹ�� V(), ��ʮ���Ƶ���ʽ��ӡ���� 'var' �� value. */
#define V_DEC(var)  V(#var " = %d.", var);

/** ʹ�� V(), ��ʮ�����Ƶ���ʽ��ӡ���� 'var' �� value. */
#define V_HEX(var)  V(#var " = 0x%x.", var);

/** ʹ�� V(), ��ʮ�����Ƶ���ʽ ��ӡָ�����ͱ��� 'ptr' �� value. */
#define V_PTR(ptr)  V(#ptr " = %p.", ptr);

/** ʹ�� V(), ��ӡ char �ִ�. */
#define V_STR(pStr) \
{\
    if ( NULL == pStr )\
    {\
        V(#pStr" = NULL.");\
    }\
    else\
    {\
        V(#pStr" = '%s'.", pStr);\
    }\
}

/*-------------------------------------------------------*/

/**
 * ���ú���, ����鷵��ֵ, ���ݷ���ֵ�����Ƿ���ת��ָ���Ĵ��������. 
 * @param functionCall
 *          ���ض������ĵ���, �ú����ķ���ֵ������ ���� �ɹ� or err �� ������. 
 *          ����, �����ú��� "����" �Ǳ�����Ϊ "���� 0 ��ʾ�����ɹ�". 
 * @param result
 *		    ���ڼ�¼�������ص� error code �� ���ͱ���, ͨ���� "ret" or "result" ��.
 * @param label
 *		    ���������ش���, ����Ҫ��ת���� �������� ���, ͨ������ "EXIT". 
 */
#define CHECK_FUNC_CALL(functionCall, result, label) \
{\
	if ( 0 != ( (result) = (functionCall) ) )\
	{\
		E("Function call returned error : " #result " = %d.", result);\
		goto label;\
	}\
}

/**
 * ���� heap �ռ�, ��鷵�ص�ַ, ������õ��� heap �ռ�ȫ���� 0.
 * ��ʧ��, ��ָ���� error code ��ֵ��ָ���ı���, ֮����ת��ָ���Ĵ��������. 
 * @param pDest
 *		    ���ڱ������õ��� heap block ��ַ��Ŀ��ָ�����.
 * @param type
 *          ������Ŀռ���, ���ݵ�Ԫ ������. 
 * @param size
 *          ������ռ��а��� "���ݵ�Ԫ"(��Ӧ���� 'type') �ľ������. 
 *          ����, ������ռ���ֽڴ�С �� "sizeof(type) * size". 
 * @param retVar
 *		    ���ڼ�¼�������ص� error code �� ���ͱ���, ͨ���� "ret" or "result" ��.
 * @param errCode
 *          �������� "�ڴ治��" �� error code, ��ͬģ���п���ʹ�ò�ͬ�ĳ�����ʶ. 
 *          ��׼ linux ��, ͨ���� ENOMEM.
 * @param label
 *		    ���ռ����ʧ��, ����Ҫ��ת���� ���������� ���, ͨ������ "EXIT". 
 */
#define CHECK_MALLOC(pDest, type, size, retVar, errCode, label) \
{\
	if ( NULL == ( (pDest) = (type*)malloc(sizeof(type) * (size) ) ) )\
	{\
        retVar = errCode;\
        E("Failed to malloc %u bytes.", sizeof(type) * (size) );\
		goto label;\
	}\
	memset( (void*)(pDest), 0, sizeof(type) * (size) );\
}

/**
 * ���ض�������, �ж� error ����, �Ա��� 'retVar' ���� 'errCode', 
 * Log �����Ӧ�� Error Caution, Ȼ����ת 'label' ָ���Ĵ��봦ִ��. 
 * @param msg
 *          ���ִ���ʽ����ʾ��Ϣ. 
 * @param retVar
 *		    ��ʶ����ִ��״̬���߽���ı���, �������þ���� Error Code. 
 *		    ͨ���� 'ret' or 'result'. 
 * @param errCode
 *          �����ض� error �ĳ�����ʶ, ͨ���� �����̬. 
 * @param label
 *          ����Ҫ��ת���Ĵ��������ı��, ͨ������ 'EXIT'. 
 * @param args...
 *          ��Ӧ 'msgFmt' ʵ���� '%s', '%d', ... �� ת��˵���� �ľ���ɱ䳤ʵ��. 
 */
#define SET_ERROR_AND_JUMP(msgFmt, retVar, errCode, label, args...) \
{\
    E("To set '" #retVar "' to %d('" #errCode "') : " msgFmt, (errCode), ## args);\
	(retVar) = (errCode);\
	goto label;\
}

#define EXIT_FOR_DEBUG \
{\
    E("To exit for debug.");\
    return 1;\
}

/*---------------------------------------------------------------------------*/

/**
 * ����ָ�������е�Ԫ�ظ���. 
 * @param array
 *      ��Ч������ʵ����ʶ��. 
 */
#define ELEMENT_NUM(array)      ( sizeof(array) /  sizeof(array[0]) )

/*---------------------------------------------------------------------------*/

/**
 * ����.
 * ���� 'expect' �� ʵ�α��ʽ "����", �� �� 0, ����ֱ�� abort ��ǰ����. 
 * @param msgFmt
 *          �ִ���ʽ����ʾ��Ϣ, ���԰��� '%s', '%d' �� ת��˵����. 
 * @param args... 
 *          ��Ӧ 'msgFmt' ʵ���� '%s', '%d', ... �� ת��˵���� �ľ���ɱ䳤ʵ��. 
 */
#define ASSERT(expect, msgFmt, args...) \
    do { \
        if ( !(expect) ) \
        { \
            E("assert('" #expect "') FAILED, to ABORT. " msgFmt, ## args); \
            abort(); \
        } \
    } while ( 0 )

/* ---------------------------------------------------------------------------------------------------------
 *  Types and Structures Definition
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 *  Global Functions' Prototype
 * ---------------------------------------------------------------------------------------------------------
 */


/* ---------------------------------------------------------------------------------------------------------
 *  Inline Functions Implementation 
 * ---------------------------------------------------------------------------------------------------------
 */
 
/*-------------------------------------------------------*/
/**
 * dump �� ��ַ 'pStart' ��ʼ, ����Ϊ 'len' �ֽڵ� memory block ������(16 ����������ʽ). 
 */
#ifdef ENABLE_DEBUG_LOG
#define D_MEM(pStart, len) \
    { \
        D("dump memory from addr of '" #pStart"' : "); \
        dumpMemory( (pStart), (len) ); \
    }
inline static void dumpMemory(const void* pStart, uint32_t len)
{
    char* pBuf = NULL;          /* dump buffer. �������ı�����ʽ("0xaa, 0xbb, ...") ��ʾ һ��, ��� 16 �ֽڵ� ʮ��������. */
    unsigned int bufLen = 0;    /* *pBuf ���ֽڳ���, ����ĩβ�� '\0'. */

    unsigned char* pSource = (unsigned char*)pStart;          /* ���� sprintf ������Դ��ַ. */
    char* pTarget = NULL;                                     /* Ŀ���ַ. */

    unsigned int i, j;

#define BYTES_PER_LINE (16)                         /* dump ���ÿ�д�ӡ�� source bytes ����Ŀ. */
    int BUF_SIZE_PER_SRC_BYTE  = strlen("0xXX, ");  // const. ÿ���� dump �� �ֽ��� *pBuf ����Ҫ�Ŀռ��С. 
                                                    // memory �е�ÿ���ֽڱ���ʾΪ "0xXX, " ���� "0xXX\n " ����ʽ. 
    
    unsigned int fullLines = len / BYTES_PER_LINE;  // �� 16 �ֽڵ�����. 
    unsigned int leftBytes = len % BYTES_PER_LINE;  // �����ܵĲ����е� �ֽڵĸ���. 

    if ( NULL == pStart || 0 == len )
    {
        return;
    }
    
    bufLen = (BYTES_PER_LINE * BUF_SIZE_PER_SRC_BYTE) + 1;  
    pBuf = (char*)malloc(bufLen); 
    if ( NULL == pBuf )
    {
        E("no enough memory.");
        return;
    }

    LOGD("from %p; length %d : ", pStart, len);

    /* �������е� full line, ... */
    for ( j = 0; j < fullLines; j++ )
    {
        pTarget = pBuf;
        /* ����ÿ���� log �� �ֽ�, ... */
        for ( i = 0; i < BYTES_PER_LINE; i++ )
        {
            /* ��ӡ�� *pTarget. */
            sprintf(pTarget, "0x%02x, ", *pSource);

            pTarget += BUF_SIZE_PER_SRC_BYTE;
            pSource++;
        }
        
        *(++pTarget) = '\0';
        /* log out ��ǰ��. */
        LOGD("\t%s", pBuf);
    }
    
    /* �������� ��������. */
    leftBytes = len % BYTES_PER_LINE;
    if ( 0 != leftBytes )
    {
        pTarget = pBuf;

        for ( i = 0; i < leftBytes; i++ )
        {
            sprintf(pTarget, "0x%02x, ", *pSource);
            pTarget += BUF_SIZE_PER_SRC_BYTE;
            pSource++;
        }

        *(++pTarget) = '\0';
        LOGD("\t%s", pBuf);
    }
    
    free(pBuf);
    
#ifdef MASSIVE_LOG
    usleep(RESPITE_TIME_FOR_MASSIVE_LOG_IN_US);
#endif
    return;
}

#else       // #ifdef ENABLE_DEBUG_LOG
#define D_MEM(...)  ((void)0)
#endif

/*-------------------------------------------------------*/

#ifdef ENABLE_VERBOSE_LOG
#define V_MEM(pStart, len) \
    { \
        D("dump memory from addr of '" #pStart"' : "); \
        dumpMemory( (pStart), (len) ); \
    }
#else       // #ifdef ENABLE_VERBOSE_LOG
#define V_MEM(...)  ((void)0)
#endif

/*---------------------------------------------------------------------------*/

#define DUMP_SIZE_PER_SRC_BYTE       (sizeof("0xXX, ") - 1)             // "1" : �ִ����� '\0'.

/** 
 * �� 'pStart' �� 'len' ָ����һ���ڴ�����, ��  hex ASCII ����̬ dump 'pBuf' ָ���� buf ��. 
 * ��������֮��, "*pBuf' β���� '\0' �ַ�. 
 * .! : ������ "����" ��֤ 'pDest' ָ��� buf �Ĳ�С�� 'len * DUMP_SIZE_PER_SRC_BYTE'. 
 */
inline static void dumpMemInHexAsciiToMem(const void* pStart, unsigned int len, char* pBuf) 
{
    unsigned int i = 0;
    unsigned char* pSource = (unsigned char*)pStart;
    char* pTarget = pBuf;       // �ض�д�������Ŀ���ַ. 

    if ( NULL == pStart || 0 == len || NULL == pBuf )
    {
        return;
    }
    
    for ( i = 0; i < len; i++ )
    {
        if ( i < len - 1 )
        {
            sprintf(pTarget, "0x%02x, ", *pSource);
        }
        else
        {
            sprintf(pTarget, "0x%02x", *pSource);
        }
        pSource++;
        pTarget += DUMP_SIZE_PER_SRC_BYTE;
    }
    
    pBuf[len * DUMP_SIZE_PER_SRC_BYTE - 1] = '\0';
}

/*---------------------------------------------------------------------------*/

/**
 * �� hex ASCII ��ʽ log dump �� 'pStart' ��ʼ, ����Ϊ 'len' �ֽڵ� mem ������, 16 �ֽ�Ϊһ��. 
 * ǰ���� 'indentNum' �� '\t' ����. 
 * ֧�ֵ�������������� 15. 
 */
inline static void dumpMemoryWithIndents(const void* pStart, unsigned int len, unsigned int indentNum)
{
    char indents[16];

    const unsigned int SRC_BYTES_NUM_PER_LINE = 16;     // �����ÿ����ʾ��Դ�ֽڵĸ���. 

    unsigned int bufLen = SRC_BYTES_NUM_PER_LINE * DUMP_SIZE_PER_SRC_BYTE;
    char* pBuf = (char*)malloc(bufLen);

    unsigned int linesNum = 0;        // ���������.

    unsigned char* pSrcStartInLine = NULL;      // ��ǰ�������Ӧ��Դ�ֽ����е���ʼ��ַ. 
    unsigned int srcBytesNumInLine = 0; // ��ǰ��Ҫ����˵�Դ�ֽڵĸ���. 
    
    unsigned char i = 0;
    
    /* ��ԭ���ݿ��Ա����ɸ� ���� ���ô�ӡ, ��... */
    if ( 0 == (len % SRC_BYTES_NUM_PER_LINE) )
    {
        linesNum = len / SRC_BYTES_NUM_PER_LINE;
    }
    /* ����, ... */
    else
    {
        /* �������һ�е� ������. */
        linesNum = (len / SRC_BYTES_NUM_PER_LINE) + 1;
    }

    /*-------------------------------------------------------*/

    indents[0] = '\0';
    // for ( i = 0; i < indentNum - 1; i++ )       // "-1" : ��������ݵĵ�ַ��ʶ, �������һ������. 
    for ( i = 0; i < indentNum; i++ )       // "-1" : ��������ݵĵ�ַ��ʶ, �������һ������. 
    {
        strcat(indents , "\t");
    }
    *(indents + indentNum) = '\0';
    
    /*-------------------------------------------------------*/

    pBuf[0] = '\0';
    pSrcStartInLine = (unsigned char*)pStart;

    /* �������. */
    for ( i = 0; i < linesNum; i++ )
    {
        srcBytesNumInLine = (len - (i * SRC_BYTES_NUM_PER_LINE) ) % SRC_BYTES_NUM_PER_LINE;

        /* ����ǰ�� "����" ���һ��(�� (linesNum - 1) �� , ��... */
        if ( i < (linesNum - 1) )
        {
            srcBytesNumInLine = SRC_BYTES_NUM_PER_LINE;
        }
        /* ����, ����ǰ�� "��" ���һ��", ��... */
        else
        {
            /* ��������, ��... */
            if ( 0 == (len % SRC_BYTES_NUM_PER_LINE ) )
            {
                srcBytesNumInLine = SRC_BYTES_NUM_PER_LINE;
            }
            else 
            {
                srcBytesNumInLine = len % SRC_BYTES_NUM_PER_LINE;
            }
        }
        
        dumpMemInHexAsciiToMem(pSrcStartInLine, srcBytesNumInLine, pBuf);
        LOGD("%s" " [0x%04x] : " "%s", indents, SRC_BYTES_NUM_PER_LINE * i, pBuf);
        
        pSrcStartInLine += srcBytesNumInLine;
    }

    /*-------------------------------------------------------*/

    free(pBuf);

#ifdef MASSIVE_LOG
    usleep(RESPITE_TIME_FOR_MASSIVE_LOG_IN_US);
#endif
}

/**
 * �� 'pIndentsBuf' ָ��� buf ��, ��ͷ���� 'indentNum' �� '\t' �ַ�(����), ������һ�� '\0'. 
 * ͨ�� pIndentsBuf ָ��һ�� 16 �ֽڵ� buffer. 
 */
inline static void setIndents(char* pIndentsBuf, unsigned char indentNum)
{
    unsigned char i = 0; 

    const unsigned char MAX_NUM_OF_INDENT = 16 - sizeof('\0');
    if ( indentNum > MAX_NUM_OF_INDENT )
    {
        indentNum = MAX_NUM_OF_INDENT;
    }

    *pIndentsBuf = '\0';
    for ( i = 0; i < indentNum; i++ )
    {
        strcat(pIndentsBuf, "\t");
    }
    *(pIndentsBuf + indentNum) = '\0';
}

inline static int dumpCharArray(const char* pStart, unsigned int len, unsigned char indentNum)
{
    int ret = 0;

    char indents[16];
    setIndents(indents, indentNum);

    char* pBuf = NULL;

    /*-------------------------------------------------------*/
    
    if ( NULL == pStart )
    {
        LOGD("%s %s", indents, "null");
        goto EXIT;
    }
    
    CHECK_MALLOC( pBuf, char, len + 1, ret, -1, EXIT);
    
    memcpy(pBuf, pStart, len);
    pBuf[len] = '\0';
    // pBuf[len] = 0;
    LOGD("%s %s", indents, pBuf);

EXIT:
    if ( NULL != pBuf )
    {
        free(pBuf);
    }

    return ret;
}

inline static int dumpStr(const char* pStr, unsigned char indentNum)
{
    int ret = 0;

    char indents[16];
    setIndents(indents, indentNum);
    /*-------------------------------------------------------*/

    if ( NULL == pStr )
    {
        LOGD("%s %s", indents, "null");
    }
    else
    {
        LOGD("%s %s", indents, pStr);
    }
    
    return ret;
}

/* ############################################################################################# */
/* < ������� Android ƽ̨�ĵ��Ժ�. > */

/**
 * ��ӡ sp<T> ʵ�� "sp" ָ���ʵ������ĵ�ǰ ǿ�����ü���. 
 * "D_SC" : Debug log Strong Count.
 */
#define D_SC(sp)    D("strong count of '" #sp "' is '%d'.", (sp)->getStrongCount() )


#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_LOG_H__ */

