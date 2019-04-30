//here is wehre I got this from:
//https://www.ibm.com/developerworks/community/blogs/58e72888-6340-46ac-b488-d31aa4058e9c/entry/understanding_linux_open_system_call?lang=en

#include <fcntl.h>
#include <unistd.h>
 
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
using namespace std;

#include "structs.h"

//open Takes file name, returns pointer to second struct
void openFile(char filename[], secondDescriptor &descriptor2);
void closeFile(secondDescriptor * descriptor2);
//two assumptions - fixed size files, never read more than 4kb
void VDIread(secondDescriptor &descriptor2, int nBytes, char *buf);
void VDIwrite(secondDescriptor &descriptor2, int nBytes, char *buf);

void VDIseek(secondDescriptor &descriptor2, int offset, int anchor);

//takes a block number returns pointer to the data
void fetchBlock(int blockNum, secondDescriptor &descriptor2, int offsetIONTS, int nBytes, char *buf);

void fetchSuperBlock(secondDescriptor &descriptor2, int offsetIONTS, int nBytes, char *buf); 


int main(int argc, char *argv[])
{

    secondDescriptor descriptor2;

    openFile(argv[1], descriptor2);

    for (int i = 0;i < sizeof(descriptor2.hd.preheader); i++) {
        cout << descriptor2.hd.preheader[i];
    }
    for (int i = 0;i < sizeof(descriptor2.hd.imageSignature); i++) {
        cout << descriptor2.hd.imageSignature[i];
    }
    cout << "Version num: " << descriptor2.hd.versionNum << endl;
    cout << "Size of Header: " << descriptor2.hd.sizeOfHeader << endl;
    cout << "Image Type: " << descriptor2.hd.imageType << endl;
    cout << "image Flags: " << descriptor2.hd.imageFlags << endl;
    cout << "image Description";
    for (int i = 0;i < sizeof(descriptor2.hd.imageDescription); i++) {
        cout << descriptor2.hd.imageDescription[i];
    }
    cout << endl << "Offset blocks: " << descriptor2.hd.offsetBlocks << endl;
    cout << "offset Data: " << descriptor2.hd.offsetData << endl;
    cout << "Disk size: " << descriptor2.hd.diskSize << endl;
    cout << "Block size: " << descriptor2.hd.blockSize << endl;
    cout << "Sector size: " << descriptor2.hd.sectorSize << endl;
    cout << "Num of Blocks in HDD: " << descriptor2.hd.numOfBlocksInHDD << endl;
    cout << "Num of Blocks Allocated: " << descriptor2.hd.numOfBlocksAllocated << endl;
    

    //read in partition table into *partT
    //partitionTable partitionT;
    //partitionT = *partT;

    char partBuffer[16];
    
    VDIseek(descriptor2, 446, -1); //446 from the start
    
    VDIread(descriptor2, sizeof(partBuffer), partBuffer);
    partitionTable *partT = (partitionTable*) partBuffer; 

    cout << "Starting Sector     " << dec << partT->startingSector << endl;
    cout << "Partition Size     " << partT->partitionSize << endl;
   
    //int importantOffsetNumberThingOnTheSide = 
    int IONTS = partT->startingSector * descriptor2.hd.sectorSize;
    int firstByte = descriptor2.hd.offsetData + IONTS;
    //fetchblock(blockNumber * descriptor2.hd.blockSize + IONTS)

    cout << endl <<  "================ Super Block ================" << endl;

    //fetchblock
    char superBuffer[1024];

    fetchSuperBlock(descriptor2, IONTS, 1024, superBuffer);
    superBlock *firstSuper = (superBlock*) superBuffer;
    firstSuper->blockSize = 1024 << firstSuper->blockSize;

    cout << "Inodes count   " << firstSuper->inodesCount << endl;
    cout << "blocks count   " << firstSuper->blocksCount << endl;
    cout << "free blocks   " << firstSuper->freeBlocks << endl;
    cout << "block size   " << firstSuper->blockSize << endl;
    cout << "num of blocks per group   " << firstSuper->numOfBlocksPerGroup << endl;
    cout << "num of inodes per group   " << firstSuper->numOfInodesPerGroup << endl;

    int numOfGroups = 1 + ((firstSuper->blocksCount - 1) / firstSuper->numOfBlocksPerGroup);
    cout << "num of Groups   " << numOfGroups << endl;
    
    int groupDescBufferSize = numOfGroups * sizeof(groupDesc); 
    cout << "Size of Descriptor   " << groupDescBufferSize << endl;
    
    char groupDescBuffer[groupDescBufferSize];
    VDIseek(descriptor2,1024 + firstSuper->blockSize, -1);
    VDIread(descriptor2, groupDescBufferSize, groupDescBuffer);

    groupDesc groupDescriptors[numOfGroups]; //numOfGroups is 16
    groupDesc *groupdesc1 = &groupDescriptors[0];
    groupdesc1 = (groupDesc*) groupDescBuffer;

    cout << endl <<  "================ Group Descriptors ================" << endl;
    for (int i = 0; i < numOfGroups; i++) {
        cout << "Free blocks count   " << groupDescriptors[i].freeBlocksCount << endl;
        cout << "Free inodes count   " << groupDescriptors[i].freeInodesCount << endl;
        cout << "Used dir count   " << groupDescriptors[i].usedDirsCount << endl;
    }

    for (int i = 0; i < sizeof(groupDescriptors[0].blockBitmap) * 8; i++) {
        int index = i / 8;
        unsigned int value = (int) groupDescriptors[0].blockBitmap[index];
        cout << (value >> (i % 8)) % 2;
    }


    cout << endl <<  "================ Inodes ================" << endl;
    cout << "Size of inode:    " << sizeof(inode) << endl;
    closeFile(&descriptor2);
    return 0;
}

