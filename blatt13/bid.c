#include <ctype.h> // serves function for testing and mapping characters
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stralloc.h>
#include <sys/stat.h>
#include <string.h>
// #include <stdio.h>


#define ERROR(msg) write(2, msg, strlen(msg))
#define UIMSG(msg) write(1, msg, strlen(msg))
#define TRIES 100
#define RDBUF_SIZE 100
// #define DEBUG //include stdio.h


int compare(stralloc* ch1, stralloc* ch2) {
    //stralloc_0(ch1);
    //stralloc_0(ch2);
    if(!isdigit(ch1->s[0]) || !isdigit(ch2->s[0])) {
        ERROR("input must be digit!\n");
        exit(1);
    }
    for(int cnt; cnt < ch1->len; cnt++) {
        if(ch1->s[cnt] == '\n') {
            ch1->s[cnt] = '\0';
            ch1->len = cnt;
            break;
        }
    }
    for(int cnt; cnt < ch2->len; cnt++) {
        if(ch2->s[cnt] == '\n') {
            ch2->s[cnt] = '\0';
            ch2->len = cnt;
            break;
        }
    }
    unsigned long int num1 = strtoul(ch1->s, NULL, 10);
    unsigned long int num2 = strtoul(ch2->s, NULL, 10);

    return (num1 < num2);
}


int main(int argc, char* argv[]) {
    // string representation of bid value
    stralloc userbid = {0};
    stralloc oldbid = {0};

    if(argc != 3) {
        ERROR("invalid arguments!\n");
        exit(1);
    }
    // path messup and username
    stralloc user = {0};
    stralloc filename = {0};
    stralloc tmpfilename = {0};
    stralloc_copys(&filename, argv[1]);
    stralloc_copys(&tmpfilename, argv[1]);
    stralloc_copys(&user, argv[2]);
    
    stralloc_catm(&filename, "/current_bid\0");
    stralloc_catm(&tmpfilename, "/current_bid.tmp\0");
    int newline = 0;

    
    // checking if file of filename already exists and is REGular file
    struct stat statbuf = {0};
    if(stat(filename.s, &statbuf) == EFAULT) exit(1);
    if(!S_ISREG(statbuf.st_mode)) {
        int initfd;
        if((initfd = open(filename.s, O_WRONLY|O_CREAT|O_TRUNC|O_EXCL, 0666)) < 0){
            ERROR("failed to create file!\n");
            exit(1);
        }
        
        write(initfd,"0", 1);
        fsync(initfd);
        close(initfd);
    }
    // main part
    while(1) {

        int file;
        char buf[RDBUF_SIZE+1] = {0};

        if((file = open(filename.s, O_RDONLY)) < 0) {
            ERROR("unable to open file!\n");
            exit(1);
        }


        size_t nbytes = 0;
        stralloc_copys(&oldbid, "");
        newline = 0;
        while(!newline && (nbytes = read(file, buf, RDBUF_SIZE)) > 0) {
            if(nbytes < 0) {
                ERROR("unable to read from file!\n");
                exit(1);
            }
            for(int i = 0; i < nbytes; i++) {
                if(buf[i] == '\n') {
                    nbytes = i;
                    newline = 1;
                    break;
                }
            }
            if(!stralloc_catb(&oldbid, buf, nbytes)) {
                ERROR("mem error!\n");
                exit(1);
            }

        }
        close(file);
        file = -1;


        // UI
        UIMSG("highest bid is: ");
        write(1, oldbid.s, oldbid.len);
        write(1, "\n", 1);
        UIMSG("Your bid:\n");

        // read input from cmd line
        stralloc_copys(&userbid, "");
        
        while(!newline && (nbytes = read(0, buf, RDBUF_SIZE)) > 0) {

            if(nbytes < 0) {
                ERROR("unable to read from stdout!\n");
                exit(1);
            }
            for(int i = 0; i < nbytes; i++) {
                if(buf[i] == '\n') {
                    nbytes = i;
                    newline = 1;
                    break;
                }
            }
            if(!stralloc_catb(&userbid, buf, nbytes)) {
                ERROR("mem error!\n");
                exit(1);
            }
        }

        // try to get permission to bid
        int tmpfile;
        for(int tries = 0; tries < TRIES; tries++) {
            tmpfile = open(tmpfilename.s, O_RDWR|O_TRUNC|O_CREAT|O_EXCL, 0666);
            if(tmpfile >= 0) break;
            if(errno != EEXIST) break;
            usleep(10000);
#ifdef DEBUG
            fprintf(stdout, "tries: %d\n", tries);
#endif
        }

        if(tmpfile < 0) {
            ERROR("unable to get write permisson!\n");
            exit(1);
        }
      

        // check wether bid is still valid or not
        if((file = open(filename.s, O_RDONLY)) < 0) {
            unlink(tmpfilename.s);
            ERROR("unable to open file!\n");
            exit(1);
        }
        // read from file
        stralloc_copys(&oldbid, "");
        newline = 0;
        while((nbytes = read(file, buf, RDBUF_SIZE)) > 0) {
            if(nbytes < 0) {
                unlink(tmpfilename.s);
                ERROR("unable to read from file!\n");
                exit(1);
            }
            for(int i = 0; i < nbytes; i++) {
                if(buf[i] == '\n') {
                    nbytes = i;
                    newline = 1;
                    break;
                }
            }
            if(!stralloc_catb(&oldbid, buf, nbytes)) {
                unlink(tmpfilename.s);
                ERROR("mem error!\n");
                exit(1);
            }
        }
        close(file);
        file = -1;

        if(compare(&oldbid, &userbid)) {
            stralloc_cats(&userbid, " by \0");
            stralloc_catb(&userbid, user.s, user.len);
            if(write(tmpfile, userbid.s, userbid.len) < 0) {
                unlink(tmpfilename.s);
                ERROR("unable to write to tmpfile!\n");
                exit(1);
            }
            if(fsync(tmpfile) < 0) {
                unlink(tmpfilename.s);
                ERROR("unable to sync!\n");
                exit(1);
            }
            if(close(tmpfile) < 0) {
                unlink(tmpfilename.s);
                ERROR("unable to close tmpfile!\n");
                exit(1);
            }
            if(rename(tmpfilename.s, filename.s) < 0) {
                unlink(tmpfilename.s);
                ERROR("unable to rename!\n");
                exit(1);
            }


        } else {
            UIMSG("someone else was faster. Try again!\n");
            if(close(tmpfile) < 0) {
                ERROR("unable to close tmpfile!\n");
                exit(1);
            }
            if(unlink(tmpfilename.s) < 0) {
                ERROR("unable to delete tmp file!\n");
                exit(1);
            }

        }
    }

    return 0;

}
 



