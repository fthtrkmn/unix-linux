#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int result;
    int a_flag,m_flag,M_flag,d_flag,s_flag,err_flag;
    char *M_arg;
    double arg1,arg2,calc_result;

    a_flag = m_flag = M_flag = d_flag = s_flag = err_flag = 0;

    opterr=0;

    while((result =getopt(argc,argv,"amM:ds"))!=-1)
    {
        switch(result)
        {
            case 'a':
            a_flag = 1;
            break;
            case 'M':
            M_flag = 1;
            M_arg=optarg;
            break;
            case 'm':
            m_flag = 1;
            break;
            case 'd':
            d_flag = 1;
            break;
            case 's':
            s_flag = 1;
            break;
            case '?':
            if(optopt == 'M')
            fprintf(stderr,"-M option must have an argument");
            else
            fprintf(stderr,"invalid option: -%c\n",optopt);

            err_flag=1;
            break;  

        }
    }

    if(err_flag)
        exit(EXIT_FAILURE);
    if(a_flag+m_flag+d_flag+s_flag>1){
        fprintf(stderr,"only one option must be specified");
                exit(EXIT_FAILURE);

    }
    if(a_flag+m_flag+d_flag+s_flag == 0){
        fprintf(stderr,"only one option must be specified");
                exit(EXIT_FAILURE);

    }
    if(argc-optind!=2){
        fprintf(stderr,"two number must be specified");
                exit(EXIT_FAILURE);

    }

    arg1=atof(argv[optind]);
    arg2=atof(argv[optind+1]);

    if(a_flag)
    calc_result=arg1+arg2;
    else if(m_flag)
    calc_result=arg1*arg2;
    else if(d_flag)
    calc_result=arg1/arg2;
    else
    calc_result=arg1-arg2;

    if(M_flag)
    printf("%s: %f\n",M_arg,calc_result);
    else
    printf("%f\n",calc_result);

    return 0;


}
