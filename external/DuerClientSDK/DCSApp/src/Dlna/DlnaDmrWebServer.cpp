////
// Created by eddy on 18-1-30.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "Dlna/upnp/upnptools.h"
#include "Dlna/upnp/upnp.h"
#include "Dlna/upnp/ithread.h"
#include "Dlna/DlnaDmrLogging.h"
#include "Dlna/DlnaDmrWebServer.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
static struct virtual_file* virtual_files = nullptr;

int WebServer::webserver_register_buf(const char* path, const char* contents,
                                      const char* content_type) {
    struct virtual_file* entry;

    dbg("Provide %s (%s) from buffer",
                                     path, content_type);

    assert(path != NULL);
    assert(contents != NULL);
    assert(content_type != NULL);

    entry = (struct virtual_file*) malloc(sizeof(struct virtual_file));

    if (entry == NULL) {
        return -1;
    }

    entry->len = strlen(contents);
    entry->contents = contents;
    entry->virtual_fname = path;
    entry->content_type = content_type;
    entry->next = virtual_files;
    virtual_files = entry;

    return 0;
}

#define PKG_DATADIR ""

int WebServer::webserver_register_file(const char* path, const char* content_type) {
    char local_fname[512] = {0};
    struct stat buf;
    struct virtual_file* entry;
    int rc = 0;

    snprintf(local_fname, sizeof(local_fname), "%s%s", PKG_DATADIR,
             strrchr(path, '/'));

    dbg("Provide %s (%s) from %s", path, content_type,
                                     local_fname);

    rc = stat(local_fname, &buf);

    if (rc) {
        err("Could not stat '%s': %s",
                                          local_fname, strerror(errno));
        return -1;
    }

    entry = (struct virtual_file*) malloc(sizeof(struct virtual_file));

    if (entry == NULL) {
        return -1;
    }

    if (buf.st_size) {
        char* cbuf = NULL;
        FILE* in = NULL;
        in = fopen(local_fname, "r");

        if (in == NULL) {
            free(entry);
            return -1;
        }

        cbuf = (char*) malloc(buf.st_size);

        if (cbuf == NULL) {
            free(entry);
            fclose(in);
            return -1;
        }

        if (fread(cbuf, buf.st_size, 1, in) != 1) {
            free(entry);
            free(cbuf);
            fclose(in);
            return -1;
        }

        fclose(in);
        entry->len = buf.st_size;
        entry->contents = cbuf;

    } else {
        entry->len = 0;
        entry->contents = NULL;
    }

    entry->virtual_fname = path;
    entry->content_type = content_type;
    entry->next = virtual_files;
    virtual_files = entry;

    return 0;
}

static int webserver_get_info(const char* filename, struct File_Info* info) {
    struct virtual_file* virtfile = virtual_files;

    while (virtfile != NULL) {
        if (strcmp(filename, virtfile->virtual_fname) == 0) {
            info->file_length = virtfile->len;
            info->last_modified = 0;
            info->is_directory = 0;
            info->is_readable = 1;
            info->content_type =
                ixmlCloneDOMString(virtfile->content_type);
            dbg("Access %s (%s) len=%zd",
                                             filename, info->content_type, virtfile->len);
            return 0;
        }

        virtfile = virtfile->next;
    }

    dbg("404 Not found. (attempt to access "
                                     "non-existent '%s')", filename);

    return -1;
}

static UpnpWebFileHandle webserver_open(const char* filename, enum UpnpOpenFileMode mode) {
    if (mode != UPNP_READ) {
        err("%s: ignoring request to open file for writing.", filename);
        return NULL;
    }

    for (struct virtual_file* vf = virtual_files; vf; vf = vf->next) {
        if (strcmp(filename, vf->virtual_fname) == 0) {
            WebServerFile* file = (WebServerFile*) malloc(sizeof(WebServerFile));
            file->pos = 0;
            file->len = vf->len;
            file->contents = vf->contents;
            return file;
        }
    }

    return NULL;
}

static inline int minimum(int a, int b) {
    return (a < b) ? a : b;
}

static int webserver_read(UpnpWebFileHandle fh, char* buf, size_t buflen) {
    WebServerFile* file = (WebServerFile*) fh;
    ssize_t len = -1;

    len = minimum(buflen, file->len - file->pos);
    memcpy(buf, file->contents + file->pos, len);

    if (len < 0) {
        err("In %s: %s",
                                          __FUNCTION__, strerror(errno));

    } else {
        file->pos += len;
    }

    return len;
}

static int webserver_write(UpnpWebFileHandle fh, char* buf, size_t buflen) {
    return -1;
}

static int webserver_seek(UpnpWebFileHandle fh, off_t offset, int origin) {
    WebServerFile* file = (WebServerFile*) fh;
    off_t newpos = -1;

    switch (origin) {
    case SEEK_SET:
        newpos = offset;
        break;

    case SEEK_CUR:
        newpos = file->pos + offset;
        break;

    case SEEK_END:
        newpos = file->len + offset;
        break;
    }

    if (newpos < 0 || newpos > (off_t) file->len) {
        err("in %s: seek failed with %s",
                                          __FUNCTION__, strerror(errno));
        return -1;
    }

    file->pos = newpos;
    return 0;
}

static int webserver_close(UpnpWebFileHandle fh) {
    WebServerFile* file = (WebServerFile*) fh;

    free(file);

    return 0;
}

#if (UPNP_VERSION < 10607)
static struct UpnpVirtualDirCallbacks virtual_dir_callbacks = {
    webserver_get_info,
    webserver_open,
    webserver_read,
    webserver_write,
    webserver_seek,
    webserver_close
};

bool WebServer::webserver_register_callbacks(void) {
    int rc = UpnpSetVirtualDirCallbacks(&virtual_dir_callbacks);

    if (UPNP_E_SUCCESS != rc) {
        err("UpnpSetVirtualDirCallbacks() Error: %s (%d)",
                  UpnpGetErrorMessage(rc), rc);
        return FALSE;
    }

    return TRUE;
}

#else
bool WebServer::webserver_register_callbacks(void) {
    bool result =
        (UpnpVirtualDir_set_GetInfoCallback(webserver_get_info) == UPNP_E_SUCCESS
         && UpnpVirtualDir_set_OpenCallback(webserver_open) == UPNP_E_SUCCESS
         && UpnpVirtualDir_set_ReadCallback(webserver_read) == UPNP_E_SUCCESS
         && UpnpVirtualDir_set_WriteCallback(webserver_write) == UPNP_E_SUCCESS
         && UpnpVirtualDir_set_SeekCallback(webserver_seek) == UPNP_E_SUCCESS
         && UpnpVirtualDir_set_CloseCallback(webserver_close) == UPNP_E_SUCCESS);
    return result;
}
#endif

}
}