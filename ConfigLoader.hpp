#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <string>
#include <map>
#include <memory>

#include "INode.hpp"

class ConfigLoader {
    static ConfigLoader loader;

    std::map<std::string, std::string> map;

    enum FdNames {BLOCKS_BITMAP=0, INODES_BITMAP=1, INODES=2, BLOCKS=3};
    int hostFd[4];

    static const int sizeOfBlock = 4096;

private:
    void strip(std::string& string, const std::string& characters_to_avoid = "\t\n\v\f\r ");
    void left_strip(std::string& string, const std::string& characters_to_avoid);
    void right_strip(std::string& string, const std::string& characters_to_avoid);

    int createSystemFiles();
    int createBitmapFile(const std::string& path, int numberOfBits);
    int createInodesFile(const std::string& path);
    int createBlocksFile(const std::string& path);
    inline bool fileExists (const std::string& name);

    std::string getBlocksBitmapPath();
    std::string getInodesBitmapPath();
    std::string getInodesPath();
    std::string getBlocksPath();
public:
    static ConfigLoader* getInstance();

    explicit ConfigLoader(std::string path);
    int getBlocksBitmap();
    int getInodesBitmap();
    int getInodes();
    int getBlocks();
    int getMaxNumberOfBlocks();
    int getMaxNumberOfInodes();
    int getMaxLengthOfName();
    int getSizeOfBlock();

    unsigned getFreeBlock();
    int freeBlock(unsigned block);

    void openFile(FdNames type, std::string path);
};

#endif
