#include "ftdilib.h"
#include <qtextcodec.h>


QScopedPointer<QFtdiLibrary> QFtdiLibrary::ftdi_lib ;
QString         QFtdiLibrary::error_string;

QFtdiLibrary::~QFtdiLibrary()
{
   unload();
}

#ifdef Q_OS_WIN
 #ifdef Q_OS_WIN64
  constexpr const char * lib_name = "ftd2xx.dll";
 #else
  constexpr const char * lib_name = "ftd2xx.dll";
 #endif
#endif

#ifdef Q_OS_LINUX
  constexpr const char * lib_name = "/usr/local/lib/libftd2xx.so";
#endif


QFtdiLibrary * QFtdiLibrary::get_instance()
{
  if(!ftdi_lib)
  {
    ftdi_lib.reset( new QFtdiLibrary);
  }
#ifndef FTDI_IMPLICIT_LINK
  if(!ftdi_lib->isLoaded())
  {
      ftdi_lib->setFileName(QLatin1String(lib_name));
      if(!ftdi_lib->load())
      {
          error_string =  ftdi_lib->errorString().toLocal8Bit();
      }
  }
#endif
  return ftdi_lib.data();
}

#ifndef FTDI_IMPLICIT_LINK
QFtdiLibrary::FP_FT_Open                   QFtdiLibrary::pFT_Open                   = Q_NULLPTR;
QFtdiLibrary::FP_FT_OpenEx                 QFtdiLibrary::pFT_OpenEx                 = Q_NULLPTR;
QFtdiLibrary::FP_FT_ListDevices            QFtdiLibrary::pFT_ListDevices            = Q_NULLPTR;
QFtdiLibrary::FP_FT_Close                  QFtdiLibrary::pFT_Close                  = Q_NULLPTR;
QFtdiLibrary::FP_FT_Read                   QFtdiLibrary::pFT_Read                   = Q_NULLPTR;
QFtdiLibrary::FP_FT_Write                  QFtdiLibrary::pFT_Write                  = Q_NULLPTR;
QFtdiLibrary::FP_FT_IoCtl                  QFtdiLibrary::pFT_IoCtl                  = Q_NULLPTR;
QFtdiLibrary::FP_FT_SetDivisor             QFtdiLibrary::pFT_SetDivisor             = Q_NULLPTR;
QFtdiLibrary::FP_FT_SetBaudRate            QFtdiLibrary::pFP_FT_SetBaudRate         = Q_NULLPTR;
QFtdiLibrary::FP_FT_SetBaudRate            QFtdiLibrary::pFT_SetBaudRate            = Q_NULLPTR;
QFtdiLibrary::FP_FT_SetDataCharacteristics QFtdiLibrary::pFT_SetDataCharacteristics = Q_NULLPTR;
QFtdiLibrary::FP_FT_SetFlowControl         QFtdiLibrary::pFT_SetFlowControl         = Q_NULLPTR;
QFtdiLibrary::FP_FT_Purge                  QFtdiLibrary::pFT_Purge                  = Q_NULLPTR;
QFtdiLibrary::FP_FT_GetStatus              QFtdiLibrary::pFT_GetStatus              = Q_NULLPTR;

QFtdiLibrary::FP_FT_CreateDeviceInfoList   QFtdiLibrary::pFT_CreateDeviceInfoList  = Q_NULLPTR;
QFtdiLibrary::FP_FT_GetDeviceInfoList      QFtdiLibrary::pFT_GetDeviceInfoList     = Q_NULLPTR;
QFtdiLibrary::FP_FT_GetDeviceInfoDetail    QFtdiLibrary::pFT_GetDeviceInfoDetail   = Q_NULLPTR;
QFtdiLibrary::FP_FT_GetDriverVersion       QFtdiLibrary::pFT_GetDriverVersion      = Q_NULLPTR;
QFtdiLibrary::FP_FT_GetLibraryVersion      QFtdiLibrary::pFT_GetLibraryVersion     = Q_NULLPTR;
QFtdiLibrary::FP_FT_Rescan                 QFtdiLibrary::pFT_Rescan                = Q_NULLPTR;
QFtdiLibrary::FP_FT_Reload                 QFtdiLibrary::pFT_Reload                = Q_NULLPTR;
QFtdiLibrary::FP_FT_ResetDevice            QFtdiLibrary::pFT_ResetDevice = Q_NULLPTR;
#endif



