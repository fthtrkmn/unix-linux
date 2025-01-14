#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 4096
#define DEFAULT_BLOCK_SIZE 512

// Hata durumunda programdan çıkmak için bir yardımcı fonksiyon
void exit_sys(const char *msg);

int main(int argc, char *argv[]) {
    char buf[BUFFER_SIZE];
    char *source_file = NULL, *dest_file = NULL, *conv = NULL;
    int fds, fdd;
    int block_size = DEFAULT_BLOCK_SIZE, count = 0;
    int c_count = 0, mode;
    ssize_t result;

    // Flag değişkenleri
    int if_flag = 0, of_flag = 0, bs_flag = 0, count_flag = 0;
    int convU_flag = 0, convL_flag = 0, convE_flag = 0;

    // Hatalı argüman sayısı kontrolü
    if (argc < 4) {
        fprintf(stderr, "Hatalı argüman sayısı!...\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "dd") != 0) {
        fprintf(stderr, "%s hatalı komut!...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Argümanları işlemeye başla
    for (int i = 2; i < argc; ++i) {
        char *delp = strtok(argv[i], "=");

        if (strcmp(delp, "if") == 0) {
            if_flag = 1;
            source_file = strtok(NULL, "=");
            if (!source_file) {
                fprintf(stderr, "--if-- hatası!...\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(delp, "of") == 0) {
            of_flag = 1;
            dest_file = strtok(NULL, "=");
            if (!dest_file) {
                fprintf(stderr, "--of-- hatası!...\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(delp, "bs") == 0) {
            bs_flag = 1;
            char *bs_val = strtok(NULL, "=");
            if (bs_val) {
                block_size = atoi(bs_val);
            } else {
                fprintf(stderr, "--bs-- hatası!...\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(delp, "count") == 0) {
            count_flag = 1;
            char *count_val = strtok(NULL, "=");
            if (count_val) {
                count = atoi(count_val);
            } 
            else {
                fprintf(stderr, "--count-- hatası!...\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(delp, "conv") == 0) {
            conv = strtok(NULL, "=");
            if (conv) {
                char *conv_delp = strtok(conv, ",");
                while (conv_delp != NULL) {
                    if (strcmp(conv_delp, "ucase") == 0) {
                        convU_flag = 1;
                    }
                    else if (strcmp(conv_delp, "lcase") == 0) {
                        convL_flag = 1;
                    }
                    else if (strcmp(conv_delp, "excl") == 0) {
                        convE_flag = 1;
                    }
                    else {
                        fprintf(stderr, "--conv-- hatalı dönüşüm!...\n");
                        exit(EXIT_FAILURE);
                    }
                    conv_delp = strtok(NULL, ",");
                }
            }
            else {
                fprintf(stderr, "--conv-- hatası!...\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            fprintf(stderr, "%s --hatalı argüman-- hata!...\n", delp);
            exit(EXIT_FAILURE);
        }
    }

    // Varsayılan block size değeri
    if (!bs_flag) 
        block_size = DEFAULT_BLOCK_SIZE;

    mode = O_WRONLY | O_CREAT | O_TRUNC | (convE_flag ? O_EXCL : 0);

    // Dosya açma işlemleri
    if ((fds = open(source_file, O_RDONLY)) == -1)
        exit_sys(source_file);

    if ((fdd = open(dest_file, mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) 
        exit_sys(dest_file);

    // Dosyayı okuma ve yazma döngüsü
    while ((result = read(fds, buf, block_size)) > 0) {
        if (count_flag && c_count++ >= count)
            break;

        if (convL_flag) {
            for (int i = 0; buf[i]; i++) 
                buf[i] = tolower(buf[i]);
        } 
        if (convU_flag) {
            for (int i = 0; buf[i]; i++) 
                buf[i] = toupper(buf[i]);
        }

        if (write(fdd, buf, result) != result) {
            fprintf(stderr, "Dosyaya yazılamıyor!...\n");
            exit(EXIT_FAILURE);
        }
    }

    close(fds);
    close(fdd);
    return 0;
}

// Sistem hatalarını göstermek için yardımcı fonksiyon
void exit_sys(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
