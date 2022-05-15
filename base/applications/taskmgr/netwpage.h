#pragma once

extern HWND hNetworkPage;

INT_PTR CALLBACK    NetworkPageWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void                RefreshNetworkPage(void);

