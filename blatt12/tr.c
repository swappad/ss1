#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stralloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define PERROR(msg) (write(2, msg, strlen(msg)))

int main(int argc, char* argv[]) {
    if(argc != 4) {
        PERROR("wrong arguments!");
        exit(0);
    }
    stralloc irep = {0};
    stralloc orep = {0};
    stralloc infile = {0};

    if(!(
            stralloc_copys(&irep, argv[1]) && 
            stralloc_copys(&orep, argv[2]) && 
            stralloc_copys(&infile, argv[3]))) {
        PERROR("wrong arguments!");
        exit(0);
    }

    if(irep.len != orep.len) {
        PERROR("arguments must have same length");
        exit(0);
    }

    int infd = open("infile", O_RDWR);
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
    for(; buf < buf + nbytes; buf++) {
        for(int index = 0; index < irep.len; index++) {
            if(*buf == irep.s[index]) {
                *buf = orep.s[index];
            }
        }
    }

    munmap((caddr_t) buf, nbytes);
    close(infd);

    return 1;

}
