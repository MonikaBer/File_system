#include "SimpleFS.hpp"
#include "simplefs.h"

static SimpleFS simpleFS = SimpleFS();

//extern "C" {
    int create(char * name, int mode) {
        return simpleFS.create(name, mode);
    }

    int open(char * name, int mode) {
        return simpleFS._open(name, mode);
    }

    int read(int fd, char * buf, int len) { 
       return simpleFS._read(fd, buf, len);
    }

    int write(int fd, char * buf, int len) {
        return simpleFS._write(fd, buf, len);
    }

    int lseek(int fd, int whence, int offset) {
        return simpleFS._lseek(fd, whence, offset);
    }

    int unlink(char * name) {
        return simpleFS.unlink(name);
    }

    int mkdir(char * name) {
        return simpleFS.mkdir(name);
    }

    int rmdir(char * name) {
        return simpleFS.rmdir(name);
    }
//}