#include <System/SystemMgr.h>
#include <System/StringMgr.h>
#include <System/Preferences.h>
#include <System/TimeMgr.h>
#include <SysEvtMgr.h>

#include "totp.h"
#include "lib/TOTPAlg.h"

#define TOTP_CODE_LENGTH 6
#define TIMESTAMP_LENGTH 10
#define PALM2UNIX(a)  (a - 2082844800)

static char hmacKey [10] = { 0x4d, 0x79, 0x4c, 0x65, 0x67, 0x6f, 0x44, 0x6f, 0x6f, 0x72 };

long timeZone;
long daylightSavingAdjustment;
UInt32 utcTime;
static UInt16 SHALibRefNum;

UInt32 PilotMain (UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
  EventType event;
  Err err;
  long totpCode;
  long timestamp;
  char totpStr[TOTP_CODE_LENGTH];
  char timestampStr[TIMESTAMP_LENGTH];

  timeZone = PrefGetPreference(prefTimeZone);
  daylightSavingAdjustment = PrefGetPreference(prefDaylightSavingAdjustment);
  utcTime = TimTimeZoneToUTC(TimGetSeconds(), timeZone, daylightSavingAdjustment);

  if (cmd == sysAppLaunchCmdNormalLaunch)
  {
    TOTPAlg((char *)hmacKey, 10, 60);

    timestamp = PALM2UNIX(utcTime);
    totpCode = getCodeFromTimestamp(1662220000);
    StrIToA(totpStr, totpCode);
    StrIToA(timestampStr, timestamp);
    WinDrawChars(totpStr, TOTP_CODE_LENGTH, 55, 90);
    WinDrawChars(totpStr, 1, 0, 20);
    WinDrawChars(totpStr + 1, 1, 6, 20);
    WinDrawChars(totpStr + 2, 1, 12, 20);
    WinDrawChars(totpStr + 3, 1, 18, 20);
    WinDrawChars(totpStr + 4, 1, 24, 20);
    WinDrawChars(totpStr + 5, 1, 30, 20);
    WinDrawChars("OTP:", 4, 32, 90); // 1662220000 -> 474791
    WinDrawChars(timestampStr, TIMESTAMP_LENGTH, 55, 74);
    WinDrawChars("NOW:", 4, 32, 74);
    do {
      EvtGetEvent( &event, evtWaitForever);
      SysHandleEvent( &event);
    } while (event.eType != appStopEvent);
  }
  return 0;
}
