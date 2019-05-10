/*
 *  wlan_test.c  --  wlan test application
 *
 *  Copyright (c) 2017 Rockchip Electronics Co. Ltd.
 *  Author: Panzhenzhuan Wang <randy.wang@rock-chips.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//WLAN test program
#include <stdio.h>

//open()相关头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//error相关头文件
#include <errno.h>
#include <string.h>
#include "wlan_test.h"

#define LOG_TAG "wlan_test"
#include "common.h"
#define WLAN_PROC_ERR -45

#define WLAN_START_UP_COMMAND "ifconfig wlan0 up"
#define WLAN_MANAGE_START "wpa_supplicant -i wlan0 -c /etc/wpa_supplicant.conf &"
#define WLAN_SCAN_COMMAND "wpa_cli scan 'wlan0'"
#define WLAN_SCAN_RESULT  "wpa_cli scan_r | busybox awk 'NR>=3{print $5,$3}'"
#define SCAN_RESULT_FILE "/tmp/wlan_scan_result.txt"

#define AP_SUPPORT_CMD "iw list | busybox grep AP > %s"
#define AP_SUPPORT_FILE "/tmp/wlan_ap_result.txt"
#define SOFTAP_MSG "dmesg | grep SSID | tail -n 6 > %s"
#define MSG_FILE "/tmp/softap_start_msg.txt"


//* 1、关闭后台wpa_supplicant进程
static int close_wpa_supplicant(void)
{
    int test_flag = -1;
    char pid_buf[64];
    char cmd[64];
    FILE* pp;
    printf("====================function : %s start =================\n",__func__);
    pp = popen("ps |grep wpa_supplicant|awk 'NR==1 {print $1}'","r");
    //pp = popen("ps | grep wpa_supplicant | grep -v grep","r");
    //如果文件打开失败，则输出错误信息
    if (!pp)
    {
        printf("%s popen err%s\n",__func__,strerror(errno));
        return -1;
    }
    memset(pid_buf,0,sizeof(pid_buf));
    fgets(pid_buf,sizeof(pid_buf),pp);
    pclose(pp);
    printf("Get pid_buf is: \t %s\n",pid_buf);
    sprintf(cmd,"kill -9 %d",atoi(pid_buf));
    printf("cmd is: %s\n",cmd);
    system(cmd);
    printf("====================function : %s finish =================\n",__func__);
    return 0;
}

/*
 * RSSI Levels as used by notification icon
 *
 * Level 4  -55 <= RSSI
 * Level 3  -66 <= RSSI < -55
 * Level 2  -77 <= RSSI < -67
 * Level 1  -88 <= RSSI < -78
 * Level 0         RSSI < -88
 */
static int calc_rssi_lvl(int rssi)
{
	if (rssi >= -55)
		return 4;
	else if (rssi >= -66)
		return 3;
	else if (rssi >= -77)
		return 2;
	else if (rssi >= -88)
		return 1;
	else
		return 0;
}


/* 将扫描到的WiFi信息输出到path对应文件
 * 并显示第一个WiFi名字和信号强度
*/
int wlan_exec(const char *cmd, char *path)
{
    char ssid[128];
    int signal_level;
    int ch=0;

     printf("=================== function :%s start======================\n\n",__func__);

    //显示第一个WiFi信息
    FILE *pp = popen(cmd, "r");
    //如果文件打开失败，则输出错误信息
    if (!pp)
    {
        printf("%s popen err%s\n",__func__,strerror(errno));
        return -1;
    }

	while(!feof(pp))
	{
	    fscanf(pp,"%s",ssid);
	    fscanf(pp,"%d",&signal_level);
	    //根据信号强度判断WiFi是否正常工作
	    if(signal_level< -200||signal_level >=0)
        {
            printf("wlan scan result is null\n");
            return -1;
        }
	    printf("SSID is: %s,signal level is: %d\n",ssid,signal_level);
	    printf("SSID is: %s,rssi is: %d\n",ssid,calc_rssi_lvl(signal_level));
	}
	pclose(pp);
	printf("\n=================== function :%s finish======================\n",__func__);
	return 0;
}

void *wlan_test(void *argv)
{
    int test_flag=-1;
    char cmd[128];
    printf("===================Wlan test start======================\n");
    //sprintf(cmd,"aplay %s/wlan_test_start.wav",AUDIO_PATH);
    //system(cmd);
    //system("aplay /data/cfg/test/wlan_test_start.wav");

    //1、先关闭后台wpa_supplicant进程，然后先卸载WLAN驱动固件，然后重新加载驱动固件
    test_flag = close_wpa_supplicant();
    system("echo 1 > /sys/class/rkwifi/driver");

    //2、开启wlan0端口
    system(WLAN_START_UP_COMMAND);
    sleep(2);

    //3、启动无线网卡管理程序wpa_supplicant
    system(WLAN_MANAGE_START);
    sleep(2);

    //4、扫描无线网络
    system(WLAN_SCAN_COMMAND);
    sleep(3);

    //5、显示出WiFi信息
    test_flag = wlan_exec(WLAN_SCAN_RESULT,SCAN_RESULT_FILE);
    if(test_flag<0)
    {
        goto fail;
        //return -1;
    }

    printf("===================Wlan test success ======================\n");
    system("ifconfig wlan0 down");
    system("busybox killall wpa_supplicant");      //关闭无线管理程序wpa_supplicant
    return (void*)test_flag;
    fail:
        printf("===================Wlan test failed======================\n");
        system("ifconfig wlan0 down");
        system("busybox killall wpa_supplicant");      //关闭无线管理程序wpa_supplicant
        return (void*)test_flag;
}

int main(int argc, char *argv[])
{
    int test_flag = 0,err_code = 0;
    char buf[COMMAND_VALUESIZE] = "wlan_test";
    char result[COMMAND_VALUESIZE] = RESULT_PASS;
    test_flag = (int)wlan_test(argv[0]);
    if(test_flag < 0)
    {
        strcpy(result,RESULT_FAIL);
        err_code = WLAN_PROC_ERR;
    }
    send_msg_to_server(buf, result, err_code);
}
