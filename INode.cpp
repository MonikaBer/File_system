//Class representing i-node
#include <algorithm>
#include <cstring>

#include "INode.hpp"


//methods definitions
std::fstream& operator>>(std::fstream &inodes, INode* iNode) {
    inodes.read((char*)&(iNode->mode), sizeof(iNode->mode));
    inodes.read((char*)&(iNode->length), sizeof(iNode->length));
    inodes.read((char*)&(iNode->number_of_blocks), sizeof(iNode->number_of_blocks));
    inodes.read((char*)iNode->blocks.data(), sizeof(iNode->blocks));
    inodes.read((char*)&(iNode->indirect_block), sizeof(iNode->indirect_block));
    return inodes;
}

std::fstream & operator<<(std::fstream &inodes, const INode* iNode) {
    inodes.write((char*)&(iNode->mode), sizeof(iNode->mode));
    inodes.write((char*)&(iNode->length), sizeof(iNode->length));
    inodes.write((char*)&(iNode->number_of_blocks), sizeof(iNode->number_of_blocks));
    inodes.write((char*)iNode->blocks.data(), sizeof(iNode->blocks));
    inodes.write((char*)&(iNode->indirect_block), sizeof(iNode->indirect_block));
    return inodes;
}

INode::INode(unsigned int id, unsigned short mode, long length, unsigned int numberOfBlocks, unsigned int indirectBlock)
                : inode_id(id), mode(mode), length(length), number_of_blocks(numberOfBlocks), indirect_block(indirectBlock) {}

INode::INode(unsigned id): inode_id(id) {
    ConfigLoader * config = ConfigLoader::getInstance();
    std::fstream & inodes = config->getInodes();
    unsigned int inode_position = id * sizeofInode;
    inodes.seekg(0, std::ios::end);
    if(inodes.tellg() < inode_position + sizeofInode)
        throw std::runtime_error("inode doesnt exist");
    inodes.seekg(inode_position);
    inodes >> this;
}

/**
 * Adds block of data to iNode. Block can be acquired by ConfigLoader::getInstance()->getFreeBlock().
 *
 * @param block - block index
 * @return
 */
int INode::addBlock(unsigned int block) {
    if(number_of_blocks < 12) {
        blocks[number_of_blocks] = block;
    }
    else {
        if (indirect_block == 0) {
            indirect_block = ConfigLoader::getInstance()->getFreeBlock();  // TODO Artur olockuj to jakos
        }
        std::fstream& blocksFile = ConfigLoader::getInstance()->getBlocks();
        unsigned long host_file_offset = indirect_block * 4096 + (number_of_blocks - 12) * sizeof(unsigned);
        blocksFile.seekp(host_file_offset);
        blocksFile.write((char*)&block, sizeof(unsigned));
    }
    number_of_blocks++;
    return 0;
}

int INode::removeBlock() {
    if(number_of_blocks <= 12) {
        ConfigLoader::freeBlock(blocks[number_of_blocks - 1]);
        blocks[number_of_blocks - 1] = 0;
    }
    else {
        std::fstream& blocksFile = ConfigLoader::getInstance()->getBlocks();
        unsigned long host_file_offset = indirect_block * 4096 + (number_of_blocks - 13) * sizeof(unsigned);
        blocksFile.seekg(host_file_offset);
        unsigned block;
        blocksFile.read((char*)&block, sizeof(unsigned ));
        ConfigLoader::freeBlock(block);
        if (number_of_blocks == 13) {
            ConfigLoader::freeBlock(indirect_block);
            indirect_block = 0;     // TODO Artur zwolnij z tego locka czy cos nie znam sie XD
        }
    }
    number_of_blocks--;
    return 0;
}

int INode::freeAllBlocks() {
    while (number_of_blocks > 0)
        removeBlock();
    return 0;
}

std::map<std::string, unsigned> INode::getDirectoryContent() {
//    if(!mode)
//        throw std::runtime_error("INode is not a directory");

    std::map<std::string, unsigned> dir_content;
    std::vector<char> inode_content = getContent();
    ConfigLoader * config = ConfigLoader::getInstance();
    int maxFileName = config->getMaxLengthOfName();
    for(unsigned long i=0; i<length; i += maxFileName+sizeof(unsigned)){
            std::string name = (inode_content.data() + i);
            unsigned inode_id = 0;
            std::memcpy(&inode_id, inode_content.data()+i+maxFileName, sizeof(unsigned));
            dir_content.insert(std::make_pair(name, inode_id));
    }
    return dir_content;
}

