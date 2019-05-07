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
};

struct secondDescriptor {
    headerDescriptor hd;
    int fd;
    int cursor;
};

struct __attribute__((packed)) partitionTable { 
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
    unsigned int blockSize;//log of blockSize
    int fragmentSize;//log of fragment size
    unsigned int numOfBlocksPerGroup;
    unsigned int numOfFragmentsPerGroup;
    unsigned int numOfInodesPerGroup;
    unsigned int mountTime;
    unsigned int writeTime;
    unsigned short int mountCount;
    short int maxMountCount;
    unsigned short int magicSignature;
    unsigned short int state;
    unsigned short int errors;
    unsigned short int minorRevisionLevel;
    unsigned int timeOfLastCheck;
    unsigned int checkInterval;
    unsigned int creatorOS;
    unsigned int revisionLevel;
    unsigned short int defaultUID;
    unsigned short int deafultGID;

};

struct groupDesc {
    unsigned int blockBitmap;
    unsigned int inodeBitmap;
    unsigned int inodeTable;
    unsigned short int freeBlocksCount;
    unsigned short int freeInodesCount;
    unsigned short int usedDirsCount;
    unsigned short int pad;
    unsigned int reserved[3];
};

struct __attribute__((packed)) inode {
    unsigned short int iMode;
    unsigned short int iUID;
    unsigned int iSize;
    unsigned int iAtime;
    unsigned int iCtime;
    unsigned int iMtime;
    unsigned int iDtime;
    unsigned short int iGid;
    unsigned short int intiLinksCount;
    unsigned int iBlocks;
    unsigned int iFlags;
    unsigned int iReserved1;
    int iBlock[15];
    float iGeneration;
    unsigned int iFileACL;
    unsigned int iDirACL;
    unsigned int iFaddr;
    char iFrag;
    char iFsize;
    unsigned short int pad;
    unsigned short int iUIDHigh;
    unsigned short int iGIDHigh;
    unsigned int iReserved2;
};

struct Entries {
    inode parentNode;
    int cursor;
    char *ptr;
};

struct dirEntry {
    unsigned int inodeNum;
    unsigned short int directoryLength;
    unsigned char nameLength;
    unsigned char fileType;
    char name[1];
};

//More Notes:

//loop to handle lost and found inside the loop loop as long as cursor is less than file size, if fall out of loop return 0 for false
//inside loop pointer to dir entry = start of array plus cursor
//if inode is 0 continue the loop

//block group isze is 8MB 8192 blocks represented
//can't pack inodeBitmap first blockgroup has 2032 inodes, its bitmap 
//(inode number - 1) / inodesperblockgroup) % inodesperblockgroup = blockgroup (%inodersperblockgroup gives where in blockgroup)
//take remainder divide by 8 to get byte within block >> left shift 1?
