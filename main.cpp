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

//open Takes file name, returns pointer to second struct
void open(string filename, secondDescriptor &refToDescriptor2);
//read
//seek
//close

int main(int argc, char *argv[])
{
    open(argv[1], refToDescriptor2);

    return 0;
}

//C:\Users\2017W\Documents\compSci\Operating-Systems-Term-Project\VDITestFiles\Good\Test-fixed-1k.vdi

void open(string filename, secondDescriptor &refToDescriptor2) {
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
        if (read(fd, &refToDescriptor2, sizeof(refToDescriptor2)) < 0)
            cout << "ERROR"<< endl;
        refToDescriptor2.hd = refToDescriptor2;
        
        //set cursor to 0
        refToDescriptor2.cursor = 0;

        //return pointer
        //return null pointer if anything went wrong
    }
}

void close(secondDescriptor refToDescriptor2) {
    //could close file too
    delete refToDescriptor2;
}

//https://stackoverflow.com/questions/32717269/how-to-read-an-integer-and-a-char-with-read-function-in-c