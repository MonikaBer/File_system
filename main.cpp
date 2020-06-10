#include <iostream>

#include "SimpleFS.hpp"

void showLseek() {
    SimpleFS fs = SimpleFS("../etc/simplefs.conf");
    fs.create("/test", 1);
    fs.create("/testDwa", 1);
    fs.create("/testTrzy", 1);
    int fd = fs.open("/testDwa", 1);
    std::cout << "fd testDwa: " << fd << std::endl;
    fd = fs.open("/test", 1);
    std::cout << "fd test: " << fd << std::endl;
    fd = fs.open("/testTrzy", 1);
    std::cout << "fd testTrzy: " << fd << std::endl;
    std::cout << "Opened files: " << std::endl;
    for (FileDescriptor fDesc : fs.getFds()) {
        std::shared_ptr<INode> inode = fDesc.getInode();
        std::cout << "Inode: " << inode->getId() << "Cursor: " << fDesc.getFileCursor() << std::endl;
    }
    fs.lseek(0, 0, 30);
    fs.lseek(1, 0, 30);
    fs.lseek(1, 1, 30);
    fs.lseek(2, 2, 20); // TODO na razie plik ma zerowa dlugosc wiec to nie pyknie
    for (FileDescriptor fDesc : fs.getFds()) {
        std::shared_ptr<INode> inode = fDesc.getInode();
        std::cout << "Inode: " << inode->getId() << "Cursor: " << fDesc.getFileCursor() << std::endl;
    }
}

int main() {
    showLseek();

    return 0;
}