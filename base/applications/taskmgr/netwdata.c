#include "precomp.h"

CRITICAL_SECTION NetwCriticalDataSection;
DWORD dwTableBufferSize;

PMIB_IFTABLE pIfTable = NULL;


BOOL LoadInterfaces(void);

// PUBLIC

BOOL NetwDataInitialize(void)
{
    DWORD Status;

    //InitializeCriticalSection(&NetwCriticalDataSection);

    if (pIfTable != NULL)
        return FALSE;
    
    pIfTable = HeapAlloc(GetProcessHeap(), 0, sizeof(MIB_IFTABLE));
    if (pIfTable == NULL)
        return FALSE;
    
    dwTableBufferSize = sizeof(MIB_IFTABLE);

    if (GetIfTable(pIfTable, &dwTableBufferSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
    {
        HeapFree(GetProcessHeap(), 0, pIfTable);
        pIfTable = (PMIB_IFTABLE) HeapAlloc(GetProcessHeap(), 0, dwTableBufferSize);
        if (pIfTable == NULL)
            return FALSE;
    }

    Status = GetIfTable(pIfTable, &dwTableBufferSize, FALSE);
    if (Status != NO_ERROR)
        return FALSE;

    return LoadInterfaces();
}

void NetwDataUninitialize(void)
{
    if (pIfTable != NULL)
        HeapFree(GetProcessHeap(), 0, pIfTable);
    
    if (pNetData != NULL)
        HeapFree(GetProcessHeap(), 0, pNetData);
    
    //DeleteCriticalSection(&NetwCriticalDataSection);
}

BOOL LoadInterfaces(void)
{
    UINT16 dwNumInterfaces;
    PNETW_INTERFACE pInterface = HeapAlloc(GetProcessHeap(), 0, sizeof(NETW_INTERFACE));

    if (pNetData != NULL)
        HeapFree(GetProcessHeap(), 0, pNetData);

    pNetData = HeapAlloc(GetProcessHeap(), 0, sizeof(NETW_DATA));
    if (pNetData == NULL)
        return FALSE;

    dwNumInterfaces = (UINT16) pIfTable->dwNumEntries;
    pNetData->nInterfaces = dwNumInterfaces;
    pNetData->stInterfaces = HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sizeof(NETW_INTERFACE) * dwNumInterfaces);

    if (pNetData->stInterfaces == NULL)
    {
        // todo: handle error
        return FALSE;
    }

    for (int i = 0; i < dwNumInterfaces; i++)
    {
        //memset(pInterface, 0, sizeof(NETW_INTERFACE));
        pInterface->szName = L"Sus? Amogus?!!";

        pNetData->stInterfaces[i] = *pInterface;
    }

    return TRUE;
}

// PRIVATE