std::vector<char> INode::getContent() {
    ConfigLoader * config = ConfigLoader::getInstance();
    std::fstream & blocks_stream = config->getBlocks();
    int sizeOfBlock = config->getSizeOfBlock();
    std::vector<char> content;
    unsigned long loaded = 0;
    char block_content[sizeOfBlock];
    //todo: pewnie trzeba tu zrobić blokady
    for(auto & a : blocks){
        blocks_stream.seekg(a * sizeOfBlock);
        if(length - loaded < sizeOfBlock){
            if(length - loaded) {
                blocks_stream.read(block_content, length - loaded);
                content.insert(content.end(), block_content, block_content+length-loaded);
            }
            break;
        }
        blocks_stream.read(block_content, sizeOfBlock);
        content.insert(content.end(), block_content, block_content+sizeOfBlock);
        loaded += sizeOfBlock;
    }
    return content;
}

unsigned int INode::getId() const {
    return inode_id;
}

void INode::saveINodeInDirectory(std::string newFileName, INode newFileInode) {
    addFileToDirectory(newFileName, newFileInode);
    ConfigLoader * config = ConfigLoader::getInstance();
    std::fstream & inodes = config->getInodes();
    inodes.seekg(newFileInode.getId()*sizeofInode);
    inodes << std::make_shared<INode>(newFileInode);
}

std::array<char, INode::sizeofInode> INode::serialize() {
    std::array<char, sizeofInode> INodeBytes = {0};
    int i = 0;

    for(; i<sizeof(mode); ++i)
        INodeBytes[i] = (mode >> (i * 8));

    for(int k=0; i<sizeof(length); ++i, ++k)
        INodeBytes[i] = (length >> (k * 8));

    for(int k=0; i<sizeof(number_of_blocks); ++i, ++k)
        INodeBytes[i] = (number_of_blocks >> (k * 8));

    for(auto & block : blocks)
        for(int k =0; i<sizeof(block); ++i, ++k)
            INodeBytes[i] = (block >> (k * 8));

    for(int k =0; i<sizeof(indirect_block); ++i, ++k)
        INodeBytes[i] = (indirect_block >> (k * 8));

    return INodeBytes;
}

void INode::addFileToDirectory(std::string newFileName, INode inode) {
    ConfigLoader* loader = ConfigLoader::getInstance();
    if(newFileName.size() > loader->getMaxLengthOfName())
        throw std::runtime_error("Bad length of file name");
    std::fstream& blocks = loader->getBlocks();
    unsigned positionInBlock = length%loader->getSizeOfBlock();
    unsigned blockId = length/loader->getSizeOfBlock();
    char *savedFileName = new char[loader->getMaxLengthOfName()];
    newFileName.copy(savedFileName, newFileName.size());
    if(newFileName.size() != loader->getMaxLengthOfName())
        savedFileName[newFileName.size()] = 0;
    for (int i = newFileName.size(); i < loader->getMaxLengthOfName(); i++) {
        savedFileName[i] = 0;
    }
    unsigned inodeId = inode.getId();
    blocks.seekg(getBlock(blockId)*loader->getSizeOfBlock()+positionInBlock);
    blocks.write(savedFileName, loader->getMaxLengthOfName());
    blocks.write((char*)&(inodeId), sizeof(inodeId));
    delete savedFileName;
    length += loader->getMaxLengthOfName() + sizeof(inodeId);
    std::fstream& inodes = loader->getInodes();
    writeInode();
}


/**
 * Saves INode to file and sets bit in bitmap to inform that INode is in use.
 *
 * @param fd - file descriptor holding INode number of file to save
 * @param inode - object to save
 * @return
 */
int INode::writeInode() {
    ConfigLoader* loader = ConfigLoader::getInstance();
    std::fstream &ofs = loader->getInodes();
    ofs.seekp(getId()*INode::sizeofInode);
    ofs << this;

    // update bitmap
    // TODO maybe split into updateInode (wont change bitmap) and createInode, which will update bitmap
    std::fstream &inodesBitmap = loader->getInodesBitmap();
    inodesBitmap.seekg(getId()/8);
    char byte;
    inodesBitmap.read(&byte, 1);
    byte |= 1 << (getId()%8);
    inodesBitmap.seekp(getId()/8);
    inodesBitmap.write(&byte, 1);
    // TODO return errors ( + doc)
}

void INode::writeToFile(char *buffer, int size, long fileCursor) {
    ConfigLoader * config = ConfigLoader::getInstance();
    std::fstream & blocks_stream = config->getBlocks();
    const int sizeOfBlock = config->getSizeOfBlock();
    long positionInBlock = fileCursor%sizeOfBlock;
    int saved = 0;
    for(int blockIndex = fileCursor/sizeOfBlock; size > 0; blockIndex++){
        int blockAddress = blocks[blockIndex];
        blocks_stream.seekg(blockAddress*sizeOfBlock+positionInBlock);
        int remainingSizeOfBlock = sizeOfBlock-positionInBlock;
        if(remainingSizeOfBlock<=size){ //TODO Check equals
            blocks_stream.write(buffer + saved, remainingSizeOfBlock);
            saved += remainingSizeOfBlock;
            size -= remainingSizeOfBlock;
        }
        else {
            blocks_stream.write(buffer + saved, size);
            size = 0;
        }
        positionInBlock=0;
    }
}
