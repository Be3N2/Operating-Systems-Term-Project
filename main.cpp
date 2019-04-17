//here is wehre I got this from:
//https://www.ibm.com/developerworks/community/blogs/58e72888-6340-46ac-b488-d31aa4058e9c/entry/understanding_linux_open_system_call?lang=en

#include <fcntl.h>
#include <unistd.h>
 
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
using namespace std;

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
    unsigned int blockSize; // 2 ^ 20
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


//open Takes file name, returns pointer to second struct
void openFile(char filename[], secondDescriptor &descriptor2);
void closeFile(secondDescriptor * descriptor2);
//two assumptions - fixed size files, never read more than 4kb
void VDIread(secondDescriptor &descriptor2, int nBytes, char *buf[]);
void VDIwrite(secondDescriptor &descriptor2, int nBytes, char *buf[]);

void VDIseek(secondDescriptor &descriptor2, int offset, int anchor);

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
    cout << "num of cylinders: " << descriptor2.hd.numOfCylinders << endl;
    cout << "num of heads: " << descriptor2.hd.numOfHeads << endl;
    cout << "num of sectors: " << descriptor2.hd.numOfSectors << endl;
    cout << "Disk size: " << descriptor2.hd.diskSize << endl;
    cout << "Block size: " << descriptor2.hd.blockSize << endl;
    cout << "Sector size: " << descriptor2.hd.sectorSize << endl;
    cout << "Num of Blocks in HDD: " << descriptor2.hd.numOfBlocksInHDD << endl;
    cout << "Num of Blocks Allocated: " << descriptor2.hd.numOfBlocksAllocated << endl;
    

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
    if (anchor == 0) {
        //seek start

        descriptor2.cursor = offset;

        //lseek(descriptor2.fd, descriptor2.cursor, SEEK_SET);
    } else if (anchor == -1) {
        //seek end
        descriptor2.cursor = descriptor2.hd.diskSize + descriptor2.hd.offsetBlocks * descriptor2.hd.blockSize;
        //hopefully negative
        if (descriptor2.cursor > 0 && descriptor2.cursor < descriptor2.hd.diskSize) {
            //return cursor?
        }
    } else {
        //seek to offset
        descriptor2.cursor = descriptor2.hd.offsetBlocks * descriptor2.hd.blockSize + anchor;
    }
    //if new position is not negative and less than disk size, then set cursor to newposition

}

void VDIread(secondDescriptor &descriptor2, int nBytes, char *buf[]) {

    //fetchblock is VDI seek followed by VDI Read

    //someone else to do VDIseek already
    //VDIseek(descriptor2, pos);

    int pos =  descriptor2.hd.offsetData + cursor;

    //lseek to offsetData + cursor 
    //use cursor and lseek
    lseek(descriptor2.fd, pos, SEEK_SET);

    if (read(descriptor2.fd, buf, nBytes) < 0)
            cout << "ERROR IN READ"<< endl;
}

void VDIwrite(secondDescriptor &descriptor2, int nBytes, char *buf[]) {
    //VDIseek(descriptor2, pos);
  
    int pos = descriptor2.hd.offsetData + descriptor2.cursor;

    lseek(descriptor2.fd, pos, SEEK_SET);
    
    if (write(descriptor2.fd, buf, nBytes) < 0)
        cout << "ERROR IN WRITE" << endl;

}

//first 4 entries 16 bytes of the first sector
//first sector number is 4 bytes and 4 byte sector count after 8 bytes
//read table in look for partition whose type (byte 4 should be 0x83 meaning linux)
//take first sector number and multiply by 512 and save that number
//offsetData + that number is first byte of the file system
//fetchblock(blockNumber * blockSize + NUMBER)

//Next after VDI
//fetchBlock()
//fetchSuperBlock()
//read in the superblock
//partition table?