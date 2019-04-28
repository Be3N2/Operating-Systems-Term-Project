//details on pack code here https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html
// __operator__ ((packed))
//#pragma pack(pop)

struct __attribute__((packed)) headerDescriptor {
    char preheader[64];
    char imageSignature[4];
    float versionNum;
    unsigned int sizeOfHeader;
    unsigned int imageType;
    int imageFlags;
    char imageDescription[256];//96-336
    int offsetBlocks;
    int offsetData;
    int numOfCylinders;
    int numOfHeads;
    int numOfSectors;
    int sectorSize; //2 ^ 9
    //4 char unused
    int unused;
    long long int diskSize; // 2 ^ 27
    unsigned int blockSize; // 2 ^ 20 1MB
    int blockExtraData;
    unsigned int numOfBlocksInHDD;
    unsigned int numOfBlocksAllocated;
    char UUID[16];
    char UUIDLastSnap[16];
    char UUIDLink[16];
    char Parent[16];
    //read garbage until next MB
};

struct secondDescriptor {
    headerDescriptor hd;
    int fd;
    int cursor;
};

struct __attribute__((packed)) partitionTable { //16 byte partition table entry
    // https://thestarman.pcministry.com/asm/mbr/PartTables.htm
    char bootIndicator;
    char startingCHS[3];
    char partitionTypeDescriptor;
    char endingCHS[3];
    int startingSector;
    int partitionSize;
};

struct __attribute__((packed)) superBlock {
    unsigned int inodesCount;
    unsigned int blocksCount;
    unsigned int reservedBlockCount;
    unsigned int freeBlocks;
    unsigned int freeInodesCount;
    unsigned int firstDataBlock;
    unsigned int blockSize;
    unsigned int fragmentSize;
    unsigned int numOfBlocksPerGroup;
    unsigned int numOfFragmentsPerGroup;
    unsigned int numOfInodesPerGroup;
    unsigned int mountTime;
    unsigned int writeTime;
};

struct __attribute__((packed)) groupDesc {
    char blockBitmap[4];
    char inodeBitmap[4];
    unsigned int inodeTable;
    unsigned short int freeBlocksCount;
    unsigned short int freeInodesCount;
    unsigned short int usedDirsCount;
    unsigned short int pad;
    unsigned int reserved[3];
};
//fetchInode
//fetchBlockFromFile
//4 directory functions opendirectory close rewind getNext
//start with inode 2 traverse everything mark as used in separate inode table
//for every file and directory you encounter run through 
//data block bitmaps are packed in inodes - block group size is determined by block size exactly one block for bitmap every 
//block group isze is 8MB 8192 blocks represented
//can't pack inodeBitmap first blockgroup has 2032 inodes, its bitmap 
//(inode number - 1) / inodesperblockgroup) % inodesperblockgroup = blockgroup (%inodersperblockgroup gives where in blockgroup)
//take remainder divide by 8 to get byte within block >> left shift 1?
