//Class representing i-node
#ifndef I_NODE_HPP
#define I_NODE_HPP
#include <iostream>
#include <array>

class INode {
private:
    const static unsigned maxBlocksNo = 12;

    unsigned short mode;
    long length;
    unsigned number_of_blocks;
    std::array<unsigned, maxBlocksNo> blocks = {0};
    unsigned indirect_block;

    friend std::istream& operator>>(std::istream& is, INode& en);
    friend std::ostream& operator<<(std::ostream& os, const INode& en);
public:
    //methods declarations
    INode() = default;
    INode(unsigned short mode, long length, unsigned int numberOfBlocks, unsigned int indirectBlock);
    int addBlock(unsigned block);
    int removeBlock(unsigned block);
    unsigned getBlock(unsigned index) const { if(index<blocks.size()) return blocks[index]; else return 0; };    // error returns 0 because block 0 is reserved for root
    long getLength() const { return length; };
};

#endif