#include "oc_getppid.hpp"

#ifdef Q_OS_WIN

#include    <windows.h>
#include    <tlhelp32.h>
#include    <stdio.h>

qint32 oc_getppid()
{
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe32;
    DWORD ppid = 0, pid = GetCurrentProcessId();

    hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if( hSnapshot == INVALID_HANDLE_VALUE ) return static_cast<quint32>(ppid);

    ZeroMemory( &pe32, sizeof( pe32 ) );
    pe32.dwSize = sizeof( pe32 );
    if( !Process32First( hSnapshot, &pe32 ) ) return static_cast<quint32>(ppid);

    do{
        if( pe32.th32ProcessID == pid ){
            ppid = pe32.th32ParentProcessID;
            break;
        }
    }while( Process32Next( hSnapshot, &pe32 ) );


    if( hSnapshot != INVALID_HANDLE_VALUE ) CloseHandle( hSnapshot );

    return static_cast<quint32>(ppid);
}

#else

#include <unistd.h>


qint32 oc_getppid(){
    return getppid();
}

#endif
