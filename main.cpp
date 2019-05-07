
#include <fcntl.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/stat.h>

#include "math.h"

#include <iostream>
using namespace std;

#include "structs.h"

void openFile(char filename[], secondDescriptor &descriptor2);
void closeFile(secondDescriptor * descriptor2);

void VDIread(secondDescriptor &descriptor2, int nBytes, char *buf);
void VDIwrite(secondDescriptor &descriptor2, int nBytes, char *buf);

void VDIseek(secondDescriptor &descriptor2, int offset, int anchor);

void fetchBlock(int blockNum, char *buf, secondDescriptor &descriptor2);

void fetchSuperBlock(secondDescriptor &descriptor2, int nBytes, char *buf); 

void fetchInodeBlock(secondDescriptor &descriptor2, superBlock *firstSuper, int inodeNum, groupDesc *currentGroup, int blockNum, inode *inodeBuf);

void fetchBlockFromFile(inode *i, int b, char *buf, secondDescriptor &descriptor2);

void checkAllSupers(secondDescriptor &descriptor2, superBlock masterBlock);
bool compareSupers(superBlock super1, superBlock super2);

void setBit(char *a,int b) {(*a) |= (1 << (b));}

void openDir(secondDescriptor &descriptor2, superBlock *firstSuper, groupDesc *group);

//GLOBAL VARIABLES  :3       :)
int IONTS;//This is the offset number that needs added to the fetchblocks
int blockSize;
char map[(8192 * 16)/8];//(numOfBlocksPerGroup * numOfGroups) / blocksPerByte

int main(int argc, char *argv[])
{

    secondDescriptor descriptor2;

    openFile(argv[1], descriptor2);

    for (int i = 0;i < sizeof(descriptor2.hd.preheader); i++) {
        cout << descriptor2.hd.preheader[i];
    }
    
    cout << endl<<  "Size of Header: " << descriptor2.hd.sizeOfHeader << endl;
    cout << "Offset blocks: " << descriptor2.hd.offsetBlocks << endl;
    cout << "offset Data: " << descriptor2.hd.offsetData << endl;
    cout << "Disk size: " << descriptor2.hd.diskSize << endl;
    cout << "Block size: " << descriptor2.hd.blockSize << endl;
    cout << "Sector size: " << descriptor2.hd.sectorSize << endl;
    cout << "Num of Blocks in HDD: " << descriptor2.hd.numOfBlocksInHDD << endl;
    cout << "Num of Blocks Allocated: " << descriptor2.hd.numOfBlocksAllocated << endl;



    cout << "================ Partition Table ================ " << endl;
    
    char partBuffer[16];
    
    VDIseek(descriptor2, 446, -1); //446 from the start
    
    VDIread(descriptor2, sizeof(partBuffer), partBuffer);
    partitionTable *partT = (partitionTable*) partBuffer; 

    cout << "Starting Sector     " << dec << partT->startingSector << endl;
    cout << "Partition Size     " << partT->partitionSize << endl;
   
    //Important Offset
    IONTS = partT->startingSector * descriptor2.hd.sectorSize;
    int firstByte = descriptor2.hd.offsetData + IONTS;



    cout << endl <<  "================ Super Block ================" << endl;

    //fetchblock
    char superBuffer[1024];

    fetchSuperBlock(descriptor2, 1024, superBuffer);
    superBlock *firstSuper = (superBlock*) superBuffer;
    blockSize = 1024 << firstSuper->blockSize;

    //incomplete function
    checkAllSupers(descriptor2, *firstSuper);
     
    cout << "size of Super Block    " << sizeof(superBlock) << endl;
    cout << "Inodes count   " << firstSuper->inodesCount << endl;
    cout << "blocks count   " << firstSuper->blocksCount << endl;
    cout << "free blocks   " << firstSuper->freeBlocks << endl;
    cout << "block size   " << blockSize << endl;
    cout << "num of blocks per group   " << firstSuper->numOfBlocksPerGroup << endl;
    cout << "num of inodes per group   " << firstSuper->numOfInodesPerGroup << endl;
    cout << "MAGIC NUMBER     " << hex << firstSuper->magicSignature << dec << endl;



    cout << endl <<  "================ Group Descriptors ================" << endl;

    int numOfGroups = 1 + ((firstSuper->blocksCount - 1) / firstSuper->numOfBlocksPerGroup);
    cout << "num of Groups   " << numOfGroups << endl;
    
    int groupDescBufferSize = numOfGroups * sizeof(groupDesc); 
    cout << "Size of Descriptor   " << groupDescBufferSize << endl;
    
    char groupDescBuffer[groupDescBufferSize];

    VDIseek(descriptor2, (firstSuper->firstDataBlock+1) * blockSize + IONTS,-1);
    VDIread(descriptor2, groupDescBufferSize, groupDescBuffer);

    groupDesc groupDescriptors[numOfGroups]; //numOfGroups is 16
    groupDesc *groupdesc1 = groupDescriptors;
    groupdesc1 = (groupDesc*) groupDescBuffer;

    int totalBlocksUsed = 0;
    int totalInodesUsed = 0;
    int totalDirectories = 0;
    for (int i = 0; i < numOfGroups; i++) {
        cout << "Bitmap block num   " << (groupdesc1+i)->blockBitmap << endl;
        cout << "Free Blocks Count   " << (groupdesc1+i)->freeBlocksCount << endl;
        cout << "Free inodes count   " << (groupdesc1+i)->freeInodesCount << endl;
        cout << "Inode Table Num    " << (groupdesc1+i)->inodeTable << endl;
        cout << "Used Dirs count   " << (groupdesc1+i)->usedDirsCount << endl;
        totalBlocksUsed += firstSuper->numOfBlocksPerGroup - (groupdesc1+i)->freeBlocksCount;
        totalInodesUsed += firstSuper->numOfInodesPerGroup - (groupdesc1+i)->freeInodesCount;
        totalDirectories += (groupdesc1+i)->usedDirsCount;
    }


    cout << endl << "========= Group Descriptor totals =============" << endl;
    cout << "Total blocks used:    " << totalBlocksUsed << endl;
    cout << "Total inodes used:    " << totalInodesUsed << endl;
    cout << "Total directories used:    " << totalDirectories << endl;



    cout << endl <<  "================ Inodes ================" << endl;
   
    openDir(descriptor2, firstSuper, groupdesc1);

    closeFile(&descriptor2);
    return 0;
}

