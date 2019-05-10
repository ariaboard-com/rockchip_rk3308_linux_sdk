#ifndef __DRM_DISPLAY_H__
#define __DRM_DISPLAY_H__

struct bo {
    int fd;
    void *ptr;
    size_t size;
    size_t offset;
    size_t pitch;
    unsigned handle;
    int fb_id;
};

int drm_init(int num, int bpp);
int drm_deinit(void);
char * getdrmdispbuff(void);
int getdrmdispinfo(struct bo *bo, int *w, int *h);
struct bo *getdrmdisp(void);
void setdrmdisp(struct bo *bo);
int drm_setmode(int num, int bpp);
#endif
