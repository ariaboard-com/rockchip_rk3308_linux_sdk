#include "Shell.h"

bool exec(const char* cmd, char* result)
{
    FILE *stream = NULL;
    char buff[1024] = {0};

    if((stream = popen(cmd, "r")) != NULL) {
        while(fgets(buff, 1024, stream)) {
            strcat(result, buff);
        }
    } else {
        printf("open cmdline failed!\n");
        return false;
    }

    pclose(stream);

    return true;
}
