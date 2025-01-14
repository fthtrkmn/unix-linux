#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ftw.h>

int string_callback(const char *path, const struct stat *finfo, int flag, struct FTW *ftw);
int callback(const char *path, const struct stat *finfo, int flag, struct FTW *ftw);
void exit_sys(const char *msg);
char filename[1024];
char *uzanti[10];
char *file_arg,*string_arg,*extension_arg = NULL;
int sayac_extension = 2;

int main(int argc,char *argv[]){

	if (argc < 3) {
		fprintf(stderr, "wrong number of arguments!...\n");
		exit(EXIT_FAILURE);
	}

    int result;
    int file_flag,string_flag,extension_flag,err_flag;
    file_flag = string_flag = extension_flag = err_flag = 0;

    struct option options[] ={
    {"file",required_argument,NULL,'f'},
    {"string",required_argument,NULL,'s'},
    {"extension",required_argument,NULL,'e'},
    {0,0,0,0}
    };
    opterr = 0;
    while((result = getopt_long(argc,argv,"f:s:e:",options,NULL)) != -1){
        switch(result){
            case 'f':
            file_arg = optarg;
            file_flag = 1;                           
            break;
            case 's':
            string_arg = optarg;
            uzanti[0]="txt";
            string_flag = 1;                           
            break;
            case 'e':
            extension_arg = optarg;
            extension_flag = 1;                           
            break;
            case '?':
            if(optopt == 'f')
                fprintf(stderr,"Error:--file option without argument!..\n");
            if(optopt == 's')
                fprintf(stderr,"Error:--string option without argument!..\n");
            if(optopt == 'e')
                fprintf(stderr,"Error:--extensions option without argument!..\n");
            if(optopt != 0)
                fprintf(stderr,"Error: Invalid  option --'%c'!..\n",optopt);
            else
                fprintf(stderr,"Error: invalid long option \n");   
            err_flag = 1;
            break;
            default:
            break;    
        }
    }

    if(err_flag)
        exit(EXIT_FAILURE);
    if(extension_flag && !string_flag){
        fprintf(stderr,"Error: Extensions option shall not be used without --string option");
        exit(EXIT_FAILURE);
    }
    if((optind+1) != argc){
        fprintf(stderr,"Error: Wrong number of argument!..\n");
    }
    else
    {
        strcpy(filename,argv[optind]);
    }



    if(file_flag){
        if ((result = nftw(filename, callback, 100, FTW_PHYS | FTW_CHDIR)) == -1)
		    exit_sys("nftw");
    }
    if(extension_flag){                
        sayac_extension = 0;
        if(extension_arg != NULL){
            uzanti[sayac_extension] = strtok(extension_arg, ";");
            while(uzanti[sayac_extension++] != NULL){
                uzanti[sayac_extension] = strtok(NULL,";");
                if(sayac_extension == 9)
                    break;
            }
        }
        else{
            sayac_extension = 2;
            uzanti[0] = "txt";
        }
    }
    else
    {
        sayac_extension = 2;
        uzanti[0] = "txt";
        //strcpy(uzanti[0],"txt");

    }
    if(string_flag){
        if ((result = nftw(filename, string_callback, 100, FTW_PHYS | FTW_CHDIR)) == -1)
		    exit_sys("nftw");
    }


}
int callback(const char *path, const struct stat *finfo, int flag, struct FTW *ftw)
{
    char absolute_path[1024];
	switch (flag) {
		case FTW_DNR:
			//printf("%*s%s (cannot read directory)\n", ftw->level * 4, "", path + ftw->base);
			break;
		case FTW_NS:
			//printf("%*s%s (cannot get statinfo)\n", ftw->level * 4, "", path + ftw->base);
			break;
		default:
            if(strcmp(path + ftw->base,file_arg) == 0)
            {
                getcwd(absolute_path,1024);
                if (S_ISDIR(finfo->st_mode)) {
                    printf("Directory Path: %s/%s\n",absolute_path,file_arg);
                }
                else{
                printf("File Path: %s%s\n",absolute_path,path + (ftw->base-1));
                }
            }
	}

	return 0;
}
int string_callback(const char *path, const struct stat *finfo, int flag, struct FTW *ftw)
{
    char absolute_path[1024];
    char line[4096];
    int tarama = 1;
    FILE *fd;
    int sayac = 0;
    if(!S_ISDIR(finfo->st_mode)){
        getcwd(absolute_path,1024);
        strcat(absolute_path,path + (ftw->base-1));    
        //printf("Diagnose: %s %d\n",absolute_path,sayac_extension);
        for(int i = 1;i < sayac_extension;i++)
        {
           if(!strcmp(uzanti[i-1],&absolute_path[(int)(strlen(absolute_path) - strlen(uzanti[i-1]))])){
                if ((fd = fopen(absolute_path, "r")) == NULL) 
		            exit_sys("fopen");

                while (fgets(line, 4096, fd) != NULL) {
                    sayac++;
                    if(strstr(line,string_arg) != NULL){
                        fprintf(stdout,"%s <%d>: %s \n",absolute_path,sayac,line);
                    }
                    else{
                    }
                
                }

            }

        }

    }
        
    return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
