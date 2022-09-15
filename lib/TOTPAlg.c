#include "TOTPAlg.h"
#include "sha1.h"

UInt8* _hmacKey;
UInt8 _keyLength;
UInt32 _timeStep;

// Init the library with the private key, its length and the timeStep duration
void TOTPAlg(UInt8* hmacKey, UInt8 keyLength, UInt32 timeStep) {
  _hmacKey = hmacKey;
  _keyLength = keyLength;
  _timeStep = timeStep;
}

// Generate a code, using the timestamp provided
UInt32 getCodeFromTimestamp(UInt32 timeStamp) {
  UInt32 steps = timeStamp / _timeStep;
  return getCodeFromSteps(steps);
}

// Generate a code, using the number of steps provided
UInt32 getCodeFromSteps(UInt32 steps) {
  // STEP 0, map the number of steps in a 8-bytes array (counter value)
  UInt8 _byteArray[8];
  UInt8* _hash;
  UInt32 _truncatedHash;
  UInt8 _offset;
  UInt8 j;

  _byteArray[0] = 0x00;
  _byteArray[1] = 0x00;
  _byteArray[2] = 0x00;
  _byteArray[3] = 0x00;
  _byteArray[4] = (UInt8)((steps >> 24) & 0xFF);
  _byteArray[5] = (UInt8)((steps >> 16) & 0xFF);
  _byteArray[6] = (UInt8)((steps >> 8) & 0XFF);
  _byteArray[7] = (UInt8)((steps & 0XFF));

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
