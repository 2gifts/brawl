#pragma once

// The debug reports of the network code (their printing is compiled out of the game). They are apart from
// mu_wifi_interface.h because some menu files have a file-static netReport of their own (muWifiCnctWndTask's).
// netReport is defined in mu_wifi_interface_task.cpp, netReport0 in nt_match.cpp.
void netReport(const char* fmt, ...);
void netReport0(const char* fmt, ...);
