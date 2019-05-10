#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "easy_setup.h"
#include "Socket.h"
#include "shell.h"

#define WIFI_INFO_FORMAT \
"\
ctrl_interface=/var/run/wpa_supplicant\n\
ap_scan=1\n\
\n\
network={\n\
    ssid=\"%s\"\n\
    psk=\"%s\"\n\
    priority=1\n\
}\
"

int killed = 0;
int debug_enable = 0;

void usage() {
    printf("-h: show help message\n");
    printf("-d: show debug message\n");
    printf("-k <v>: set 16-char key for all protocols\n");
    printf("-p <v>: bitmask of protocols to enable\n");
    printf("  0x%04x - bcast\n", 1<<EASY_SETUP_PROTO_BCAST);
    printf("  0x%04x - neeze\n", 1<<EASY_SETUP_PROTO_NEEZE);
    printf("  0x%04x - Air Kiss\n", 1<<EASY_SETUP_PROTO_AKISS);
    printf("  0x%04x - Xiaoyi\n", 1<<EASY_SETUP_PROTO_XIAOYI);
    printf("  0x%04x - changhong\n", 1<<EASY_SETUP_PROTO_CHANGHONG);
    printf("  0x%04x - jingdong\n", 1<<EASY_SETUP_PROTO_JINGDONG);
    printf("  0x%04x - jd JoyLink\n", 1<<EASY_SETUP_PROTO_JD);
}

static void signal_handler(int sig) {
    printf("aborted\n");
    killed = 1;

    easy_setup_stop();
    system("ifconfig wlan0 up");
    system("wpa_supplicant -B -i wlan0 -c /data/cfg/wpa_supplicant.conf");
    exit(0);
}

