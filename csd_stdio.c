#include "csd_stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


/*
CSD_FILE *csd_fopen(const char *path, const char *mode);
int csd_fgetc(CSD_FILE *f);
int csd_fputc(int ch, CSD_FILE *f);
int csd_fflush(FILE *f);
int csd_fclose(CSD_FILE *f);
int fseek(FILE *stream, long offset, int whence);S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
*/
void exit_sys(const char *msg);
CSD_FILE *csd_fopen(const char *path, const char *mode)
{
    CSD_FILE *file = malloc(sizeof(CSD_FILE));
    if(!strcmp(mode,"r") || !strcmp(mode,"rb")){
        if(file->fd = open(path,O_RDONLY) == -1)
            exit_sys("open: ");

    }
    else if(!strcmp(mode,"w") || !strcmp(mode,"w+b")){
        if(file->fd = open(path,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1)
            exit_sys("open: ");
    }
    else if(!strcmp(mode,"a") || !strcmp(mode,"ab")){
        if(file->fd = open(path,O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1);
            exit_sys("open: ");

    }
    else if(!strcmp(mode,"r+") || !strcmp(mode,"rb+") || !strcmp(mode,"r+b")){
        file->fd = open(path,O_RDWR);
    }
    else if(!strcmp(mode,"w+") || !strcmp(mode,"wb+") || !strcmp(mode,"w+b")){
        file->fd = open(path,O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }
    else if(!strcmp(mode,"a+") || !strcmp(mode,"ab+") || !strcmp(mode,"a+b")){
        file->fd = open(path,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }
    else{
         fprintf(stderr,"%s is not available mode for 'csd_fopen'\n",mode);
         exit(EXIT_FAILURE);
    }
    file->buf_beg = malloc(512);
    file->size = 512;
    file->count = 0;
    file->buf_pos = file->buf_beg;
    file->offset = 0;
    return file;
}
int csd_fgetc(CSD_FILE *f)
{

}
int csd_putc(int ch,CSD_FILE *f)
{

}
int csd_fflush(FILE *f)
{

}
int csd_fclose(CSD_FILE *f)
{

}
int csd_fseek(FILE *stream, long offset, int whence)
{

}
void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