void openFile(char filename[], secondDescriptor &descriptor2) {
    int fd;

    //file descriptor = open
    fd = open(filename,O_RDWR);  
    //this is error catching, if its false, the file opens
    if(-1 == fd)            
    {
        //printf("\n open() failed with error [%s]\n",strerror(errno));
    }
    else
    {
        //set the file descriptor
        descriptor2.fd = fd;
        
        //read the header descriptor
        if (read(fd, &(descriptor2.hd), sizeof(descriptor2.hd)) < 0)
            cout << "ERROR"<< endl;
        
        //set cursor to 0
        descriptor2.cursor = 0;

    }
}

void closeFile(secondDescriptor * descriptor2) {
    //could close file too
    delete descriptor2;
}

void VDIseek(secondDescriptor &descriptor2, int offset, int anchor) {
    
    int newPosition = 0;
    if (anchor == -1) {// start
        newPosition = offset;
    } else if (anchor == 0) {//current
        newPosition = descriptor2.cursor + offset;
    } else if (anchor == 1) {//end
        newPosition = descriptor2.hd.diskSize + offset;
    } else {
        newPosition = descriptor2.cursor;
    }

    if (newPosition >= 0 && newPosition < descriptor2.hd.diskSize) {
        descriptor2.cursor = newPosition;
    }
}

void VDIread(secondDescriptor &descriptor2, int nBytes, char *buf) {

    int pos = descriptor2.hd.offsetData + descriptor2.cursor;

     lseek(descriptor2.fd, pos, SEEK_SET);

    if (read(descriptor2.fd, buf, nBytes) < 0)
            cout << "ERROR IN READ"<< endl;

}

