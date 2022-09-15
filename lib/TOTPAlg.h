#include <PalmOS.h>

void TOTPAlg(UInt8 *hmacKey, UInt8 keyLength, UInt32 timeStep);
UInt32 getCodeFromTimestamp(UInt32 timeStamp);
UInt32 getCodeFromSteps(UInt32 steps);
