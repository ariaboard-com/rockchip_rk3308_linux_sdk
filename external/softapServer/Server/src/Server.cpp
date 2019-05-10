#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>

#include "Server.h"
#include "WifiUtil.h"
#include "Shell.h"

const char *STATUS_FILE = "/tmp/softap";
static char HTTP_RESPOSE_MESSAGE[] = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:%d\r\n\r\n%s";
static char HTTP_OK[] = "OK";

static WifiUtil *m_wifiUtil;
static int serverSockfd = -1;

int save_file(const char *path, const char *contant)
{
    FILE* file = fopen(path, "w+");
    if(!file) {
        log_err("open file:%s error!\n");
        return -1;
    }
    fwrite(contant, sizeof(char), strlen(contant), file);
    fclose(file);

    return 0;
}

int read_file(const char *path, char *result)
{
    char szTest[1000] = {0};
    int len = 0;

    FILE *fp = fopen(path, "r");
    if(NULL == fp) {
        //log_err("failed to open %s.\n", path);
        return -1;
    }

    while(!feof(fp)) {
        memset(szTest, 0, sizeof(szTest));
        fgets(szTest, sizeof(szTest) - 1, fp); // 包含了\n
        strcat(result, szTest);
    }

    fclose(fp);
    return 0;
}


static void sig_child_handle(int sig){
    log_info("Application exit.");
    if(serverSockfd != -1){
        close(serverSockfd);

        char cmd[24];
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "softapDemo stop");
        system(cmd);
        system("ifconfig wlan1 down");

        char contant[128];
        snprintf(contant, sizeof(contant), "AP:OFF\nSERVER:NOT RUNNING\n");
        save_file(STATUS_FILE, contant);
    }
}

int initSocketServer(){
    int val = 1;
    int server_sockfd;
    struct sockaddr_in server_addr;
    int ret;

    /* create a socket */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        log_err("socket error:%s\n", strerror(errno));
        return -1;
    }

    ret = setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(int));
    if (ret == -1) {
        log_err("setsockopt error:%s\n", strerror(errno));
        return -1;
    }

    /*  initialize server address */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SOCKET_PORT);

    /* bind with the local file */
    ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret) {
        log_err("bind error:%s\n", strerror(errno));
        close(server_sockfd);
        return -1;
    }

    /* listen */
    ret = listen(server_sockfd, TCP_QUEUE_LINE);
    if (ret) {
        log_err("listen error:%s\n", strerror(errno));
        close(server_sockfd);
        return -1;
    }
    log_info("tcp server is ready ... \n");

    return server_sockfd;
}

bool sendWifiList(int sockfd){
    int ret;
    char respose_message[MSG_BUFF_LEN] = {0};

    /* rapidjson change into json string.*/
    std::string json_result = m_wifiUtil->getWifiListJson();
    // log_info("json_result: %s \n",json_result.c_str());

    memset(respose_message, 0, sizeof(respose_message));
    sprintf(respose_message,HTTP_RESPOSE_MESSAGE,strlen(json_result.c_str()),json_result.c_str());

    ret = send(sockfd,respose_message,sizeof(respose_message),0);
    if(ret < 0){
        log_err("Send wifi info failed. \n");
        return false;
    }
    return true;
}

bool deviceWifiSetUp(int sockfd,char recv_buf[]){
    int ret;
    char respose_message[MSG_BUFF_LEN] = {0};

    memset(respose_message, 0, sizeof(respose_message));
    sprintf(respose_message,HTTP_RESPOSE_MESSAGE,strlen(HTTP_OK),HTTP_OK);

    ret = send(sockfd,respose_message,sizeof(respose_message),0);
    if(ret < 0){
        log_err("Send wifi info failed. \n");
        return false;
    }
    m_wifiUtil->WifiSetUp(recv_buf);
    return true;
}

    /*
     * used for process client request.
     * @parm client_sockfd
     */
void handleClientProcess(int client_sockfd){
    int recv_num;
    char recv_buf[RECV_BUFFER_SIZE] = {0};

    recv_num = recv(client_sockfd, recv_buf, sizeof(recv_buf), 0);
    if(recv_num <= 0) {
        log_err("recv error:%s\n", strerror(errno));
        return;
    }
    recv_buf[recv_num]='\0';
    // log_info("recv_buf: %s, recv_num: %d \n",recv_buf,recv_num);
    if (strstr(recv_buf,REQUEST_WIFI_LIST)){
        log_info("Come wifi list requeset from client. \n");
        sendWifiList(client_sockfd);
    } else if (strstr(recv_buf,REQUEST_WIFI_SET_UP) ){
        log_info("Come wifi setUp requeset from client. \n");
        deviceWifiSetUp(client_sockfd,recv_buf);
    } else if(strstr(recv_buf,REQUEST_COMPANION_INFO)) {
        log_info("Come companion info requeset from client. \n");
        //handleRequestMessage(client_sockfd,recv_buf);
    } else if(strstr(recv_buf,REQUEST_DEVICE_CONTEXT)) {
        log_info("Come device context requeset from client. \n");
    }
    log_info("Close client sockfd. \n\n");
    close(client_sockfd);
}