void VDIwrite(secondDescriptor &descriptor2, int nBytes, char *buf) {
  
    int pos = descriptor2.hd.offsetData + descriptor2.cursor;

    lseek(descriptor2.fd, pos, SEEK_SET);
    
    if (write(descriptor2.fd, buf, nBytes) < 0)
        cout << "ERROR IN WRITE" << endl;

}

void fetchBlock(int blockNum, char *buf, secondDescriptor &descriptor2) {
    //cout << "FETCH BLOCK CALLED  Block Num   " << blockNum << endl;
    VDIseek(descriptor2,(blockNum * blockSize) + IONTS, -1);
    
    //setBit(map[g * blockSize + e/8], e % 8) set in byte, this bit
    VDIread(descriptor2, blockSize, buf);
}

//always 1024 bytes into the file system
void fetchSuperBlock(secondDescriptor &descriptor2, int nBytes, char *buf) {
    VDIseek(descriptor2,1024 + IONTS, -1);
    
    VDIread(descriptor2, nBytes, buf);
}

void openDir(secondDescriptor &descriptor2, superBlock *firstSuper, groupDesc *group) {
    cout << "Size of inode:    " << sizeof(inode) << endl;
    int inodesPerBlock = blockSize / sizeof(inode);
    cout << "Inodes Per Block    " << inodesPerBlock << endl;
    int blocksInItable = inodesPerBlock * firstSuper->numOfInodesPerGroup;
    cout << "Blocks in Itable    " << blocksInItable << endl;
    

    int startingInodeNum = 2;

    int groupNum = (startingInodeNum-1) / firstSuper->numOfInodesPerGroup; //g = e / epg
    int withinGroup = (startingInodeNum-1) % firstSuper->numOfInodesPerGroup;  //b = e % epg
    groupDesc *currentGroup = (group + groupNum);
    int blockNum = withinGroup / inodesPerBlock;
    int inodeNum = withinGroup % inodesPerBlock;
    cout << "Num of group      " << groupNum << endl;
    cout << "Within Group      " << withinGroup << endl;
    cout << "Block Number      " << blockNum << endl;
    cout << "Inode Num         " << inodeNum << endl;
    cout << "Inode table       " << currentGroup->inodeTable << endl; 
    /*prints out the iBlock numbers for reference
    for (int i = 0; i < 15; i++) {
        cout << (inodePtr + inodeNum)->iBlock[i] << endl;
    }
    */

    //mark off the first 10 bits
    for (int i= 0; i < 10; i++) {
        int byte = i / 8;
        setBit(&(map[byte]), i);
    }
    
    inode inodesArray[blockSize / sizeof(inode)];
    
    fetchInodeBlock(descriptor2, firstSuper, inodeNum, currentGroup, blockNum, inodesArray);
    
}

void fetchInodeBlock(secondDescriptor &descriptor2, superBlock *firstSuper, int inodeNum, groupDesc *currentGroup, int blockNum, inode *inodeBuf) {

    
    char buffer[blockSize];
    fetchBlock(currentGroup->inodeTable + blockNum, buffer, descriptor2);

    inodeBuf = (inode*) buffer;

    if (S_ISDIR((inodeBuf+inodeNum)->iMode)) cout << "IS A DIRECTORY" << endl;
    cout << "iMode     " <<  (inodeBuf + inodeNum)->iMode  << endl; 
    cout << "iUID      " << (inodeBuf + inodeNum)->iUID << endl; 
    cout << "iSize      " << (inodeBuf + inodeNum)->iSize << endl; 
    cout << "iBlocks      " << (inodeBuf + inodeNum)->iBlocks << endl; 
    
    cout << endl <<  "=============== ROOT DIRECTORY ENTRIES ================ " << endl;

    Entries rootEntries;
    rootEntries.parentNode = *(inodeBuf + inodeNum);
    rootEntries.cursor = 0;
    char entryBuf[blockSize];
    rootEntries.ptr = entryBuf;

    fetchBlockFromFile(&rootEntries.parentNode, 0, rootEntries.ptr, descriptor2);
    //currentGroup->usedDirsCount
    dirEntry temp;
    dirEntry *ptrCur = &temp;
    
    bool notFinished = false;
    int i = 24;
    int runTimes = 0;
    while(runTimes < currentGroup->usedDirsCount -2) {
        ptrCur = (dirEntry*) (rootEntries.ptr + i);
        cout <<"Inode Number:    " <<  ptrCur->inodeNum << endl;
        cout <<"Directory Length:    " <<  ptrCur->directoryLength << endl;
        cout <<"Name Length:    " << (int) ptrCur->nameLength << endl;
        cout <<"File Type:    " << (int) ptrCur->fileType << endl;

        cout <<"File Name:   " << rootEntries.ptr + i + 8 << endl << endl;

        i += ptrCur->directoryLength;
        runTimes++;
    }
}

