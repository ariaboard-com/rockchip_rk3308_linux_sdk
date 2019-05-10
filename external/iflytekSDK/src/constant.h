#ifndef CONSTANT_VALUE_H_
#define CONSTANT_VALUE_H_

#include <string>

using namespace std;

class Constant
{
public:
    static const char* kCaeOutPcmPath;
    static const char* kCaeLibPATH;
    static const char* kIwvResPath;
    static const char* kMSCLoginParams;
    static const char* kIatSessionParams;
    static const char* kTTSsessionParams;
    static const char* kJsonQuestionNotFound;
    static const char* kMSCVadPrompt;
    static const char* kCaeWakeupWorkds;
	static const char* kWifiConnected;
    static const char* kJsonNotLink;
    static const char* kJsonNotPreChapter;
    static const char* kJsonNotNextChapter;
};

#endif // CONSTANT_VALUE_H_
