# Header Descriptor Layout 
## In Little Endian

- 48 Bytes Sun xVM VirtualBox Disk Image
- 4 Version #
- 4 Header
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