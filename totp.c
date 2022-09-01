#include <Common.h>
#include <System/SysAll.h>
#include <SysEvtMgr.h>
#include <UI/UIAll.h>

#include "totp.h"
#include "TOTPAlg.h"

#define TOTP_CODE_LENGTH 6

static int hmacKey [10];

DWord PilotMain (Word cmd, Ptr cmdPBP, Word launchFlags)
{
  EventType event;
  long totpCode;
  char totpStr[TOTP_CODE_LENGTH];

  if (cmd == sysAppLaunchCmdNormalLaunch)
  {
    hmacKey[0] = 0x4d;
    hmacKey[1] = 0x79;
    hmacKey[2] = 0x4c;
    hmacKey[3] = 0x65;
    hmacKey[4] = 0x67;
    hmacKey[5] = 0x6f;
    hmacKey[6] = 0x44;
    hmacKey[7] = 0x6f;
    hmacKey[8] = 0x6f;
    hmacKey[9] = 0x72;
    TOTPAlg((int *)hmacKey, 10, 7200); 
    totpCode = getCodeFromTimestamp(1557414000);
    StrPrintF(totpStr, "%ld", totpCode);
    WinDrawChars(totpStr, TOTP_CODE_LENGTH, 55, 74 );
    do {
      EvtGetEvent( &event, evtWaitForever);

      SysHandleEvent( &event);

    } while (event.eType != appStopEvent);
  }
  return 0;
}
