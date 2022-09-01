Sauce for TOTPAlg.c, TOTPAlg.h, sha1.c and sha1.h:
https://github.com/Netthaw/TOTP-MCU


ToDo for first "releasable" version:
- [x] Find a TOTP library that will compile on Palm OS
- [x] Find out how to "get time" in Palm OS
- [x] Write/fix helper function to display totp code correctly
- [ ] Change hardcoded hmac key to a input field in the main form
- [ ] Add a refresh button
- [ ] Clean up the makefile that I copy pasted from Dopewars (and contains hardcoded paths for my system)

Features for after that:
- [ ] Implement Menu + settings (to set a single hmac key)
- [ ] Implement support for multiple TOTP entries
- [ ] Implement auto-refresh every x seconds
- [ ] Implement indicator for remaining time till next refresh

Features that are considered... some day:
- [ ] QR code to "export" TOTP key to a phone or other camera-equipped device

Features that will probably never come:
- [ ] NTP sync
- [ ] Encryption of hmac secrets
- [ ] QR code import (because Palm OS cameras in general suck)