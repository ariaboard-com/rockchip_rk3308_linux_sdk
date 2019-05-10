/** 
 * @file	cae_lib.h
 * @brief	
 * 
 *  This file implements interfaces of module and dynamic library.
 * 
 * @author	    cyhu
 * @version	1.0
 * @date	    2016-01-06
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2016-01-06	<td>cyhu	<td>Create this file</tr>
 * </table>
 * 
 */


#ifndef __CAE_LIB_H__
#define __CAE_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "dlfcn.h"
typedef void* CAE_MODULEHANDLE;
typedef void* CAE_LIBHANDLE;

CAE_LIBHANDLE cae_LoadLibrary( const char* lib_name );

int cae_FreeLibrary( CAE_LIBHANDLE lib_handle );

void* cae_GetProcAddress( CAE_LIBHANDLE lib_handle, const char* fun_name );


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif	/* __MSP_LIB_H__ */
