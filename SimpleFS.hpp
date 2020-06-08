//Main class of file system
#ifndef SIMPLE_FS_HPP
#define SIMPLE_FS_HPP
static const int blockSize = 4096;

#include <string>
#include <vector>
#include <map>

#include "Lock.hpp"
#include "FileDescriptor.hpp"
#include "INode.hpp"


class SimpleFS {
private:
    const static int sizeofInode = sizeof(unsigned short) + sizeof(long) + 14 * sizeof(unsigned);

    // TODO implement shared filedescriptors!
    //    enum fdNames {BLOCKS_BITMAP, INODES_BITMAP, INODES, BLOCKS};
//    static int hostFd[4];
//    // ALBO
//    std::map<std::string, int> fdTable =
//            // mapa będzie wypełniona w konstruktorze podczas otwierania plików ale to będzie coś w tym stylu
//            {
//                    {"inodes", 3},
//                    {"inodesBitmap", 4},
//                    {"blocks", 5},
//                    {"blocksBitmap", 6}
//            };

    //files paths
    std::string blocks_bitmap;
    std::string inodes_bitmap;
    std::string inodes;
    std::string blocks;

    int max_number_of_blocks;
    int max_number_of_inodes;

    std::vector<Lock> open_files;
    std::vector<FileDescriptor*> fds;

    std::vector<std::string> parse_direct(std::string & path);
    int find_free_inode();
    static int createBitmapFile(const std::string& path, int numberOfBits);
    int createInodesFile(const std::string& path) const;
    int createBlocksFile(const std::string& path) const;
    static inline bool fileExists (const std::string& name);

public:
    // TODO public only for testing:
    int writeInode(FileDescriptor& fd, INode& inode);
    int readInode(FileDescriptor& fd, INode& inode);
    int clearInode(FileDescriptor&fd);
    unsigned getFreeBlock();
    int freeBlock(unsigned block);

    SimpleFS(std::string && configPath);
    int create(std::string && name, int mode);
    int open(std::string && name, int mode);
    int read(int fd, char * buf, int len);
    int write(int fd, char * buf, int len);
    int lseek(int fd, int whence, int offset);
    int unlink(std::string && name);
    int mkdir(std::string && name);
    int rmdir(std::string && name);
    int createSystemFiles();
};

#endif
