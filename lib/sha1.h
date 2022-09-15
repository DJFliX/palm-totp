#include <PalmOS.h>

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

union _buffer {
  UInt8 b[BLOCK_LENGTH];
  UInt32 w[BLOCK_LENGTH/4];
} buffer;
union _state {
  UInt8 b[HASH_LENGTH];
  UInt32 w[HASH_LENGTH/4];
} state;

UInt8 bufferOffset;
UInt32 byteCount;
UInt8 keyBuffer[BLOCK_LENGTH];
UInt8 innerHash[HASH_LENGTH];

void init(void);
void initHmac(const UInt8* secret, UInt8 secretLength);
UInt8* result(void);
UInt8* resultHmac(void);
void write(UInt8);
void writeArray(UInt8 *buffer, UInt8 size);
