#ifndef SHA1_H
#define SHA1_H

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

// SHA-1 words must be exactly 32 bits. The m68k Palm has 32-bit 'long' but
// 16-bit 'int'; hosted 64-bit toolchains have 64-bit 'long'. Pick per target.
#if defined(__m68k__) || defined(__MC68K__)
typedef unsigned long sha_u32;
#else
typedef unsigned int sha_u32;
#endif

void init(void);
void initHmac(const char* key, char keyLength);
char* result(void);
char* resultHmac(void);
void write(char data);
void writeArray(char *data, char size);

#endif