//C:\Users\2017W\Documents\compSci\Operating-Systems-Term-Project\VDITestFiles\Good\Test-fixed-1k.vdi

void openFile(char filename[], secondDescriptor &descriptor2) {
    //file descriptor
    int fd;

    //file descriptor = open
    fd = open(filename,O_RDWR);  
    //this is erroe catching, if its false, the file opens
    if(-1 == fd)            
    {
        //return null pointer
        //printf("\n open() failed with error [%s]\n",strerror(errno));
    }
    else
    {
        //set the filed descriptor
        descriptor2.fd = fd;
        
        //read the header descriptor
        if (read(fd, &(descriptor2.hd), sizeof(descriptor2.hd)) < 0)
            cout << "ERROR"<< endl;
        
        //set cursor to 0
        descriptor2.cursor = 0;

        //return pointer
        //return null pointer if anything went wrong
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
    //return cursor?
}

void VDIread(secondDescriptor &descriptor2, int nBytes, char *buf) {

    //fetchblock is VDI seek followed by VDI Read

    //someone else to do VDIseek already
    //VDIseek(descriptor2, pos);

    int pos = descriptor2.hd.offsetData + descriptor2.cursor;

    //lseek to offsetData + cursor 
    //use cursor and lseek
    lseek(descriptor2.fd, pos, SEEK_SET);

    if (read(descriptor2.fd, buf, nBytes) < 0)
            cout << "ERROR IN READ"<< endl;

    //return buf;
}


void VDIwrite(secondDescriptor &descriptor2, int nBytes, char *buf) {
    //VDIseek(descriptor2, pos);
  
    int pos = descriptor2.hd.offsetData + descriptor2.cursor;

    lseek(descriptor2.fd, pos, SEEK_SET);
    
    if (write(descriptor2.fd, buf, nBytes) < 0)
        cout << "ERROR IN WRITE" << endl;

}

void fetchBlock(int blockNum, secondDescriptor &descriptor2, int offsetIONTS, int nBytes, char *buf) {
    VDIseek(descriptor2,blockNum * descriptor2.hd.blockSize + offsetIONTS, -1);
    
    VDIread(descriptor2, nBytes, buf);
}

//always 1024 bytes into the file system
void fetchSuperBlock(secondDescriptor &descriptor2, int offsetIONTS, int nBytes, char *buf) {
    VDIseek(descriptor2,1024 + offsetIONTS, -1);
    
    VDIread(descriptor2, nBytes, buf);
}

//first 4 entries 16 bytes of the first sector
//first sector number is 4 bytes and 4 byte sector count after 8 bytes
//read table in look for partition whose type (byte 4 should be 0x83 meaning linux)
//take first sector number and multiply by 512 and save that number
//offsetData + that number is first byte of the file system
//fetchblock(blockNumber * blockSize + NUMBER)

//blocks will only be used for one thing
//if a block has inodes mark it as used

//allocate a chunk of memory one block per blocksize * numberOfBlockGroups in size
//initialize it all to 0
//itemNumber (subtract one if inode) / itemsperBlockGroup (both values are in super block)
//itemNumber / itemsperBlockGroup = quotient is block group
//remainder is where in the block group to be marked (/8 is byte %8)

//fetchblockfrom file (j = 0; j * blocksize < filesize; j++) block number you get mark it
//in fetchblock mark off block in bitmap - will take car of all the file blocks
//theres copies of the superblock, descriptors, setofinodetables, reservedblock in inode 7 that are used
//inodes 1-10 1 is bad block inode 1-10 are not reachable except 2 
//for loop 1-10 and add in those to the bitmap 

