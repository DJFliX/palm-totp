#include "TOTPAlg.h"
#include "sha1.h"

char* _hmacKey;
char _keyLength;
long _timeStep;

// Init the library with the private key, its length and the timeStep duration
void TOTPAlg(char* hmacKey, char keyLength, long timeStep) {
    _hmacKey = hmacKey;
    _keyLength = keyLength;
    _timeStep = timeStep;
}

// Generate a code, using the timestamp provided
long getCodeFromTimestamp(long timeStamp) {
    long steps = timeStamp / _timeStep;
    return getCodeFromSteps(steps);
}

// Generate a code, using the number of steps provided
long getCodeFromSteps(long steps) {
    // STEP 0, map the number of steps in a 8-bytes array (counter value)
    char _byteArray[8];
    char* _hash;
    long _truncatedHash;
    char _offset;
    char j;

    _byteArray[0] = 0x00;
    _byteArray[1] = 0x00;
    _byteArray[2] = 0x00;
    _byteArray[3] = 0x00;
    _byteArray[4] = (char)((steps >> 24) & 0xFF);
    _byteArray[5] = (char)((steps >> 16) & 0xFF);
    _byteArray[6] = (char)((steps >> 8) & 0XFF);
    _byteArray[7] = (char)((steps & 0XFF));

    // STEP 1, get the HMAC-SHA1 hash from counter and key
    initHmac(_hmacKey, _keyLength);
    writeArray(_byteArray, 8);
    _hash = resultHmac();

    // STEP 2, apply dynamic truncation to obtain a 4-bytes string
    _truncatedHash = 0;
    _offset = _hash[20 - 1] & 0xF;

    for (j = 0; j < 4; ++j) {
        _truncatedHash <<= 8;
        _truncatedHash  |= _hash[_offset + j];
    }

    // STEP 3, compute the OTP value
    _truncatedHash &= 0x7FFFFFFF;    //Disabled
    _truncatedHash %= 1000000;

    return _truncatedHash;
}