#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>
#include <libdrm_macros.h>
#include "drm_display.h"
#define MAX_FB    3

struct crtc {
    drmModeCrtc *crtc;
    drmModeObjectProperties *props;
    drmModePropertyRes **props_info;
    drmModeModeInfo *mode;
};

struct encoder {
    drmModeEncoder *encoder;
};

struct connector {
    drmModeConnector *connector;
    drmModeObjectProperties *props;
    drmModePropertyRes **props_info;
};

struct fb {
    drmModeFB *fb;
};

struct plane {
    drmModePlane *plane;
    drmModeObjectProperties *props;
    drmModePropertyRes **props_info;
};

struct resources {
    drmModeRes *res;
    drmModePlaneRes *plane_res;

    struct crtc *crtcs;
    struct encoder *encoders;
    struct connector *connectors;
    struct fb *fbs;
    struct plane *planes;
};

struct device {
    int fd;

    struct resources *resources;

    struct {
        unsigned int width;
        unsigned int height;

        //unsigned int fb_id[MAX_FB];
        struct bo *bo[MAX_FB];
        int current;
        int fb_num;
        int bpp;
    } mode;
};

struct device *pdev;
static void free_resources(struct resources *res)
{
    int i;

    if (!res)
        return;

#define free_resource(_res, __res, type, Type)                  \
    do {                                    \
        if (!(_res)->type##s)                       \
            break;                          \
        for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) { \
            if (!(_res)->type##s[i].type)               \
                break;                      \
            drmModeFree##Type((_res)->type##s[i].type);     \
        }                               \
        free((_res)->type##s);                      \
    } while (0)

#define free_properties(_res, __res, type)                  \
    do {                                    \
        for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) { \
            drmModeFreeObjectProperties(res->type##s[i].props); \
            free(res->type##s[i].props_info);           \
        }                               \
    } while (0)

    if (res->res) {
        free_properties(res, res, crtc);

        free_resource(res, res, crtc, Crtc);
        free_resource(res, res, encoder, Encoder);

        free_resource(res, res, connector, Connector);
        free_resource(res, res, fb, FB);

        drmModeFreeResources(res->res);
    }

    if (res->plane_res) {
        free_properties(res, plane_res, plane);

        free_resource(res, plane_res, plane, Plane);

        drmModeFreePlaneResources(res->plane_res);
    }

    free(res);
}

static struct resources *get_resources(struct device *dev)
{
    struct resources *res;
    int i;

    res = calloc(1, sizeof(*res));
    if (res == 0)
        return NULL;

    drmSetClientCap(dev->fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);

    res->res = drmModeGetResources(dev->fd);
    if (!res->res) {
        fprintf(stderr, "drmModeGetResources failed: %s\n",
                strerror(errno));
        goto error;
    }

    res->crtcs = calloc(res->res->count_crtcs, sizeof(*res->crtcs));
    res->encoders = calloc(res->res->count_encoders, sizeof(*res->encoders));
    res->connectors = calloc(res->res->count_connectors, sizeof(*res->connectors));
    res->fbs = calloc(res->res->count_fbs, sizeof(*res->fbs));

    if (!res->crtcs || !res->encoders || !res->connectors || !res->fbs)
        goto error;

#define get_resource(_res, __res, type, Type)                   \
    do {                                    \
        for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) { \
            (_res)->type##s[i].type =               \
                drmModeGet##Type(dev->fd, (_res)->__res->type##s[i]); \
            if (!(_res)->type##s[i].type)               \
                fprintf(stderr, "could not get %s %i: %s\n",    \
                    #type, (_res)->__res->type##s[i],   \
                    strerror(errno));           \
        }                               \
    } while (0)

    get_resource(res, res, crtc, Crtc);
    get_resource(res, res, encoder, Encoder);
    get_resource(res, res, connector, Connector);
    get_resource(res, res, fb, FB);

    /* Set the name of all connectors based on the type name and the per-type ID. */
    for (i = 0; i < res->res->count_connectors; i++) {
        struct connector *connector = &res->connectors[i];
        drmModeConnector *conn = connector->connector;
    }

#define get_properties(_res, __res, type, Type)                 \
    do {                                    \
        for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) { \
            struct type *obj = &res->type##s[i];            \
            unsigned int j;                     \
            obj->props =                        \
                drmModeObjectGetProperties(dev->fd, obj->type->type##_id, \
                               DRM_MODE_OBJECT_##Type); \
            if (!obj->props) {                  \
                fprintf(stderr,                 \
                    "could not get %s %i properties: %s\n", \
                    #type, obj->type->type##_id,        \
                    strerror(errno));           \
                continue;                   \
            }                           \
            obj->props_info = calloc(obj->props->count_props,   \
                         sizeof(*obj->props_info)); \
            if (!obj->props_info)                   \
                continue;                   \
            for (j = 0; j < obj->props->count_props; ++j)       \
                obj->props_info[j] =                \
                    drmModeGetProperty(dev->fd, obj->props->props[j]); \
        }                               \
    } while (0)

    get_properties(res, res, crtc, CRTC);
    get_properties(res, res, connector, CONNECTOR);

    for (i = 0; i < res->res->count_crtcs; ++i)
        res->crtcs[i].mode = &res->crtcs[i].crtc->mode;

    res->plane_res = drmModeGetPlaneResources(dev->fd);
    if (!res->plane_res) {
        fprintf(stderr, "drmModeGetPlaneResources failed: %s\n",
                strerror(errno));
        return res;
    }

    res->planes = calloc(res->plane_res->count_planes, sizeof(*res->planes));
    if (!res->planes)
        goto error;

    get_resource(res, plane_res, plane, Plane);
    get_properties(res, plane_res, plane, PLANE);

    return res;

error:
    free_resources(res);
    return NULL;
}

static struct bo *
bo_create_dumb(int fd, unsigned int width, unsigned int height, unsigned int bpp)
{
    struct drm_mode_create_dumb arg;
    struct bo *bo;
    int ret;

    bo = calloc(1, sizeof(struct bo));
    if (bo == NULL) {
        fprintf(stderr, "failed to allocate buffer object\n");
        return NULL;
    }

    memset(&arg, 0, sizeof(arg));
    arg.bpp = bpp;
    arg.width = width;
    arg.height = height;

    ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &arg);
    if (ret) {
        fprintf(stderr, "failed to create dumb buffer: %s\n",
                strerror(errno));
        free(bo);
        return NULL;
    }

    bo->fd = fd;
    bo->handle = arg.handle;
    bo->size = arg.size;
    bo->pitch = arg.pitch;

    return bo;
}

static int bo_map(struct bo *bo)
{
    struct drm_mode_map_dumb arg;
    void *map;
    int ret;

    memset(&arg, 0, sizeof(arg));
    arg.handle = bo->handle;

    ret = drmIoctl(bo->fd, DRM_IOCTL_MODE_MAP_DUMB, &arg);
    if (ret)
        return ret;

    //drmMap(bo->fd, arg.offset, bo->size, &map);
    map = drm_mmap(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                   bo->fd, arg.offset);

    if (map == MAP_FAILED)
        return -EINVAL;

    bo->ptr = map;

    return 0;
}

static void bo_unmap(struct bo *bo)
{
    if (!bo->ptr)
        return;

    drmUnmap(bo->ptr, bo->size);
    bo->ptr = NULL;
}

void bo_destroy(struct bo *bo)
{
    struct drm_mode_destroy_dumb arg;
    int ret;

    memset(&arg, 0, sizeof(arg));
    arg.handle = bo->handle;

    ret = drmIoctl(bo->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);
    if (ret)
        fprintf(stderr, "failed to destroy dumb buffer: %s\n",
                strerror(errno));

    free(bo);
}

static void buffer_init(struct device *dev, int num, int bpp)
{
    uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};
    unsigned int fb_id;
    struct bo *bo;
    unsigned int i;
    unsigned int j;
    int ret, x = 0;

    dev->mode.width = 0;
    dev->mode.height = 0;

    dev->mode.fb_num = num;
    dev->mode.bpp = bpp;
    dev->mode.current = 0;
    dev->mode.width = dev->resources->crtcs[0].mode->hdisplay;
    dev->mode.height = dev->resources->crtcs[0].mode->vdisplay;

    for (i = 0; i < dev->mode.fb_num; i++) {
        bo = bo_create_dumb(dev->fd, dev->mode.width, dev->mode.height, bpp);
        if (!bo)
            return;

        ret = bo_map(bo);
        if (ret) {
            fprintf(stderr, "failed to map buffer: %s\n",
                    strerror(-errno));
            bo_destroy(bo);
            return;
        }

        dev->mode.bo[i] = bo;
        offsets[0] = 0;
        handles[0] = bo->handle;
        pitches[0] = bo->pitch;
        if (bpp == 32)
            ret = drmModeAddFB2(dev->fd, dev->mode.width, dev->mode.height,
                                DRM_FORMAT_ABGR8888, handles, pitches, offsets, &fb_id, 0);
        else
            ret = drmModeAddFB2(dev->fd, dev->mode.width, dev->mode.height,
                                DRM_FORMAT_BGR565, handles, pitches, offsets, &fb_id, 0);
        if (ret) {
            fprintf(stderr, "failed to add fb (%ux%u): %s\n",
                    dev->mode.width, dev->mode.height, strerror(errno));
            return;
        }
        bo->fb_id = fb_id;
    }

    ret = drmModeSetCrtc(dev->fd, dev->resources->crtcs[0].crtc->crtc_id, dev->mode.bo[0]->fb_id,
                         x, 0, &dev->resources->connectors->connector->connector_id, 1,
                         dev->resources->crtcs[0].mode);
    if (ret) {
        fprintf(stderr, "failed to set mode: %s\n", strerror(errno));
        return;
    }
}

static void clear_mode(struct device *dev)
{
    int i;
    for (i = 0; i < dev->mode.fb_num; i++) {
        if (dev->mode.bo[i]) {
            drmModeRmFB(dev->fd, dev->mode.bo[i]->fb_id);
            bo_unmap(dev->mode.bo[i]);
            bo_destroy(dev->mode.bo[i]);
        }
    }
}

static int drm_open(void)
{
    int fd;

    fd = drmOpen("rockchip", NULL);
    if (fd < 0) {
        fprintf(stderr, "failed to open device 'rockchip': %s\n",
                strerror(errno));
        return -errno;
    }

    return fd;
}

int drm_setmode(int num, int bpp)
{
    if (num > MAX_FB) {
        printf("err: fb num = %d > %d%s\n", __func__, num, MAX_FB);
        return -errno;
    }

    if ((num != pdev->mode.fb_num) || (bpp != pdev->mode.bpp)) {
        clear_mode(pdev);
        buffer_init(pdev, num, bpp);
    }
}

int drm_init(int num, int bpp)
{
    if (num > MAX_FB) {
        printf("err: fb num = %d > %d%s\n", __func__, num, MAX_FB);
        return -errno;
    }

    if (pdev == NULL) {
        pdev = calloc(1, sizeof(struct device));
        if (pdev == NULL) {
            fprintf(stderr, "failed to allocate buffer object\n");
            return 0;
        }
    }
    pdev->fd = drm_open();
    if (pdev->fd < 0)
        return -errno;

    pdev->resources = get_resources(pdev);
    if (!pdev->resources) {
        drmClose(pdev->fd);
        return -errno;
    }

    buffer_init(pdev, num, bpp);

    return 0;
}

int drm_deinit(void)
{
    struct device* dev = pdev;
    clear_mode(dev);
    free_resources(dev->resources);
    drmClose(dev->fd);
    dev->fd = 0;
    free(pdev);
    pdev = NULL;

    return 0;
}

char * getdrmdispbuff(void)
{
    if (pdev->mode.bo)
        return pdev->mode.bo[0]->ptr;
    else
        return NULL;
}

int getdrmdispinfo(struct bo *bo, int *w, int *h)
{
    if (pdev->mode.bo[0]) {
        memcpy(bo, pdev->mode.bo[0], sizeof(struct bo));
        *w = pdev->mode.width;
        *h = pdev->mode.height;
    }

    return 0;
}

struct bo *getdrmdisp(void)
{
    pdev->mode.current ++;
    if (pdev->mode.current >= MAX_FB || pdev->mode.current >= pdev->mode.fb_num)
        pdev->mode.current = 0;

    return pdev->mode.bo[pdev->mode.current];
}

void setdrmdisp(struct bo *bo)
{
    int ret = 0;
    struct device* dev = pdev;

    if (dev == NULL)
        return;

    ret = drmModeSetCrtc(dev->fd, dev->resources->crtcs[0].crtc->crtc_id, bo->fb_id,
                         0, 0, &dev->resources->connectors->connector->connector_id, 1,
                         dev->resources->crtcs[0].mode);
    if (ret) {
        fprintf(stderr, "failed to set mode: %s\n", strerror(errno));
        return;
    }
}

#if 0
int main(int argc, char **argv)
{
    memset(&sdev, 0, sizeof sdev);
    printf("drm_display run\n");
    drm_init(&sdev, 1, 32);
    memset(getdrmdispbuff(), 0xff, 320 * 480);
    sleep(10);
    drm_deinit(&sdev);
    printf("drm_display exit\n");

    return 0;
}
#endif