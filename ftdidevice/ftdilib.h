#ifndef FTDI_LIB_INC
#define FTDI_LIB_INC

#include <qlibrary.h>
#include <qdebug.h>

//#include <windows.h>
#include "ftd2xx.h"


#if defined(Q_OS_WIN)
#elif defined (Q_OS_UNIX) || defined (Q_OS_LINUX)
#include "WinTypes.h"
#endif

typedef   Qt::HANDLE FTDI_HANDLE_TYPE;
#define   FTDI_ZERO_HANDLE FTDI_HANDLE_TYPE(Q_NULLPTR)

#ifdef Q_OS_LINUX
//#include "ftdiconst.h"
//#include <libftdi1/ftdi.h>
#endif
#include <qscopedpointer.h>


class QFtdiLibrary:protected QLibrary
{
private:
    explicit QFtdiLibrary():QLibrary(Q_NULLPTR){}

static QScopedPointer<QFtdiLibrary> ftdi_lib;
static QString error_string;

#ifndef FTDI_IMPLICIT_LINK

typedef FT_STATUS (WINAPI * FP_FT_Open)(int deviceNumber,FT_HANDLE *pHandle);
typedef FT_STATUS (WINAPI * FP_FT_OpenEx)(PVOID pArg1,DWORD Flags,FT_HANDLE *pHandle);
typedef FT_STATUS (WINAPI * FP_FT_ListDevices)(PVOID pArg1,PVOID pArg2,DWORD Flags);
typedef FT_STATUS (WINAPI * FP_FT_Close)(FT_HANDLE ftHandle);
typedef FT_STATUS (WINAPI * FP_FT_Read)(FT_HANDLE ftHandle,LPVOID lpBuffer,DWORD dwBytesToRead,LPDWORD lpBytesReturned);
typedef FT_STATUS (WINAPI * FP_FT_Write)(FT_HANDLE ftHandle,LPVOID lpBuffer,DWORD dwBytesToWrite,LPDWORD lpBytesWritten);
typedef FT_STATUS (WINAPI * FP_FT_IoCtl)(FT_HANDLE ftHandle,DWORD dwIoControlCode,LPVOID lpInBuf,DWORD nInBufSize,LPVOID lpOutBuf,DWORD nOutBufSize,LPDWORD lpBytesReturned,LPOVERLAPPED lpOverlapped);
typedef FT_STATUS (WINAPI * FP_FT_SetBaudRate)(FT_HANDLE ftHandle,ULONG BaudRate);
typedef FT_STATUS (WINAPI * FP_FT_SetDivisor)(FT_HANDLE ftHandle,USHORT Divisor);
typedef FT_STATUS (WINAPI * FP_FT_SetDataCharacteristics)(FT_HANDLE ftHandle,UCHAR WordLength,UCHAR StopBits,UCHAR Parity);
typedef FT_STATUS (WINAPI * FP_FT_SetFlowControl)(FT_HANDLE ftHandle,USHORT FlowControl,UCHAR XonChar,UCHAR XoffChar);
typedef FT_STATUS (WINAPI * FP_FT_ResetDevice)(FT_HANDLE ftHandle);
typedef FT_STATUS (WINAPI * FP_FT_SetDtr)(FT_HANDLE ftHandle);
typedef FT_STATUS (WINAPI * FP_FT_ClrDtr)(FT_HANDLE ftHandle);
typedef FT_STATUS (WINAPI * FP_FT_SetRts)(FT_HANDLE ftHandle);
typedef FT_STATUS (WINAPI * FP_FT_ClrRts)(FT_HANDLE ftHandle);
typedef FT_STATUS (WINAPI * FP_FT_GetModemStatus)(FT_HANDLE ftHandle,ULONG *pModemStatus);
typedef FT_STATUS (WINAPI * FP_FT_SetChars)(FT_HANDLE ftHandle,UCHAR EventChar,UCHAR EventCharEnabled,UCHAR ErrorChar,UCHAR ErrorCharEnabled);
typedef FT_STATUS (WINAPI * FP_FT_Purge)(FT_HANDLE ftHandle,ULONG Mask);
typedef FT_STATUS (WINAPI * FP_FT_SetTimeouts)(FT_HANDLE ftHandle,ULONG ReadTimeout,ULONG WriteTimeout);
typedef FT_STATUS (WINAPI * FP_FT_GetQueueStatus)(FT_HANDLE ftHandle,DWORD *dwRxBytes);
typedef FT_STATUS (WINAPI * FP_FT_SetEventNotification)(FT_HANDLE ftHandle,DWORD Mask,PVOID Param);
typedef FT_STATUS (WINAPI * FP_FT_GetStatus)(FT_HANDLE ftHandle,DWORD *dwRxBytes,DWORD *dwTxBytes,DWORD *dwEventDWord);

typedef FT_STATUS (WINAPI * FP_FT_CreateDeviceInfoList)(LPDWORD lpdwNumDevs );
typedef FT_STATUS (WINAPI * FP_FT_GetDeviceInfoList   )(FT_DEVICE_LIST_INFO_NODE *pDest,LPDWORD lpdwNumDevs );
typedef FT_STATUS (WINAPI * FP_FT_GetDeviceInfoDetail )( DWORD dwIndex,LPDWORD lpdwFlags,LPDWORD lpdwType, LPDWORD lpdwID,LPDWORD lpdwLocId, LPVOID lpSerialNumber, LPVOID lpDescription,FT_HANDLE *pftHandle );


typedef FT_STATUS (WINAPI * FP_FT_GetDriverVersion    )(FT_HANDLE ftHandle,LPDWORD lpdwVersion  );
typedef FT_STATUS (WINAPI * FP_FT_GetLibraryVersion   )(LPDWORD lpdwVersion);
typedef FT_STATUS (WINAPI * FP_FT_Rescan              )(  void  );
typedef FT_STATUS (WINAPI * FP_FT_Reload              )( WORD wVid, WORD wPid  );


static  FP_FT_Open                   pFT_Open;
static  FP_FT_OpenEx                 pFT_OpenEx;
static  FP_FT_ListDevices            pFT_ListDevices;
static  FP_FT_Close                  pFT_Close;
static  FP_FT_Read                   pFT_Read;
static  FP_FT_Write                  pFT_Write;
static  FP_FT_IoCtl                  pFT_IoCtl;
static  FP_FT_SetDivisor             pFT_SetDivisor;
static  FP_FT_SetBaudRate            pFP_FT_SetBaudRate;
static  FP_FT_SetBaudRate            pFT_SetBaudRate;
static  FP_FT_SetDataCharacteristics pFT_SetDataCharacteristics;
static  FP_FT_SetFlowControl         pFT_SetFlowControl;
static  FP_FT_Purge                  pFT_Purge;
static  FP_FT_GetStatus              pFT_GetStatus;
static  FP_FT_ResetDevice            pFT_ResetDevice;

static  FP_FT_CreateDeviceInfoList   pFT_CreateDeviceInfoList;
static  FP_FT_GetDeviceInfoList      pFT_GetDeviceInfoList;
static  FP_FT_GetDeviceInfoDetail    pFT_GetDeviceInfoDetail;
static  FP_FT_GetDriverVersion       pFT_GetDriverVersion;
static  FP_FT_GetLibraryVersion      pFT_GetLibraryVersion;
static  FP_FT_Rescan                 pFT_Rescan;
static  FP_FT_Reload                 pFT_Reload ;
#endif
public:

virtual ~QFtdiLibrary() override;
static QFtdiLibrary * get_instance();

static FT_STATUS  FT_Open(int deviceNumber,FT_HANDLE *pHandle);
static FT_STATUS  FT_OpenEx(PVOID pArg1,uint32_t Flags,FT_HANDLE *pHandle);
static FT_STATUS  FT_ListDevices(PVOID pArg1,PVOID pArg2,uint32_t Flags);
static FT_STATUS  FT_Close(FT_HANDLE ftHandle);
static FT_STATUS  FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, uint32_t dwBytesToRead, LPDWORD lpBytesReturned);
static FT_STATUS  FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer, uint32_t dwBytesToWrite, LPDWORD  lpBytesWritten);
static FT_STATUS  FT_IoCtl(FT_HANDLE ftHandle, uint32_t dwIoControlCode, LPVOID lpInBuf, uint32_t nInBufSize, LPVOID lpOutBuf, uint32_t nOutBufSize, LPDWORD  lpBytesReturned);
static FT_STATUS  FT_SetBaudRate(FT_HANDLE ftHandle,ULONG BaudRate);
static FT_STATUS  FT_SetDivisor(FT_HANDLE ftHandle,USHORT Divisor);
static FT_STATUS  FT_SetDataCharacteristics(FT_HANDLE ftHandle,UCHAR WordLength,UCHAR StopBits,UCHAR Parity);
static FT_STATUS  FT_SetFlowControl(FT_HANDLE ftHandle,USHORT FlowControl,UCHAR XonChar,UCHAR XoffChar);
static FT_STATUS  FT_ResetDevice(FT_HANDLE ftHandle);
static FT_STATUS  FT_SetDtr(FT_HANDLE ftHandle);
static FT_STATUS  FT_ClrDtr(FT_HANDLE ftHandle);
static FT_STATUS  FT_SetRts(FT_HANDLE ftHandle);
static FT_STATUS  FT_ClrRts(FT_HANDLE ftHandle);
static FT_STATUS  FT_GetModemStatus(FT_HANDLE ftHandle,ULONG *pModemStatus);
static FT_STATUS  FT_SetChars(FT_HANDLE ftHandle,UCHAR EventChar,UCHAR EventCharEnabled,UCHAR ErrorChar,UCHAR ErrorCharEnabled);
static FT_STATUS  FT_Purge(FT_HANDLE ftHandle,ULONG Mask);
static FT_STATUS  FT_SetTimeouts(FT_HANDLE ftHandle,ULONG ReadTimeout,ULONG WriteTimeout);
static FT_STATUS  FT_GetQueueStatus(FT_HANDLE ftHandle,uint32_t *dwRxBytes);
static FT_STATUS  FT_SetEventNotification(FT_HANDLE ftHandle,uint32_t Mask,PVOID Param);
static FT_STATUS  FT_GetStatus(FT_HANDLE ftHandle, LPDWORD dwRxBytes, LPDWORD dwTxBytes, LPDWORD dwEventuint32_t);

static FT_STATUS  FT_CreateDeviceInfoList(LPDWORD  lpdwNumDevs );
static FT_STATUS  FT_GetDeviceInfoList   (FT_DEVICE_LIST_INFO_NODE *pDest,LPDWORD  lpdwNumDevs );
static FT_STATUS  FT_GetDeviceInfoDetail ( uint32_t dwIndex,LPDWORD  lpdwFlags,LPDWORD  lpdwType, LPDWORD  lpdwID,LPDWORD  lpdwLocId, LPVOID lpSerialNumber, LPVOID lpDescription,FT_HANDLE *pftHandle );


static FT_STATUS  FT_GetDriverVersion    (FT_HANDLE ftHandle,LPDWORD  lpdwVersion  );
static FT_STATUS  FT_GetLibraryVersion   (LPDWORD  lpdwVersion);
static FT_STATUS  FT_Rescan              (  void  );
static FT_STATUS  FT_Reload              ( WORD wVid, WORD wPid  );


template <typename T>
bool get_proc_addr(T & fptr, const char * func_name)
{
  if(!fptr)
  {
      fptr = T(resolve(func_name));
      if(!fptr) qDebug()<<errorString();
  }
  return fptr;
}

};


#endif
