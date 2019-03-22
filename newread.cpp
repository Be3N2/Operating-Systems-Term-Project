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
    unsigned int numOfCylinders;
    unsigned int numOfHeads;
    unsigned int numOfSectors;
    unsigned int sectorSize;
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

void readHeaderDescriptor(int fd, headerDescriptor hd);

int main(int argc, char *argv[])
{

    //file descriptor
    int fd;

    //if no no arguments, then print something
    if(2 != argc)
    {
        printf("\n Usage :  \n");
        return 1;
    }

    //error number
    errno = 0;

    //file descriptor = open
    fd = open(argv[1],O_RDWR);  
    //this is erroe catching, if its false, the file opens
    if(-1 == fd)            
    {
        printf("\n open() failed with error [%s]\n",strerror(errno));
        return 1;
    }
    else
    {
        printf("\n Open() Successful\n");
        /* open() succeeded, now one can do read operations
           on the file opened since we opened it in read-only
           mode. Also once done with processing, the file needs
           to be closed. Closing a file can be achieved using
           close() function. */

        readHeaderDescriptor(fd, descriptor1);
    }

    return 0;
}

//https://stackoverflow.com/questions/32717269/how-to-read-an-integer-and-a-char-with-read-function-in-c
void readHeaderDescriptor(int fd, headerDescriptor hd) {
    char test[1000];
    if (read(fd, test, sizeof(test)) < 0) 
        cout << "Error in the read" << endl;

    for (int i = 0; i < sizeof(hd.preheader); i++) {
        hd.preheader[i] = test[i];
        cout << hd.preheader[i];
    }

    cout << endl;

    for (int i = 0; i < sizeof(hd.imageSignature); i++) {
        hd.imageSignature[i] = test[i] + sizeof(hd.preheader);
        cout << (char)((int)hd.imageSignature[i]);
    }
    //7c7c7c60

    /*
    read(fd, hd.preheader, sizeof(hd.preheader) );
    read(fd, hd.name, sizeof(hd.name));
    char intBuffer[4];
    read(fd, intBuffer, sizeof(hd.versionNum));
    hd.versionNum = atoi(intBuffer);

    read(fd, intBuffer, sizeof(hd.sizeOfHeader));
    hd.sizeOfHeader = atoi(intBuffer);

    read(fd, intBuffer, sizeof(hd.imageType));
    hd.imageType = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.imageFlags));
    hd.imageFlags = atoi(intBuffer);
    read(fd, hd.imageDescription, sizeof(hd.imageDescription));
    read(fd, intBuffer, sizeof(hd.offsetBlocks));
    hd.offsetBlocks = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.offsetData));
    hd.offsetData = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.numOfCylinders));
    hd.numOfCylinders = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.numOfHeads));
    hd.numOfHeads = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.numOfSectors));
    hd.numOfSectors = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.sectorSize));
    hd.sectorSize = atoi(intBuffer);

    int unused;
    read(fd, intBuffer, sizeof(unused));
    //long long int
    read(fd, intBuffer, sizeof(hd.diskSize));
    hd.diskSize = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.blockSize));
    hd.blockSize = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.blockExtraData));
    hd.blockExtraData = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.numOfBlocksInHDD));
    hd.numOfBlocksInHDD = atoi(intBuffer);
    read(fd, intBuffer, sizeof(hd.numOfBlocksAllocated));
    hd.numOfBlocksAllocated = atoi(intBuffer);
    read(fd, hd.UUID, sizeof(hd.UUID));
    read(fd, hd.UUIDLastSnap, sizeof(hd.UUIDLastSnap));
    read(fd, hd.UUIDLink, sizeof(hd.UUIDLink));
    read(fd, hd.Parent, sizeof(hd.Parent));
    */
    //garbage!!!!!!!
}