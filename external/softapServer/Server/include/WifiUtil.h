#ifndef __WIFI_UTIL_H__
#define __WIFI_UTIL_H__

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "Server.h"
#include "WifiInfo.h"

#define WIFI_CONFIG_MAX 240

class WifiUtil{

public:
    WifiUtil(){}

public:
    /* use wpa_cli to get wifi list from device*/
    std::string getWifiListJson();

    /**
     * try connect wifi by wpa_cli.
     * it will create a new thread detached to check wifi state.
     * @Param recv_buff  http request header.
     */ 
    void WifiSetUp(char recv_buff[]);

};
#endif // __WIFI_UTIL_H__

