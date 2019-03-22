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
headerDescriptor& ref = descriptor1;
secondDescriptor descriptor2;
secondDescriptor& refToDescriptor2 = descriptor2;

void readHeaderDescriptor(int fd, headerDescriptor &hd);
//open
//read
//seek
//close

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

        //readHeaderDescriptor(fd, descriptor1);
        readHeaderDescriptor(fd, ref);

    }

    return 0;
}

//C:\Users\2017W\Documents\compSci\Operating-Systems-Term-Project\VDITestFiles\Good\Test-fixed-1k.vdi
void readHeaderDescriptor(int fd, headerDescriptor &hd){
    if (read(fd, &hd, sizeof(hd)) < 0)
        cout << "ERROR"<< endl;
  
    for (int i = 0; i < sizeof(hd.preheader); i++) {
        cout << hd.preheader[i];
    }
    for (int i = 0; i < sizeof(hd.UUID); i++) {
        cout << hd.UUID[i];
    }

}

//https://stackoverflow.com/questions/32717269/how-to-read-an-integer-and-a-char-with-read-function-in-c