void fetchBlockFromFile(inode *i, int b, char *buf, secondDescriptor &descriptor2) {
    
    int ipb = 1024 / 4;//indexes per block
    int selected = 0;

    if (b < 12) {
        //direct pointer
        //list = i_block?
        goto direct;
    }
    b -= 12;

    if (b < ipb) {
        //list = i_block + 12;
        goto single;
    }
    b -= ipb;
    if (b < ipb * ipb) {
        //list = i_block + 13;
        goto doubleI;
    }
    b-= ipb * ipb;
    //list = i_block + 14
    triple:
        selected = b / (ipb*ipb*ipb);
        fetchBlock(i->iBlock[selected], buf, descriptor2);
        i = (inode*) buf;
        b = b%(ipb * ipb);
    doubleI:
        selected = b/(ipb*ipb);
        fetchBlock(i->iBlock[selected], buf, descriptor2);
        i = (inode *) buf;
        b = b%(ipb * ipb);
    single: 
        fetchBlock(i->iBlock[b], buf, descriptor2);
        i = (inode*) buf;
        b = b % ipb;
    direct:
        fetchBlock(i->iBlock[b], buf, descriptor2);
    
}

//Did not complete:
void checkAllSupers(secondDescriptor &descriptor2, superBlock masterBlock) {

    int start = 1;
    char buffer[blockSize];
    char *bufPtr = buffer;

    while (pow(3, start) < masterBlock.blocksCount) {
        
        fetchBlock(pow(3, start), bufPtr, descriptor2);
        superBlock *test = (superBlock*) bufPtr;
        compareSupers(masterBlock, *test);
        start++;
    }
    start = 1;  
    while (pow(5, start) < masterBlock.blocksCount) {
        start++;
    }
    start = 1;
    while (pow(7, start) < masterBlock.blocksCount) {
        start++;
    }
}

bool compareSupers(superBlock super1, superBlock super2) {
    bool returnVal = true;
    if (super1.inodesCount != super2.inodesCount) returnVal = false ;
    if (super1.blocksCount != super2.blocksCount) returnVal = false ;
    if (super1.reservedBlockCount != super2.reservedBlockCount) returnVal = false ;
    if (super1.freeBlocks != super2.freeBlocks) returnVal = false ;
    if (super1.freeInodesCount != super2.freeInodesCount) returnVal = false ;

    return returnVal;
}


//SOME NOTES:

//allocate a chunk of memory one block per blocksize * numberOfBlockGroups in size
//initialize it all to 0
//itemNumber (subtract one if inode) / itemsperBlockGroup (both values are in super block)
//itemNumber / itemsperBlockGroup = quotient is block group
//remainder is where in the block group to be marked (/8 is byte %8)

//fetchblockfrom file (j = 0; j * blocksize < filesize; j++) block number you get mark it
//in fetchblock mark off block in bitmap - will take care of all the file blocks
//theres copies of the superblock, descriptors, setofinodetables, reservedblock in inode 7 that are used
//inodes 1-10 1 is bad block inode 1-10 are not reachable except 2 
//for loop 1-10 and add in those to the bitmap 

