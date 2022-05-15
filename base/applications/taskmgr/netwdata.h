#pragma once

typedef struct _NETW_INTERFACE
{
    PWCHAR szName;
} NETW_INTERFACE, *PNETW_INTERFACE;

typedef struct _NETW_DATA
{
    UINT16 nInterfaces;
    PNETW_INTERFACE stInterfaces;
} NETW_DATA, *PNETW_DATA;
PNETW_DATA pNetData;

BOOL NetwDataInitialize(void);
void NetwDataUninitialize(void);