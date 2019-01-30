#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stralloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

//#define DEBUG

#define PERROR(msg) (write(2, msg, strlen(msg)))

int main(int argc, char* argv[]) {
    if(argc != 4) {
        PERROR("wrong arguments!");
        exit(0);
    }
    stralloc irep = {0};
    stralloc orep = {0};
    stralloc infile = {0};
    for(int p = 0; p < strlen(argv[1]); p++) {
        if(argv[1][p] == '-') {
            int range = argv[1][p+1] - argv[1][p-1];
            for(char ch = (char) (argv[1][p-1] + 1); ch < argv[1][p-1] + range; ch++) {
                if(!stralloc_append(&irep, &ch)) {
                    PERROR("wrong arguments!");
                    exit(0);
                }
            }
        } else {
            if(!stralloc_append(&irep, &(argv[1][p]))) {
                PERROR("wrong arguments!");
                exit(0);
            }
        }
  }
    for(int p = 0; p < strlen(argv[2]); p++) {
        if(argv[2][p] == '-') {
            int range = argv[2][p+1] - argv[2][p-1];
            for(char ch = (char) (argv[2][p-1] + 1); ch < argv[2][p-1] + range; ch++) {
                if(!stralloc_append(&orep, &ch)) {
                    PERROR("wrong arguments!");
                    exit(0);
                }
            }
        } else {
            if(!stralloc_append(&orep, &(argv[2][p]))) {
                PERROR("wrong arguments!");
                exit(0);
            }
        }
    }
#ifdef DEBUG
    write(1, irep.s, irep.len);
    write(1, "\n", 1);
    write(1, orep.s, orep.len);
    write(1, "\n", 1);
#endif


    if(!stralloc_copys(&infile, argv[3])) {
        PERROR("wrong arguments!");
        exit(0);
    }
    if(irep.len != orep.len) {
        PERROR("arguments must have same length");
        exit(0);
    }

    int infd = open(infile.s, O_RDWR);
    if(infd < 0) {
        PERROR("wrong path!");
        exit(0);
    }
    struct stat statbuf;
    if(fstat(infd, &statbuf) < 0) {
        PERROR("unable to hande file!");
        exit(0);
    }
    off_t nbytes = statbuf.st_size;
    char* buf = (char*) mmap(0, nbytes, PROT_READ | PROT_WRITE, MAP_SHARED, infd, 0);
    if(buf == MAP_FAILED) {
        PERROR("file error!");
        exit(0);
    }
    for(int cnt = 0; cnt < nbytes; cnt++) {
        for(int index = 0; index < irep.len; index++) {
            if(buf[cnt] == irep.s[index]) {
                buf[cnt] = orep.s[index];
                break;
            }
        }
    }

    munmap((caddr_t) buf, nbytes);
    close(infd);

    exit(1);

}
