#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "csd_pwd.h"


void exit_sys(const char *msg);

int main(int argc,char *argv[])
{
    struct passwd *l_passwd =NULL;
    if(argc<2){
        fprintf(stderr,"Kullanicı adı giriniz.\n");
    }
    /*
    
        BURADA İSTENİLEN KOMUTA GÖRE BİR SWİTCH CASE YAPISI KURULACAK.
            
    
    */
    for(int i=1;i<argc;i++)
    {
        l_passwd = csd_getpwuid((uid_t)atoi(argv[i]));
        printf("Kullanıcı Adı: %s \n",l_passwd->pw_name);
        printf("Şifre: %s\n",l_passwd->pw_passwd);
        printf("Kullanıcı ID: %llu\n",(unsigned long long)l_passwd->pw_uid);
        printf("Grup ID: %llu\n",(unsigned long long)l_passwd->pw_gid);
        printf("Bilgiler: %s\n",l_passwd->pw_gecos);
        printf("Ana Dizin: %s\n",l_passwd->pw_dir);
        printf("Kabuk Program: %s",l_passwd->pw_shell);
        printf("----------------------\n");

    }


    return 0;

}

struct passwd *csd_getpwnam(const char *name)
{                   

    FILE *fd;
    static struct passwd ret_val;
    static char line[4096];
    static char copy_line[4096];
    int kullanici_bayrak=0;
    int sayac=0;
	if ((fd = fopen("/etc/passwd", "r")) == NULL) 
		exit_sys("fopen");

    while (fgets(line, 4096, fd) != NULL) {
        strcpy(copy_line,line);
        if(!strncmp(name,line,strlen(name)))
        {
            kullanici_bayrak=1;
            if(copy_line[sayac++]!=':'){
                ret_val.pw_name = strtok(line, ":");
                sayac +=strlen(ret_val.pw_name);
            }
            if(copy_line[sayac++]!=':'){
                ret_val.pw_passwd = strtok(NULL, ":");
                sayac +=strlen(ret_val.pw_passwd);
            }
            if(copy_line[sayac++]!=':'){
                char *pwuid = strtok(NULL, ":");
                ret_val.pw_uid = atoi(pwuid);
                sayac +=strlen(pwuid);
            }
            if(copy_line[sayac++]!=':'){
                char *pwuid = strtok(NULL, ":");
                ret_val.pw_gid = atoi(pwuid);
                sayac +=strlen(pwuid);
            }
            if(copy_line[sayac++]!=':'){
                ret_val.pw_gecos = strtok(NULL, ":");
                sayac +=strlen(ret_val.pw_gecos);
            }
            if(copy_line[sayac++]!=':'){
                ret_val.pw_dir = strtok(NULL, ":");
                sayac +=strlen(ret_val.pw_dir);
            }
            if(copy_line[sayac++]!=':'){
                ret_val.pw_shell = strtok(NULL, ":");
                sayac +=strlen(ret_val.pw_shell);
            }                       
            break;
        }

    }
    if(!kullanici_bayrak)
    {
        fprintf(stderr,"'%s' Kullanıcı Bulunamadı..!\n",name);
        exit(EXIT_FAILURE);
    }

    return &ret_val;	

}
struct passwd *csd_getpwuid(uid_t uid)
{

    FILE *fd;
    static struct passwd ret_val;
    static char line[4096];
    static char copy_line[4096];
    int kullanici_bayrak=0;
    int sayac=0;
	if ((fd = fopen("/etc/passwd", "r")) == NULL) 
		exit_sys("fopen");

    while (fgets(line, 4096, fd) != NULL) {
        strcpy(copy_line,line);

        if(copy_line[sayac++]!=':'){
            ret_val.pw_name = strtok(line, ":");
            if(ret_val.pw_name != NULL)
                sayac +=strlen(ret_val.pw_name);
        }
        if(copy_line[sayac++]!=':'){
            ret_val.pw_passwd = strtok(NULL, ":");
            if(ret_val.pw_passwd != NULL)
                sayac +=strlen(ret_val.pw_passwd);
        }
        if(copy_line[sayac++]!=':'){
            char *pwuid = strtok(NULL, ":");
            if(pwuid!=NULL)
                ret_val.pw_uid = atoi(pwuid);
            if(uid != ret_val.pw_uid){
               sayac=0;
               continue;
            }
            kullanici_bayrak=1;
            sayac +=strlen(pwuid);
        }
        if(copy_line[sayac++]!=':'){
            char *pwuid = strtok(NULL, ":");
            if(pwuid!=NULL){
                ret_val.pw_gid = atoi(pwuid);
            sayac +=strlen(pwuid);}
        }
        if(copy_line[sayac++]!=':'){
            ret_val.pw_gecos = strtok(NULL, ":");
            if(ret_val.pw_gecos != NULL)
                sayac +=strlen(ret_val.pw_gecos);
        }
        if(copy_line[sayac++]!=':'){
            ret_val.pw_dir = strtok(NULL, ":");
            if(ret_val.pw_dir != NULL)
                sayac +=strlen(ret_val.pw_dir);
        }
        if(copy_line[sayac++]!=':'){
            ret_val.pw_shell = strtok(NULL, ":");
            if(ret_val.pw_shell != NULL)
                sayac +=strlen(ret_val.pw_shell);
        }                       
        break;
        

    }
    if(!kullanici_bayrak)
    {
        fprintf(stderr,"'%d' Kullanıcı Bulunamadı..!\n",uid);
        exit(EXIT_FAILURE);
    }

    return &ret_val;	




}
void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
