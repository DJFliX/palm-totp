#include <string.h>
#include "sha1.h"

#define SHA1_K0 0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c

typedef struct {
  sha_u32 h[5];
  unsigned char block[BLOCK_LENGTH];
  sha_u32 byteCount;
  unsigned char blockLen;
} Sha1Ctx;

static Sha1Ctx ctx;
static unsigned char keyBuffer[BLOCK_LENGTH];
static unsigned char innerHash[HASH_LENGTH];
static char digest[HASH_LENGTH];

static sha_u32 rol32(sha_u32 number, unsigned char bits) {
  return (number << bits) | (number >> (32 - bits));
}

static void hashBlock(void) {
  sha_u32 w[16];
  sha_u32 a, b, c, d, e, f, k, t;
  unsigned char i;

  // The 512-bit message block is split into sixteen 32-bit words in
  // big-endian order, independent of the host byte order.
  for (i = 0; i < 16; i++) {
    w[i] = ((sha_u32)ctx.block[i * 4] << 24)
         | ((sha_u32)ctx.block[i * 4 + 1] << 16)
         | ((sha_u32)ctx.block[i * 4 + 2] << 8)
         | (sha_u32)ctx.block[i * 4 + 3];
  }

  a = ctx.h[0];
  b = ctx.h[1];
  c = ctx.h[2];
  d = ctx.h[3];
  e = ctx.h[4];

  for (i = 0; i < 80; i++) {
    if (i >= 16) {
      t = w[(i + 13) & 15] ^ w[(i + 8) & 15] ^ w[(i + 2) & 15] ^ w[i & 15];
      w[i & 15] = rol32(t, 1);
    }
    if (i < 20) {
      f = (b & c) | ((~b) & d);
      k = SHA1_K0;
    } else if (i < 40) {
      f = b ^ c ^ d;
      k = SHA1_K20;
    } else if (i < 60) {
      f = (b & c) | (b & d) | (c & d);
      k = SHA1_K40;
    } else {
      f = b ^ c ^ d;
      k = SHA1_K60;
    }
    t = rol32(a, 5) + f + e + k + w[i & 15];
    e = d;
    d = c;
    c = rol32(b, 30);
    b = a;
    a = t;
  }

  ctx.h[0] += a;
  ctx.h[1] += b;
  ctx.h[2] += c;
  ctx.h[3] += d;
  ctx.h[4] += e;
}

void init(void) {
  ctx.h[0] = 0x67452301;
  ctx.h[1] = 0xefcdab89;
  ctx.h[2] = 0x98badcfe;
  ctx.h[3] = 0x10325476;
  ctx.h[4] = 0xc3d2e1f0;
  ctx.byteCount = 0;
  ctx.blockLen = 0;
}

void write(char data) {
  ctx.block[ctx.blockLen] = (unsigned char)data;
  ctx.blockLen++;
  ctx.byteCount++;
  if (ctx.blockLen == BLOCK_LENGTH) {
    hashBlock();
    ctx.blockLen = 0;
  }
}

void writeArray(char *data, char size) {
  while (size--) {
    write(*data++);
  }
}

static void pad(void) {
  sha_u32 bitLen;

  // Capture the message length in bits BEFORE the padding bytes are written,
  // since write() counts every byte fed into the block buffer.
  bitLen = ctx.byteCount << 3;

  // Padding as per FIPS 180-2 section 5.1.1: 0x80, zeros up to byte 55,
  // then the 64-bit message length in bits (big-endian).
  write(0x80);
  while (ctx.blockLen != 56) {
    write(0x00);
  }
  // The upper 32 bits of the length are zero for any message we process.
  write(0);
  write(0);
  write(0);
  write(0);
  write((char)(bitLen >> 24));
  write((char)(bitLen >> 16));
  write((char)(bitLen >> 8));
  write((char)bitLen);
}

char* result(void) {
  unsigned char i;

  pad();

  // Emit the digest as 20 bytes, each state word big-endian.
  for (i = 0; i < HASH_LENGTH; i++) {
    digest[i] = (char)(ctx.h[i >> 2] >> (24 - ((i & 3) << 3)));
  }
  return digest;
}

void initHmac(const char* key, char keyLength) {
  unsigned char i;

  memset(keyBuffer, 0, BLOCK_LENGTH);
  if ((unsigned char)keyLength > BLOCK_LENGTH) {
    // Keys longer than a block are hashed down to 20 bytes first.
    init();
    for (i = 0; i < keyLength; i++) {
      write(key[i]);
    }
    memcpy(keyBuffer, result(), HASH_LENGTH);
  } else {
    for (i = 0; i < keyLength; i++) {
      keyBuffer[i] = (unsigned char)key[i];
    }
  }

  // Start the inner hash with (key XOR ipad).
  init();
  for (i = 0; i < BLOCK_LENGTH; i++) {
    write((char)(keyBuffer[i] ^ HMAC_IPAD));
  }
}

char* resultHmac(void) {
  unsigned char i;

  // Complete the inner hash, then run the outer hash over (key XOR opad)
  // followed by the inner digest.
  memcpy(innerHash, result(), HASH_LENGTH);
  init();
  for (i = 0; i < BLOCK_LENGTH; i++) {
    write((char)(keyBuffer[i] ^ HMAC_OPAD));
  }
  for (i = 0; i < HASH_LENGTH; i++) {
    write((char)innerHash[i]);
  }
  return result();
}