int start(const char* ap_name){
    int server_sockfd;
    int client_sockfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pid_t pid;

    char cmd[124];
    system("gst-play-1.0 -q --no-interactive /usr/ap_notification/start_softap.mp3 &");
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "softapDemo %s", ap_name);
    system(cmd);

    signal(SIGINT,sig_child_handle);
    /* used for wifi operation */
    m_wifiUtil = new WifiUtil();
    /* used for handler companion information */

    server_sockfd = initSocketServer();
    if (server_sockfd < 0) {
        log_err("tcp server init fail\n");

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "softapDemo stop");
        system(cmd);
        system("ifconfig wlan1 down");

        char contant[128];
        snprintf(contant, sizeof(contant), "AP:OFF\nSERVER:NOT RUNNING\n");
        save_file(STATUS_FILE, contant);

        return -1;
    }

    char contant[128];
    snprintf(contant, sizeof(contant), "AP:ON\nSERVER:RUNNING\nAP_NAME:%s\n", ap_name);
    save_file(STATUS_FILE, contant);
    serverSockfd = server_sockfd;


    /* accept a connection all time*/
    while(1){
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sockfd < 0) {
            log_err("accept error:%s\n", strerror(errno));
            close(server_sockfd);
            return -1;
        }

        log_info("accept a new client, ip:%s, port:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        handleClientProcess(client_sockfd);
    }
    close(server_sockfd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "softapDemo stop");
    system(cmd);
    system("ifconfig wlan1 down");

    memset(contant, 0, sizeof(contant));
    snprintf(contant, sizeof(contant), "AP:OFF\nSERVER:NOT RUNNING\n");
    save_file(STATUS_FILE, contant);

    return 0;
}

int main(int argc, char *argv[]){
    //system("gst-play-1.0 -q --no-interactive /usr/ap_notification/start_softap.mp3 &");
    if (argc > 1) {
        if (argc > 2) {
            if (0 != strcmp(argv[1], "start")) {
                log_info("Wrong parameters!\n");
            } else {
                start(argv[2]);
            }
        } else {
            if (0 == strcmp(argv[1], "start")) {
                start("Rockchip_AP");
            } else if (0 == strcmp(argv[1], "stop")) {
                char cmd[48];
                char cmdResult[24];

                memset(cmd, 0, sizeof(cmd));
                memset(cmdResult, 0, sizeof(cmdResult));

                snprintf(cmd, sizeof(cmd), "pidof softapServer");
                bool res = exec(cmd, cmdResult);
                log_info("getpid cmdResult:%s. self:%d.\n", cmdResult, getpid());

                if (res && strlen(cmdResult) > 0) {
                    int pid = 0;
                    if (strstr(cmdResult, " ")) {
                        char *s = strtok(cmdResult, " ");

                        while(s) {
                            pid = atoi(s);

                            if (pid != getpid()) {
                                /*memset(cmd, 0, sizeof(cmd));
                                snprintf(cmd, sizeof(cmd), "kill -9 %d", pid);
                                log_info("kill pid %d\n", pid);
                                system(cmd);*/
                                kill(pid, SIGINT);
                            }

                            s = strtok(NULL, " ");
                        }
                    } else {
                        pid = atoi(cmdResult);
                        if (pid != getpid()) {
                            /*memset(cmd, 0, sizeof(cmd));
                            snprintf(cmd, sizeof(cmd), "kill -9 %d", pid);
                            log_info("kill pid %d\n", pid);
                            system(cmd);*/
                            kill(pid, SIGINT);
                        }
                    }
                } else {

                }
            } else if (0 == strcmp(argv[1], "status")) {
                char contant[128];
                if (0 == read_file(STATUS_FILE, contant)) {
                    printf("%s", contant);
                } else {
                    printf("AP:OFF\nSERVER:NOT RUNNING\n");
                }
            } else {
                start(argv[1]);
            }
        }
    } else {
        log_info("Please running with ap name at least!");
    }

    return 0;
}
