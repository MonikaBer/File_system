//Class representing structure of file descriptor
#ifndef FILE_DESCRIPTOR_HPP
#define FILE_DESCRIPTOR_HPP

#include <memory>
#include <utility>
#include "INode.hpp"
#include "Lock.hpp"

class FileDescriptor {
private:
    std::shared_ptr<INode> inode;
    long fileCursor{};
    Lock::Type lockType;

public:
    //methods declarations
    FileDescriptor(std::shared_ptr<INode> inode, Lock::Type type) : inode(std::move(inode)), lockType(type), fileCursor(0) {}

    int writeToInode(char *buffer, int len);
    std::shared_ptr<INode> getInode() { return inode; }
    long getFileCursor() const { return fileCursor; }
    Lock::Type getLockType() const { return lockType; }
    void setFileCursor(long fileCursor) { this->fileCursor = fileCursor; }

};

#endif