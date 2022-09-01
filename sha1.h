#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

union _buffer {
  char b[BLOCK_LENGTH];
  long w[BLOCK_LENGTH/4];
} buffer;
union _state {
  char b[HASH_LENGTH];
  long w[HASH_LENGTH/4];
} state;

char bufferOffset;
long byteCount;
char keyBuffer[BLOCK_LENGTH];
char innerHash[HASH_LENGTH];

void init(void);
void initHmac(const char* secret, char secretLength);
char* result(void);
char* resultHmac(void);
void write(char);
void writeArray(char *buffer, char size);