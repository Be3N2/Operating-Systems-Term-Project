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
#pragma pack(push, 1)
// __operator__ ((packed))
struct headerDescriptor {
    char preheader[64];
    char imageSignature[4];
    float versionNum;
    unsigned int sizeOfHeader;
    int imageType;
    int imageFlags;
    char imageDescription[32];
    int offsetBlocks;
    int offsetData;
    int numOfCylinders;
    int numOfHeads;
    int numOfSectors;
    int sectorSize;
    //4 char unused
    int unused;
    long long int diskSize;
    unsigned int blockSize;
    int blockExtraData;
    unsigned int numOfBlocksInHDD;
    unsigned int numOfBlocksAllocated;
    char UUID[16];
    char UUIDLastSnap[16];
    char UUIDLink[16];
    char Parent[16];
    //read garbage until next MB
};
#pragma pack(pop)

#pragma pack(push, 1)
struct secondDescriptor {
    headerDescriptor hd;
    int fd;
    int cursor;
};
#pragma pack(pop)

headerDescriptor descriptor1;
headerDescriptor& ref = descriptor1;
secondDescriptor descriptor2;
secondDescriptor& refToDescriptor2 = descriptor2;

//open Takes file name, returns pointer to second struct
void openFile(char filename[], secondDescriptor &refToDescriptor2);
//void closeFile(secondDescriptor * refToDescriptor2);
//read
//seek
//close

int main(int argc, char *argv[])
{
    openFile(argv[1], refToDescriptor2);
    cout << "SIZE OF " << sizeof(refToDescriptor2.hd);
    for (int i = 0;i < sizeof(refToDescriptor2.hd.preheader); i++) {
        cout << refToDescriptor2.hd.preheader[i];
    }
    for (int i = 0;i < sizeof(refToDescriptor2.hd.imageSignature); i++) {
        cout << refToDescriptor2.hd.imageSignature[i];
    }
    cout << "Version num: " << refToDescriptor2.hd.versionNum << endl;
    cout << "Size of Header: " << refToDescriptor2.hd.sizeOfHeader << endl;
    cout << "Image Type: " << refToDescriptor2.hd.imageType << endl;
    cout << "image Flags: " << refToDescriptor2.hd.imageFlags << endl;
    cout << "image Description";
    for (int i = 0;i < sizeof(refToDescriptor2.hd.imageDescription); i++) {
        cout << refToDescriptor2.hd.imageDescription[i];
    }
    cout << endl << "Offset blocks: " << refToDescriptor2.hd.offsetBlocks << endl;
    cout << "offset Data: " << refToDescriptor2.hd.offsetData << endl;
    cout << "num of cylinders: " << refToDescriptor2.hd.numOfCylinders << endl;
    cout << "num of heads: " << refToDescriptor2.hd.numOfHeads << endl;
    cout << "num of sectors: " << refToDescriptor2.hd.numOfSectors << endl;
    cout << "Disk size: " << refToDescriptor2.hd.diskSize << endl;
    cout << "Block size: " << refToDescriptor2.hd.blockSize << endl;
    cout << "Sector size: " << refToDescriptor2.hd.sectorSize << endl;
    //closeFile(refToDescriptor2);
    return 0;
}

//C:\Users\2017W\Documents\compSci\Operating-Systems-Term-Project\VDITestFiles\Good\Test-fixed-1k.vdi

void openFile(char filename[], secondDescriptor &refToDescriptor2) {
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
        refToDescriptor2.fd = fd;
        
        //read the header descriptor
        if (read(fd, &(refToDescriptor2.hd), sizeof(refToDescriptor2.hd)) < 0)
            cout << "ERROR"<< endl;
        
        //set cursor to 0
        refToDescriptor2.cursor = 0;

        //return pointer
        //return null pointer if anything went wrong
    }
}

void closeFile(secondDescriptor * refToDescriptor2) {
    //could close file too
    delete refToDescriptor2;
}

//https://stackoverflow.com/questions/32717269/how-to-read-an-integer-and-a-char-with-read-function-in-c