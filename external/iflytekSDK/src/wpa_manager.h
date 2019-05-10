#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WPAManager
{
public:
    static WPAManager* getInstance();
    // ~WPAManager();

    bool isNetConnected();

private:
    WPAManager();

    static WPAManager *_instance;
};

#endif // WIFI_MANAGER_H
