#ifndef CSD_STDIO_H_INCLUDED
#define CSD_STDIO_H_INCLUDED

#include <stdio.h>

typedef struct {
    int fd;             /* file descriptor */
    char *buf_beg;       /* starting buffer address */
    size_t size;        /* buffer size */
    size_t count;       /* number of bytes in the buffer */
    char *buf_pos;      /* current buffer address */
    off_t offset;       /* file offset */
    /* .... */
} CSD_FILE;


extern CSD_FILE *csd_fopen(const char *path, const char *mode);
extern int csd_fgetc(CSD_FILE *f);
extern int csd_fputc(int ch, CSD_FILE *f);
extern int csd_fflush(FILE *f);
extern int csd_fclose(CSD_FILE *f);
extern int csd_fseek(FILE *stream, long offset, int whence);



































#endif // CSD_STDIO_H_INCLUDED