FT_STATUS  QFtdiLibrary::FT_Open(int deviceNumber,FT_HANDLE *pHandle)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Open,"FT_Open"))
      ret = pFT_Open(deviceNumber, pHandle);
#else
    ret = ::FT_Open(deviceNumber, pHandle);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_OpenEx(PVOID pArg1,uint32_t Flags,FT_HANDLE *pHandle)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_OpenEx, "FT_OpenEx"))
      ret = pFT_OpenEx(pArg1, Flags,  pHandle);
#else
    ret = ::FT_OpenEx(pArg1, Flags,  pHandle);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_ListDevices(PVOID pArg1,PVOID pArg2,uint32_t Flags)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_ListDevices, "FT_ListDevices" ) )
      ret = pFT_ListDevices(pArg1, pArg2,  Flags);
#else
    ret = ::FT_ListDevices(pArg1, pArg2,  Flags);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_Close(FT_HANDLE ftHandle)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Close, "FT_Close"))
      ret = pFT_Close(ftHandle);
#else
        ret = ::FT_Close(ftHandle);
#endif
    return ret;
}



FT_STATUS  QFtdiLibrary::FT_Read(FT_HANDLE ftHandle,LPVOID lpBuffer,uint32_t dwBytesToRead,LPDWORD lpBytesReturned)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Read, "FT_Read"))
      ret = pFT_Read(ftHandle, lpBuffer, dwBytesToRead, lpBytesReturned);
#else
    ret = ::FT_Read(ftHandle, lpBuffer, dwBytesToRead, lpBytesReturned);
#endif
 return ret;
}

FT_STATUS  QFtdiLibrary::FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer,uint32_t dwBytesToWrite,LPDWORD  lpBytesWritten)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Write, "FT_Write"))
      ret = pFT_Write(ftHandle, lpBuffer, dwBytesToWrite, lpBytesWritten);
#else
      ret = ::FT_Write(ftHandle, lpBuffer, dwBytesToWrite, lpBytesWritten);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_IoCtl(FT_HANDLE ftHandle,uint32_t dwIoControlCode,LPVOID lpInBuf,uint32_t nInBufSize,LPVOID lpOutBuf,uint32_t nOutBufSize,LPDWORD  lpBytesReturned)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_IoCtl, "FT_IoCtl"))
      ret = pFT_IoCtl(ftHandle, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpBytesReturned ,Q_NULLPTR );
#else
    ret = ::FT_IoCtl(ftHandle, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpBytesReturned ,Q_NULLPTR );
#endif
     return ret;
}

FT_STATUS  QFtdiLibrary::FT_SetBaudRate(FT_HANDLE ftHandle,ULONG BaudRate)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_SetBaudRate, "FT_SetBaudRate"))
      ret = pFT_SetBaudRate(ftHandle, BaudRate);
#else
    ret = ::FT_SetBaudRate(ftHandle, BaudRate);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_SetDivisor(FT_HANDLE ftHandle,USHORT Divisor)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_SetDivisor, "FT_SetDivisor"))
      ret = pFT_SetDivisor(ftHandle, Divisor);
#else
      ret = ::FT_SetDivisor(ftHandle, Divisor);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_SetDataCharacteristics(FT_HANDLE ftHandle,UCHAR WordLength,UCHAR StopBits,UCHAR Parity)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_SetDataCharacteristics, "FT_SetDataCharacteristics"))
      ret = pFT_SetDataCharacteristics(ftHandle, WordLength, StopBits, Parity);
#else
    ret = ::FT_SetDataCharacteristics(ftHandle, WordLength, StopBits, Parity);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_SetFlowControl(FT_HANDLE ftHandle,USHORT FlowControl,UCHAR XonChar,UCHAR XoffChar)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_SetFlowControl, "FT_SetFlowControl"))
      ret = pFT_SetFlowControl(ftHandle, FlowControl, XonChar, XoffChar);
#else
    ret = ::FT_SetFlowControl(ftHandle, FlowControl, XonChar, XoffChar);
#endif
    return ret;
}



FT_STATUS  QFtdiLibrary::FT_ResetDevice(FT_HANDLE ftHandle)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_ResetDevice, "FT_SetFlowControl"))
      ret = pFT_ResetDevice(ftHandle);
#else
    ret = ::FT_ResetDevice(ftHandle);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_Purge(FT_HANDLE ftHandle,ULONG Mask)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Purge, "FT_Purge"))
      ret = pFT_Purge(ftHandle, Mask);
