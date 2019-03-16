//here is wehre I got this from:
//https://www.ibm.com/developerworks/community/blogs/58e72888-6340-46ac-b488-d31aa4058e9c/entry/understanding_linux_open_system_call?lang=en

#include <iostream>
#include <fstream>
using namespace std;

//details on pack code here https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html
#pragma pack(push, 1)
struct headerDescriptor {
    char preheader[72];
    char name[48];
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
    fstream file;
    int cursor;
};
#pragma pack(pop)

headerDescriptor descriptor1;

void readHeaderDescriptor(fstream fd, headerDescriptor hd);

int main(int argc, char *argv[])
{
    //if no no arguments, then print something
    if(2 != argc)
    {
        printf("\n Usage :  \n");
        return 1;
    }
    headerDescriptor hd;

    fstream file;
    file.open(argv[1]);

    if (file.fail())
    {
        cout << "ERROR: Cannot open the file..." << endl;
        exit(0);
    }

    file.read(hd, sizeof(hd));

}

void readHeaderDescriptor(fstream fd, headerDescriptor hd) {
}