#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc,char *argv[]){

int result;

//{"number",optional_argument,NULL,3},

opterr=0;
char *bytes_arg,*lines_arg = NULL;
int verbose_flag,help_flag,bytes_flag,lines_flag,err_flag,version_flag;

verbose_flag=help_flag=bytes_flag=lines_flag=err_flag=version_flag= 0 ;

struct option options[] ={
{"verbose",no_argument,&verbose_flag,'v'},
{"help",no_argument,&help_flag,1},
{"version",no_argument,&version_flag,2},
{"bytes",required_argument,NULL,'c'},
{"lines",required_argument,NULL,'n'},
{0,0,0,0},
};

while((result = getopt_long(argc,argv,"vc:n:",options,NULL)) != -1 ){

    switch (result)
    {
    case 'v':/* constant-expression */
    printf("--verbose option given \n");
    verbose_flag=1;
    break;
    case 'c':/* constant-expression */
    printf("--bytes option given\n");
    bytes_flag=1;
    bytes_arg = optarg;
    break;
    case 1:
    printf("--help option given\n");
    break;
    case 2:
    printf("--version option given\n");
    break;
    case 'n':
    printf("--lines option given\n");
    lines_flag=1;
    lines_arg=optarg;
    break;   
    case '?':
    if(optopt=='n')
        fprintf(stderr,"Error: --lines option without argument \n");
    else if(optopt== 'c')
        fprintf(stderr,"Error: --bytes option without argument \n");
    else if(optopt != 0)
        fprintf(stderr,"Error: invalid option: -%c \n",optopt);
    else
        fprintf(stderr,"Error: invalid long option \n");   

    err_flag=1;         
    break; 
    
    default:
        break;
    }
}   

    if(err_flag)
        exit(EXIT_FAILURE);
    else if(lines_flag && bytes_flag)
    {
        fprintf(stderr,"Error: --lines and --bytes options cannot be used together \n");   
        exit(EXIT_FAILURE);
    }
    if(optind==argc)
    {   
        fprintf(stderr,"Error: Enter File Name! \n");   
        exit(EXIT_FAILURE);
    }
        
    FILE *fp;
    if((bytes_flag || verbose_flag ||lines_flag)
        || !(help_flag || version_flag))
    {
        for(int k=optind;k<argc;k++)
        {                         

            if((fp=fopen(argv[k],"r"))==NULL)
            {   
            fprintf(stderr,"Error: \"%s\" File Not Found! \n",argv[k]);   
            exit(EXIT_FAILURE);
            }
            else
            {
                if(verbose_flag)
                   printf("<<<%s>>>\n",argv[k]);
                int satir_sayac = 10;
                if(bytes_flag+lines_flag != 0)
                {
                    if(lines_arg!=NULL)
                    {
                       satir_sayac = atoi(lines_arg);
                    }
                    else
                        satir_sayac = atoi(bytes_arg);
                }
                for(int i=0;i<satir_sayac;++i)
                {
                    if(bytes_flag)
                    {
                      putchar(fgetc(fp));

                    }
                    else
                    {
                        char temp=0;
                        while(temp!='\n')
                        {
                        temp=fgetc(fp);
                        putchar(temp);
                        }
                    }

                }

            }
        }

    }
    else if(help_flag)
    {
    printf("-c ya da --bytes: Dosyanın başından itibaren kaç byte yazılacağını belirtir.\n");
    printf("-n ya da --lines: Dosyanın başından itibaren kaç satırın yazılacağını belirtir.\n");
    printf("-v ya da --verbose: Dosyaların isimlerini de yazdırır.\n");
    printf("--help: Komut hakkında açıklama yazdırır.\n");
    printf("--version: Copyright ve versiyon numarasını yazdırır.\n");
    }
    else if(version_flag)
        printf("Version: 1.0\n");



}