#else
    ret = ::FT_Purge(ftHandle, Mask);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_GetStatus(FT_HANDLE ftHandle,LPDWORD  dwRxBytes,LPDWORD dwTxBytes,LPDWORD dwEventuint32_t)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_GetStatus, "FT_GetStatus"))
      ret = pFT_GetStatus(ftHandle, dwRxBytes, dwTxBytes, dwEventuint32_t);
#else
    ret = ::FT_GetStatus(ftHandle, dwRxBytes, dwTxBytes, dwEventuint32_t);
#endif
    return ret;
}


//FT_STATUS  QFtdiLibrary::FT_SetEventNotification(FT_HANDLE ftHandle,uint32_t Mask,PVOID Param)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_SetTimeouts(FT_HANDLE ftHandle,ULONG ReadTimeout,ULONG WriteTimeout)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_GetQueueStatus(FT_HANDLE ftHandle,uint32_t *dwRxBytes)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_SetDtr(FT_HANDLE ftHandle)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_ClrDtr(FT_HANDLE ftHandle)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_SetRts(FT_HANDLE ftHandle)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_ClrRts(FT_HANDLE ftHandle)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_GetModemStatus(FT_HANDLE ftHandle,ULONG *pModemStatus)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

//FT_STATUS  QFtdiLibrary::FT_SetChars(FT_HANDLE ftHandle,UCHAR EventChar,UCHAR EventCharEnabled,UCHAR ErrorChar,UCHAR ErrorCharEnabled)
//{
//    FT_STATUS ret = FT_NOT_SUPPORTED;
//    return ret;
//}

FT_STATUS  QFtdiLibrary::FT_CreateDeviceInfoList(LPDWORD  lpdwNumDevs )
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_CreateDeviceInfoList, "FT_CreateDeviceInfoList"))
       ret = pFT_CreateDeviceInfoList(lpdwNumDevs);
#else
    ret = ::FT_CreateDeviceInfoList(lpdwNumDevs);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_GetDeviceInfoList   (FT_DEVICE_LIST_INFO_NODE *pDest,LPDWORD  lpdwNumDevs )
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_GetDeviceInfoList, "FT_GetDeviceInfoList"))
       ret = pFT_GetDeviceInfoList(pDest, lpdwNumDevs);
#else
    ret = ::FT_GetDeviceInfoList(pDest, lpdwNumDevs);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_GetDeviceInfoDetail ( uint32_t dwIndex,LPDWORD  lpdwFlags,LPDWORD  lpdwType, LPDWORD  lpdwID,LPDWORD  lpdwLocId, LPVOID lpSerialNumber, LPVOID lpDescription,FT_HANDLE *pftHandle )
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_GetDeviceInfoDetail, "FT_GetDeviceInfoDetail"))
       ret = pFT_GetDeviceInfoDetail(dwIndex ,lpdwFlags, lpdwType, lpdwID, lpdwLocId, lpSerialNumber, lpDescription, pftHandle);
#else
    ret = ::FT_GetDeviceInfoDetail(dwIndex ,lpdwFlags, lpdwType, lpdwID, lpdwLocId, lpSerialNumber, lpDescription, pftHandle);
#endif
    return ret;
}



FT_STATUS  QFtdiLibrary::FT_GetDriverVersion    (FT_HANDLE ftHandle,LPDWORD  lpdwVersion  )
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_GetDriverVersion, "FT_GetDriverVersion"))
       ret = pFT_GetDriverVersion(ftHandle, lpdwVersion);
#else
    ret = ::FT_GetDriverVersion(ftHandle, lpdwVersion);
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_GetLibraryVersion   (LPDWORD  lpdwVersion)
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_GetLibraryVersion, "FT_GetLibraryVersion"))
       ret = pFT_GetLibraryVersion(lpdwVersion)  ;
#else
    ret = ::FT_GetLibraryVersion(lpdwVersion)  ;
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_Rescan              (  void  )
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Rescan, "FT_Rescan"))
       ret = pFT_Rescan();
#else
    ret = ::FT_Rescan();
#endif
    return ret;
}

FT_STATUS  QFtdiLibrary::FT_Reload              ( WORD wVid, WORD wPid  )
{
    FT_STATUS ret = FT_NOT_SUPPORTED;
#ifndef FTDI_IMPLICIT_LINK
    if(get_instance()->get_proc_addr(pFT_Reload, "FT_Reload"))
       ret = pFT_Reload(wVid, wPid);
#else
    ret = ::FT_Reload(wVid, wPid);
#endif
    return ret;
}



