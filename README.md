# Files System

The job is to build a file system to use i2c EEPROM memory with an AVR core. The file system behaved as follows:
- A 2-byte header indicating the number of the first free data block. Then there are 12 entries of
files (inodos). Each file entry consists of fields (Status - Indicates whether the entry is free or busy, Name
- 8 letters for file name, size - how many bytes the file occupies, direct block (32 data bytes), indirect block (is
the address of a block whose contents are addresses to data blocks).

- A block pointers of these the data blocks. Pointer blocks are 2 Bytes each and were initially organized to create a list.
chained. A block stores in its 2 Bytes the address the next free block. There is a relationship between each block of
pointer and the data block. Pointer block 0 is related to data block 0.
