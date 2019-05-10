////
// Created by eddy on 18-1-30.
//
#include <string.h>
#include <stdlib.h>
#include "Dlna/DlnaDmrXmlEscape.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
char* XMLEscape::xmlescape(const char* str, int attribute) {
    int len = 0;
    char* out = NULL;

    xmlescape_real(str, NULL, &len, attribute);
    out = (char*) malloc(len + 1);
    xmlescape_real(str, out, NULL, attribute);
    return out;
}

void XMLEscape::xmlescape_real(const char* str, char* target, int* length,
                               int attribute) {
    if (target != NULL) {
        int len = 0;

        for (/**/; *str; str++) {
            if (*str == '<') {
                memcpy(target + len, "&lt;", 4);
                len += 4;
            } else if (attribute && (*str == '"')) {
                memcpy(target + len, "%22", 3);
                len += 3;
            } else if (*str == '>') {
                memcpy(target + len, "&gt;", 4);
                len += 4;
            } else if (*str == '&') {
                memcpy(target + len, "&amp;", 5);
                len += 5;
            } else {
                target[len++] = *str;
            }
        }

        target[len] = '\0';

        if (length != NULL) {
            *length = len;
        }
    } else if (length != NULL) {
        int len = 0;

        for (/**/; *str; str++) {
            if (*str == '<') {
                len += 4;
            } else if (attribute && (*str == '"')) {
                len += 3;
            } else if (*str == '>') {
                len += 4;
            } else if (*str == '&') {
                len += 5;
            } else {
                len++;
            }
        }

        *length = len;
    }
}
}
}