#include <System/SystemMgr.h>
#include <System/StringMgr.h>
#include <System/Preferences.h>
#include <System/TimeMgr.h>
#include <SysEvtMgr.h>
#include <unix_stdarg.h>

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

static Boolean appHandleEvent(EventPtr event);

void showMessage(char *format, ...)
{
  va_list args;
  char msg[99];

  va_start(args, format);
  StrVPrintF(msg, format, args);
  va_end(args);

  WinDrawChars(msg, StrLen(msg), 0, 80);
}

static FieldPtr SetFieldTextFromHandle(UInt8 fieldID, MemHandle txtH)
{
  MemHandle oldTxtH;
  FormPtr frm = FrmGetActiveForm();
  FieldPtr fldP;

  // get the field and the field's current text handle.
  fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fieldID));
  ErrNonFatalDisplayIf(!fldP, "missing field");
  oldTxtH = FldGetTextHandle(fldP);

  // set the field's text to the new text.
  FldSetTextHandle(fldP, txtH);
  FldDrawField(fldP);

  // free the handle AFTER we call FldSetTextHandle().
  if (oldTxtH)
    MemHandleFree(oldTxtH);

  return fldP;
}

// https://lspo.feri.um.si/teachware/internal/programming/palmos_programming/ch05.htm#P1127_56204
static FieldPtr SetFieldTextFromStr(UInt8 fieldID, char *strP)
{
  MemHandle txtH;

  // get some space in which to stash the string.
  txtH = MemHandleNew(StrLen(strP) + 1);
  if (!txtH)
    return NULL;

  // copy the string to the locked handle.
  StrCopy(MemHandleLock(txtH), strP);

  // unlock the string handle.
  MemHandleUnlock(txtH);

  // set the field to the handle
  return SetFieldTextFromHandle(fieldID, txtH);
}

static void CreateAccountFlow()
{
  FormPtr previousForm = FrmGetActiveForm();
  FormPtr form;
  UInt8 hitButton;
  FieldPtr descriptionField;
  FieldPtr totpSecretField;

  form = FrmInitForm(NewAccountForm);
  FrmSetActiveForm(form);
  FrmDrawForm(form);

  descriptionField = FrmGetObjectPtr(form, NewAccountNameField);
  totpSecretField = FrmGetObjectPtr(form, NewAccountSecretField);
  SetFieldTextFromStr(NewAccountNameField, "Test account");
  SetFieldTextFromStr(NewAccountSecretField, "JBSWY3DPEHPK3PXP");
  FrmSetFocus(form, FrmGetObjectIndex(form, NewAccountSecretField));
  FldSetSelection(totpSecretField, 0, FldGetTextLength(totpSecretField));

  hitButton = FrmDoDialog(form);

  if (hitButton == NewAccountOkButton)
  {
    if (previousForm)
      FrmSetActiveForm(previousForm);
    FrmDeleteForm(form);
  }
}

Boolean appHandleEvent(EventPtr event)
{
  Boolean handled;
  FormPtr form;

  handled = true;

  if (frmLoadEvent == event->eType)
  {
    form = FrmInitForm(event->data.frmLoad.formID);
    FrmSetActiveForm(form);
    FrmDrawForm(form);
  }
  else if (menuEvent == event->eType)
  {
    switch (event->data.menu.itemID)
    {
    case MainMenuAccountNew:
      FrmGotoForm(NewAccountForm);
      break;
    default:
      FrmAlert(FeatureNotImplemented);
      break;
    }
  }
  else
  {
    handled = false;
  }

  return handled;
}

UInt32 PilotMain (UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
  EventType event;
  Err err;
  FormPtr form;
  long totpCode;
  long timestamp;
  char totpStr[TOTP_CODE_LENGTH];
  char timestampStr[TIMESTAMP_LENGTH];

  timeZone = PrefGetPreference(prefTimeZone);
  daylightSavingAdjustment = PrefGetPreference(prefDaylightSavingAdjustment);
  utcTime = TimTimeZoneToUTC(TimGetSeconds(), timeZone, daylightSavingAdjustment);

  if (cmd == sysAppLaunchCmdNormalLaunch)
  {

    // TOTPAlg((char *)hmacKey, 10, 60);

    // timestamp = PALM2UNIX(utcTime);
    // totpCode = getCodeFromTimestamp(1662220000);
    // StrIToA(totpStr, totpCode);
    // StrIToA(timestampStr, timestamp);
    // WinDrawChars(totpStr, TOTP_CODE_LENGTH, 55, 90);
    // WinDrawChars(totpStr, 1, 0, 20);
    // WinDrawChars(totpStr + 1, 1, 6, 20);
    // WinDrawChars(totpStr + 2, 1, 12, 20);
    // WinDrawChars(totpStr + 3, 1, 18, 20);
    // WinDrawChars(totpStr + 4, 1, 24, 20);
    // WinDrawChars(totpStr + 5, 1, 30, 20);
    // WinDrawChars("OTP:", 4, 32, 90); // 1662220000 -> 474791
    // WinDrawChars(timestampStr, TIMESTAMP_LENGTH, 55, 74);
    // WinDrawChars("NOW:", 4, 32, 74);

    FrmGotoForm(MainForm);
    // showMessage("open the menu");
    do
    {
      EvtGetEvent(&event, evtWaitForever);

      if (!SysHandleEvent(&event))
        if (!MenuHandleEvent(0, &event, &err))
          appHandleEvent(&event);
    } while (event.eType != appStopEvent);

    FrmCloseAllForms();
  }
  return 0;
}
