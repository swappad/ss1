#include <ctype.h> // serves function for testing and mapping characters
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stralloc.h>
#include <sys/stat.h>
#include <string.h>

#define ERROR(msg) write(2, msg, strlen(msg))
#define UIMSG(msg) write(1, msg, strlen(msg))
#define OFFSET 48
#define TRIES 10

int main() {
    char* filename = "./current_bid\0";
    char* tmpfilename = "current_bid.tmp\0";
    long bid = 0;
    // string representation of bid value
    stralloc bidch = {0};
    stralloc_catulong(&bidch, bid);


    // checking if file of filename already exists and is REGular file
    struct stat statbuf = {0};
    if(stat(filename, &statbuf) == EFAULT) exit(1);
    if(!S_ISREG(statbuf.st_mode)) {
        int initfd;
        if((initfd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) exit(1);
        write(initfd, &bid, sizeof(long));
        close(initfd);
    }

    while(1) {

        int file;
        if((file = open(filename, O_RDONLY)) < 0) {
            ERROR("unable to open file!\n");
            exit(0);
        }

        // read (long) bid from official file
        if(read(file, &bid, sizeof(unsigned long int)) != sizeof(unsigned long int)) {
            ERROR("unable to read from file!\n");
            exit(0);
        }
        close(file);
        file = -1;

        // UI
        UIMSG("highest bid is:\n");
        write(1, bidch.s, bidch.len);
        unsigned long int in = 0;
        char inbuf[10];
        ssize_t nbytes = 0;
        // read input from cmd line
        char inch;
        while(read(0, &inch, 1) != 1) {
            if(in == '\n') break; 
            if(!isdigit(inch)) {
                ERROR("input must be digits!\n");
                exit(1);
            }

            if(!stralloc_catb(&bidch, inch, 1)){
                ERROR("unable to allocate mem!");
                exit(1);
            }

        }
        if(!stralloc_0(&bidch)) {
            ERROR("unable to allocate mem!");
            exit(1);
        }
        bid = atol(bidch.s);

        // try to get permission to bid
        int tmpfile;
        for(int tries = 0; tries < TRIES; tries++) {
            tmpfile = open(tmpfilename, O_WRONLY|O_CREAT|O_EXCL, 0666);
            if(outfd >= 0 || errno != EEXIST) break;
            usleep(10000);
        }

        if(outfd < 0) {
            ERROR("unable to get write permisson!\n");
            exit(0);
        }

        // check wether bid is still valid or not
        unsigned long int bidbuf = 0;
        if(file = open(filename, O_RONLY) < 0) {
            ERROR("unable to open file!\n");
            exit(1);
        }
        if(read(file, &bidbuf, sizeof(unsigned long int)) != sizeof(unsigned long int)) {
            ERROR("file broken!\n");
            exit(1);
        }
        close(file);
        if(bidbuf < bid) {
            if(write(tmpfile, bid, sizeof(unsigned long int)) != sizeof(unsigned long int)) {
                ERROR("unable to write to tmpfile!\n");
                exit(1);
            }
            if(fsync(tmpfile) < 0) {
                ERROR("unable to sync!\n");
                exit(1);
            }
            if(close(tmpfile) < 0) {
                ERROR("unable to close tmpfile!\n");
                exit(1);
            }
            if(rename(&tmpfilename, &filename) < 0) {
                ERROR("unable to rename!\n");
                exit(1);
            }
        } else {
            UIMSG("someone else was faster. Try again!\n");
        }
    }

    return 0;

}
 