int main(int argc, char* argv[])
{
    int ret;
    int len;
    int WifiOpenOK = 0;
    uint16 val;

    char gssid[33];
    char gpassword[65];
    FILE *fd, *fds;
    char commset[]={"update_config=1\nctrl_interface=%s\neapol_version=1\nfast_reauth=1\n"};
    char WPAstr[]={"\nnetwork={\n\tssid=\"%s\"\n\tpsk=\"%s\"\n\tkey_mgmt=WPA-PSK\n}\n"};
    char CmdStr[2048];
    char send_ip[16]="hello\0"; /* ipv4 max length */

    int i=1;
    if (argc > 1) {
        printf("argc:%d,argv:\n",argc);
        for (; i<argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
    }

    for (;;) {
        int c = getopt(argc, argv, "dhk:p:");
        if (c < 0) {
            break;
        }

        printf("getopt c:%d\n", c);
        switch (c) {
            case 'd':
                debug_enable = 1;
                break;
            case 'k':
                bcast_set_key(optarg);
                bcast_set_key_qqcon(optarg);
                neeze_set_key(optarg);
                neeze_set_key_qqcon(optarg);
                akiss_set_key(optarg);
                jingdong_set_key(optarg);
                jd_set_key(optarg);
                break;
            case 'p':
                sscanf(optarg, "%04x", (uint32*)&val);
                easy_setup_enable_protocols(val);
                break;
            case 'h':
                usage();
                return 0;
            default:
                usage();
                return 0;
        }
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while (1) {
        memset(CmdStr, 0, sizeof(CmdStr));
        exec("pidof wpa_supplicant", CmdStr);
        printf("pidof wpa_supplicant result:%s\n", CmdStr);
        if (strlen(CmdStr) > 0) {
            _kill(CmdStr);
        }

        system("ifconfig wlan0 down");
        system("ip link set wlan0 up");
        ret = easy_setup_start();
        if (ret) return ret;

        /* query for result, blocks until bcast comes or times out */
        for (;;) {
            ret = easy_setup_query();
            if (!ret) {
                break;
            }
        }
 
        if (!ret) {
            char ssid[33]; /* ssid of 32-char length, plus trailing '\0' */
            ret = easy_setup_get_ssid(ssid, sizeof(ssid));
            if (!ret) {
                printf("ssid: %s\n", ssid);
                memcpy(gssid, ssid, sizeof(gssid));
                printf("gssid: %s\n", gssid);
            }

            char password[65]; /* password is 64-char length, plus trailing '\0' */
            ret = easy_setup_get_password(password, sizeof(password));
            if (!ret) {
                printf("password: %s\n", password);
                memcpy(gpassword, password, sizeof(password));
                printf("gpassword: %s\n", gpassword);
            }

            uint8 protocol;
            ret = easy_setup_get_protocol(&protocol);
            if (ret) {
                printf("failed getting protocol.\n");
            } else if (protocol == EASY_SETUP_PROTO_BCAST) {
                char ip[16]; /* ipv4 max length */
                ret = bcast_get_sender_ip(ip, sizeof(ip));
                if (!ret) {
                    printf("EASY_SETUP_PROTO_BCAST sender ip: %s\n", ip);
                }
                memcpy(send_ip,ip,sizeof(ip));
                uint16 port;
                ret = bcast_get_sender_port(&port);
                if (!ret) {
                    printf("EASY_SETUP_PROTO_BCAST sender port: %d\n", port);
                }
            } else if (protocol == EASY_SETUP_PROTO_NEEZE) {
                char ip[16]; /* ipv4 max length */
                ret = neeze_get_sender_ip(ip, sizeof(ip));
                if (!ret) {
                    printf("EASY_SETUP_PROTO_NEEZE sender ip: %s\n", ip);
                }

                uint16 port;
                ret = neeze_get_sender_port(&port);
                if (!ret) {
                    printf("EASY_SETUP_PROTO_NEEZE sender port: %d\n", port);
                }
            } else if (protocol == EASY_SETUP_PROTO_AKISS) {
                uint8 random;
                ret = akiss_get_random(&random);
                if (!ret) {
                    printf("random: 0x%02x\n", random);
                }
            } else if (protocol == EASY_SETUP_PROTO_CHANGHONG) {
                uint8 sec;
                ret = changhong_get_sec_mode(&sec);
                if (!ret) {
                    printf("sec mode: 0x%02x\n", sec);
                }
            } else if (protocol == EASY_SETUP_PROTO_JINGDONG) {
                uint8 sec;
                ret = jingdong_get_sec_mode(&sec);
                if (!ret) {
                    printf("sec mode: 0x%02x\n", sec);
                }
            } else if (protocol == EASY_SETUP_PROTO_JD) {
                uint8 crc;
                ret = jd_get_crc(&crc);
                if (!ret) {
                    printf("crc: 0x%02x\n", crc);
                }

                uint32 ip;
                ret = jd_get_ip(&ip);
                if (!ret) {
                    printf("ip: 0x%08x\n", ip);
                }

                uint16 port;
                ret = jd_get_port(&port);
                if (!ret) {
                    printf("port: 0x%04x\n", port);
                }
            } else if (protocol == EASY_SETUP_PROTO_XIAOYI) {
                uint8 buf[128];
                uint8 len = sizeof(buf);
                ret = xiaoyi_get_buffer(&len, buf);
                if (!ret) {
                    printf("buf(%d) - ", len);
                    int i;
                    for (i=0; i<len; i++) printf("%02x ", buf[i]);
                    printf("\n");
                }
            }

            #if 0
            /* if easy_setup_get_security() returns -1, try it more times */
            int tries = 3;
            while (tries--) {
                ret = easy_setup_get_security();
                if (ret != -1) break;
            }
            printf("security: ");
            if (ret == WLAN_SECURITY_WPA2) printf("wpa2\n");
            else if (ret == WLAN_SECURITY_WPA) printf("wpa\n");
            else if (ret == WLAN_SECURITY_WEP) printf("wep\n");
            else if (ret == WLAN_SECURITY_NONE) printf("none\n");
            else printf("wpa2");
            #endif

            /*system("echo 'fopen /data/wpa_cfg ... ...'");
            fd=fopen("/data/wpa_cfg", "w+");
            if( fd == NULL ) {
                printf("file open can not create file !!! \n");
                return -ENOENT;
            }

            sprintf(CmdStr, commset, "wlan0");
            fprintf(fd,"%s", CmdStr);

            sprintf(CmdStr, WPAstr, gssid, gpassword);
            fprintf(fd,"%s", CmdStr);

            fclose(fd);*/

            //sprintf(CmdStr, "cat %s > /data/misc/wifi/wpa_supplicant.conf", "/data/wpa_cfg");
            //system(CmdStr);

            //system("rm /etc/wpa_supplicant.conf");
            fds=fopen("/data/cfg/wpa_supplicant.conf", "wt");
            if( fds == NULL ) {
                printf("file open can not create file !!! \n");
                return -ENOENT;
            }
            memset(CmdStr, 0, sizeof(CmdStr));
            snprintf(CmdStr, sizeof(CmdStr), WIFI_INFO_FORMAT, gssid, gpassword);
            fprintf(fds,"%s", CmdStr);
            fclose(fds);

            system("sync");
            system("echo 'cat /data/cfg/wpa_supplicant.conf'");
            system("cat /data/cfg/wpa_supplicant.conf");
            system("chmod 660 /data/cfg/wpa_supplicant.conf");
            WifiOpenOK = 1;
        }

        /* must do this! */
        easy_setup_stop();
        if(WifiOpenOK) {
            system("echo 'reboot wifi'");
            system("ifconfig wlan0 down");
            system("ifconfig wlan0 up");

            memset(CmdStr, 0, sizeof(CmdStr));
            exec("pidof wpa_supplicant", CmdStr);
            if (strlen(CmdStr) > 0) {
                _kill(CmdStr);
            }

            memset(CmdStr, 0, sizeof(CmdStr));
            exec("pidof udhcpc", CmdStr);
            if (strlen(CmdStr) > 0) {
                _kill(CmdStr);
            }

            memset(CmdStr, 0, sizeof(CmdStr));
            exec("pidof dhcpcd", CmdStr);
            if (strlen(CmdStr) > 0) {
                _kill(CmdStr);
            }

            system("wpa_supplicant -B -i wlan0 -c /data/cfg/wpa_supplicant.conf");

            i = 0;
            while (i < 15 && isWpaCompleted() != 0) {
                sleep(1);
                i++;
            }

            if (i < 15) {
                system("udhcpc -i wlan0");

                usleep(1000*1000);
                //system("rm /var/run/dhcpcd-wlan0.pid");
                //system("dhcpcd wlan0");
            } else {
                printf("Wifi connected timeout, please check wifi info...\n");
                continue;
            }
        }

        if (0 == socket_connect("127.0.0.1", 1221)) {
            socket_send(send_ip);
            socket_close();
        }

        //system("wpa_supplicant -i wlan0 -c /etc/wpa_supplicant.conf &");
        //system("svc wifi disable");
        //system("svc wifi enable");

        if (isWifiConnected() == 0) {
            printf("Wifi conneted success!");
            system("gst-play-1.0 -q --no-interactive /data/mode_sound/wifi_connected.mp3 &");
            break;
        }
    }

    return 0;
}
