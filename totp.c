#include <System/SystemMgr.h>
#include <System/StringMgr.h>
#include <System/Preferences.h>
#include <System/TimeMgr.h>
#include <SysEvtMgr.h>

#include "totp.h"
#include "TOTPAlg.h"

#define TOTP_CODE_LENGTH 6
#define TIMESTAMP_LENGTH 10
#define PALM2UNIX(a)  (a - 2082844800)

static char hmacKey [10];

UInt32 PilotMain (UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
  EventType event;
  long totpCode;
  long timestamp;
  char totpStr[TOTP_CODE_LENGTH];
  char timestampStr[TIMESTAMP_LENGTH];
  long timeZone = PrefGetPreference(prefTimeZone);
  long daylightSavingAdjustment = PrefGetPreference(prefDaylightSavingAdjustment);
  UInt32 utcTime = TimTimeZoneToUTC(TimGetSeconds(), timeZone, daylightSavingAdjustment);

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
    TOTPAlg((char *)hmacKey, 10, 60);


    timestamp = PALM2UNIX(utcTime);
    totpCode = getCodeFromTimestamp(timestamp);
    StrPrintF(totpStr, "%ld", totpCode);
    StrPrintF(timestampStr, "%ld", timestamp);
    WinDrawChars(totpStr, TOTP_CODE_LENGTH, 55, 90);
    WinDrawChars("OTP:", 4, 32, 90);
    WinDrawChars(timestampStr, TIMESTAMP_LENGTH, 55, 74);
    WinDrawChars("NOW:", 4, 32, 74);
    do {
      EvtGetEvent( &event, evtWaitForever);

      SysHandleEvent( &event);

    } while (event.eType != appStopEvent);
  }
  return 0;
}
