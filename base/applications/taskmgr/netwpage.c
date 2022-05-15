#include "precomp.h"

HWND hNetworkPage;
static int      nNetworkPageWidth;
static int      nNetworkPageHeight;
//static BOOL     bSortAscending = TRUE;

HWND hSendText;
HWND hReceiveText;
HWND hSendValue;
HWND hReceiveValue;
HWND hNetListCtrl;
DWORD      dwBufferSize;


#ifdef RUN_NETW_PAGE
static HANDLE   hNetworkThread = NULL;
static DWORD    dwNetworkThread;
#endif

DWORD WINAPI   NetworkPageRefreshThread(void *lpParameter);

DWORD Status;

INT_PTR CALLBACK
NetworkPageWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT       rc;
    LV_COLUMN  column;
    WCHAR      szText[256];

    switch (message)
    {
    case WM_DESTROY:
#ifdef RUN_NETW_PAGE
        EndLocalThread(&hNetworkThread, dwNetworkThread);
#endif
        break;
    case WM_INITDIALOG:
        /* Save the width and height */
        GetClientRect(hDlg, &rc);
        nNetworkPageWidth = rc.right;
        nNetworkPageHeight = rc.bottom;

        /* Update window position */
        SetWindowPos(hDlg, NULL, 15, 30, 0, 0, SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOZORDER);

        hSendText = GetDlgItem(hDlg, IDS_NET_SEND);
        hReceiveText = GetDlgItem(hDlg, IDS_NET_RECEIVE);
        hSendValue = GetDlgItem(hDlg, IDC_NET_SEND);
        hReceiveValue = GetDlgItem(hDlg, IDC_NET_RECEIVE);
        hNetListCtrl = GetDlgItem(hDlg, IDC_NET_LIST);

        SetWindowTextW(hNetListCtrl, L"Network");

        column.mask = LVCF_TEXT|LVCF_WIDTH;
        LoadStringW(hInst, IDS_TAB_INTERFC_NAME, szText, 256);
        column.pszText = szText;
        column.cx = 250;
        (void)ListView_InsertColumn(hNetListCtrl, 0, &column);    /* Add the "Interface" column */

#ifdef RUN_NETW_PAGE
        hNetworkThread = CreateThread(NULL, 0, NetworkPageRefreshThread, NULL, 0, &dwNetworkThread);
#endif

        return TRUE;
    }

    return 0;
}

void RefreshNetworkPage(void)
{
#ifdef RUN_NETW_PAGE
    /* Signal the event so that our refresh thread */
    /* will wake up and refresh the network page */
    PostThreadMessage(dwNetworkThread, WM_TIMER, 0, 0);
#endif
}

DWORD WINAPI NetworkPageRefreshThread(void *lpParameter)
{
    MSG           msg;
    WCHAR         szText[256], szNetSend[256], szNetReceive[256];
    PWCHAR        szInterfaceName;
    UINT16        dwNumEntries;
    LV_ITEM       item;
    //MIB_IFROW     row;
    //int           nEth = 0, nOther = 0, nTokenRing = 0, nPeerToPeer = 0, nLoopBack = 0, nAtm = 0;

    LoadStringW(hInst, IDS_NET_SEND, szNetSend, 256);
    LoadStringW(hInst, IDS_NET_RECEIVE, szNetReceive, 256);

    /* If we couldn't create the event then exit the thread */
    while (1)
    {
        /*  Wait for an the event or application close */
        if (GetMessage(&msg, NULL, 0, 0) <= 0)
            goto cleanup;

        if (msg.message == WM_TIMER)
        {
            
            if (IsWindowVisible(hNetworkPage))
            {
                SendMessage(hNetListCtrl, WM_SETREDRAW, FALSE, 0);
                //InvalidateRect(hNetListCtrl, NULL, FALSE);
            }

            dwNumEntries = pNetData->nInterfaces;
            swprintf(szText, L"%u", dwNumEntries);
            SetWindowTextW(hSendValue, szText);

            //(void)ListView_DeleteAllItems(hNetListCtrl);

            for (UINT16 i = 0; i < dwNumEntries; i++)
            {
                szInterfaceName = pNetData->stInterfaces[i].szName;

                memset(&item, 0, sizeof(LV_ITEM));
                item.mask = LVIF_TEXT | LVIF_STATE;
                item.pszText = szInterfaceName;
                item.iItem = i;
                item.state = 0;
                item.stateMask = 0;

                (void)ListView_InsertItem(hNetListCtrl, &item);
            }

            (void)ListView_RedrawItems(hNetListCtrl, 0, ListView_GetItemCount(hNetListCtrl));

/*

            for (int i = 0; i < dwNumEntries; i++)
            {
                row = IfTable->table[i];

                _itow(i, szText, 10);
                SetWindowTextW(hReceiveValue, szText);

                switch (row.dwType)
                {
                    case MIB_IF_TYPE_OTHER:
                        wsprintfW(szText, L"other %d", nOther++);
                        break;
                    case MIB_IF_TYPE_ETHERNET:
                        wsprintfW(szText, L"ethernet %d", nEth++);
                        break;
                    case MIB_IF_TYPE_TOKENRING:
                        wsprintfW(szText, L"tokenring %d", nTokenRing++);
                        break;
                    case MIB_IF_TYPE_PPP:
                        wsprintf(szText, L"PPP %d", nPeerToPeer++);
                        break;
                    case MIB_IF_TYPE_LOOPBACK:
                        wsprintf(szText, L"loopback %d", nLoopBack++);
                        break;
                    case MIB_IF_TYPE_SLIP:
                        wsprintf(szText, L"ATM %d", nAtm++);
                        break;
                    default:
                        continue;
                }

                memset(&item, 0, sizeof(LV_ITEM));
                item.mask = LVIF_TEXT | LVIF_STATE;
                item.pszText = szText;
                item.iItem = i;
                item.state = 0;
                item.stateMask = 0;

                (void)ListView_InsertItem(hNetListCtrl, &item);
            }
            */
        }
    }

cleanup:
    //HeapFree(GetProcessHeap(), 0, IfTable);

    return 0;
}