# Header Descriptor Layout 
## In Little Endian

struct headerDescriptor {
	byte preheader[72];
	byte name[48];
	float versionNum;
	unsigned int sizeOfHeader;
	int imageType;
	int imageFlags;
	byte imageDescription[32];
	int offsetBlocks;
	int offsetData;
	unsigned int numOfCylinders;
	unsigned int numOfHeads;
	unsigned int numOfSectors;
	unsigned int sectorSize;
	//4 byte unused
	long long int diskSize;
	unsigned int blockSize;
	unsigned int numOfBlocksInHDD;
	unsigned int numOfBlocksAllocated;
	byte UUID[16];
	byte UUIDLastSnap[16];
	byte UUIDLink[16];
	byte Parent[16];
	//read garbage until next MB
};
- 72 Byte pre header
- 48 Bytes Sun xVM VirtualBox Disk Image
- 4 Version #
- 4 Size of Header
- 4 Image Type (Dynamic VDI)
- 4 Image Flags
- 32 Image Descritption
- 4 Offset Blocks
- 4 Offset Data
- 4 #Cylinders
- 4 #Heads
- 4 #Sectors
- 4 SectorSize
- 4 UNUSED
- 8 DiskSize (in Bytes)
- 4 BlockSize
- 4 Block Extra Data
- 4 BlocksInHDD
- 4 #OfBlocksAllocated
- 16 UUID of this VDI
- 16 UUID of last SNAP
- 16 UUID link
- 16 UUID parent
- 56 UNUSED (To fill 1MB)