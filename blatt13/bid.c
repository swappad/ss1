#include <ctype.h> // serves function for testing and mapping characters
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stralloc.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#define ERROR(msg) write(2, msg, strlen(msg))
#define UIMSG(msg) write(1, msg, strlen(msg))
#define TRIES 10
#define RDBUF_SIZE 100


int compare(stralloc* ch1, stralloc* ch2) {
    stralloc_0(ch1);
    stralloc_0(ch2);
    for(char* cp = ch1->s; cp < ch1->s + ch1->len; cp++) {
        if(*cp == '\n') {
            *cp = '\0';
        }
        if(!isdigit(cp)) {
            ERROR("input must be digits!\n");
            exit(1);
        }
    }
    for(char* cp = ch2->s; cp < ch2->s + ch2->len; cp++) {
        if(*cp == '\n') {
            *cp = '\0';
            ch1->len = cp - ch1->s;
        }
        if(!isdigit(cp)) {
            ERROR("input must be digits!\n");
            exit(1);
        }
    }
    stralloc_0(ch1);
    stralloc_0(ch2);
    unsigned long int num1 = atol(ch1->s);
    unsigned long int num2 = atol(ch1->s);
    return (num1 < num2);
}


int main() {
    char* filename = "./current_bid\0";
    char* tmpfilename = "current_bid.tmp\0";
    // string representation of bid value
    stralloc userbid = {0};
    stralloc oldbid = {0};
    
    // checking if file of filename already exists and is REGular file
    struct stat statbuf = {0};
    if(stat(filename, &statbuf) == EFAULT) exit(1);
    if(!S_ISREG(statbuf.st_mode)) {
        int initfd;
        if((initfd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) exit(1);
        write(initfd,"0", 1);
        close(initfd);
    }
    // main part
    while(1) {

        int file;
        char buf[RDBUF_SIZE] = {0};

        if((file = open(filename, O_RDONLY)) < 0) {
            ERROR("unable to open file!\n");
            exit(0);
        }


        size_t nbytes = 0;
        if((nbytes = read(file, buf, RDBUF_SIZE)) < 0) {
            ERROR("unable to read from file!\n");
            exit(0);
        }
        close(file);
        file = -1;
        buf[nbytes-1] = '\0';
        if(!stralloc_cats(&oldbid, buf)) {
            ERROR("mem error!\n");
            exit(0);
        }

        // UI
        UIMSG("highest bid is:");
        write(1, oldbid.s, oldbid.len);
        write(1, "\n", 1);
        UIMSG("Your bid:\n");

        // read input from cmd line
        if((nbytes = read(0, buf, RDBUF_SIZE)) < 0) {
            ERROR("unable to read from stdout!\n");
            exit(0);
        }
        fprintf(stdout, "%ld \n", nbytes);
        buf[nbytes-1] = '\0';
        if(!stralloc_cats(&userbid, buf)) {
            ERROR("mem error!\n");
            exit(0);
        }


        // try to get permission to bid
        int tmpfile;
        for(int tries = 0; tries < TRIES; tries++) {
            tmpfile = open(tmpfilename, O_WRONLY|O_CREAT|O_EXCL, 0666);
            if(tmpfile >= 0) break;
            if(errno != EEXIST) break;
            usleep(10000);
            fprintf(stdout, "%d\n", tries);
        }

        if(tmpfile < 0) {
            ERROR("unable to get write permisson!\n");
            exit(0);
        }

        // check wether bid is still valid or not
        if((file = open(filename, O_RDONLY)) < 0) {
            ERROR("unable to open file!\n");
            exit(1);
        }
        // read from file
        if((nbytes = read(file, buf, RDBUF_SIZE)) < 0) {
            ERROR("unable to read from file!\n");
            exit(0);
        }
        buf[nbytes-1] = '\0';
        close(file);
        file = -1;
        if(!stralloc_cats(&oldbid, buf)) {
            ERROR("mem error!\n");
            exit(0);
        }

        printf("Hallo");
        if(compare(&oldbid, &userbid)) {
            if(write(tmpfile, &userbid.s, userbid.len) < 0) {
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
            if(rename(tmpfilename, filename) < 0) {
                ERROR("unable to rename!\n");
                exit(1);
            }
        } else {
            UIMSG("someone else was faster. Try again!\n");
            if(close(tmpfile) < 0) {
                ERROR("unable to close tmpfile!\n");
                exit(1);
            }
            if(unlink(tmpfilename) < 0) {
                ERROR("unable to delete tmp file!\n");
                exit(1);
            }

        }
    }

    return 0;

}
 



