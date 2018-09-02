// A simple exercise in emulating the cat command

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>


// Since we're not using a file pointer we need to buffer it manually
// This function does that
// I'm regretting this exercise already
void cat(int fd) {

    // We use this to write to stdout
    int output = fileno(stdout);
    // We need a buffer for fast reads
    static char* buffer;

    // https://linux.die.net/man/2/stat
    struct stat filestats;

    if(fstat(output, &filestats) == -1) {
        fprintf(stderr, "%d", errno);
    }

    // st_blksize is size_t
    buffer = malloc(filestats.st_blksize);
    if(buffer == NULL) {
        fprintf(stderr, "SEGMENTATION FAULT. FUCK YOU");
    }

    ssize_t readbytes, writtenbytes;

    readbytes = read(fd, buffer, filestats.st_blksize);
    while (readbytes > 0) {
        writtenbytes = write(output, buffer, readbytes);
        readbytes = read(fd, buffer, filestats.st_blksize);
    }
}

int main(int argc, char *argv[]) {
    int descriptor;
    
    // If there are no args just read from stdin
    // This gives us a file descriptor to pass to the write call
    if(argc < 2) {
        descriptor = fileno(stdin);
        cat(descriptor);
    } else {
        
        // First argv is always the name of the executing file. We don't want that
        argv++;
        do {
            if(strcmp("-", *argv) == 0) {
                descriptor = fileno(stdin);
            } else {
                descriptor = open(*argv, O_RDONLY);
            }
            if(descriptor < 0) {
                fprintf(stderr, "%d", errno);
            }
            cat(descriptor);
            argv++;
        } while(*argv);
    
    }
}