package org.qtproject.deviceinterface.usbftdi;


import java.lang.Object;
import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.util.Log;
import android.app.Activity;
import java.lang.Exception;
import java.io.IOException;
import java.util.Map;
import java.lang.String;

import com.ftdi.j2xx.D2xxManager;
import com.ftdi.j2xx.FT_Device;


public class qtFTDIInterface
{

    public static Context pContext;
    public static D2xxManager ftdid2xx;
    public static FT_Device ftDevice = null;
    public static D2xxManager.DriverParameters d2xxDrvParameter;

    public static byte[] sendBuffer = null;
    //
    //
    //

    private static String TAG = "qtFTDIInterface";

    //
    //
    //

    public qtFTDIInterface()
    {
    }

    public qtFTDIInterface(Context parentContext , D2xxManager ftdid2xxContext)
    {
        pContext = parentContext;
        ftdid2xx = ftdid2xxContext;
        Log.i(TAG,"public qtFTDIInterface(Context parentContext , D2xxManager ftdid2xxContext)");
    }

    public int open(String serNum, int baudRate, byte bits, byte stopBits, byte parity) //0 - fail  Can not open,  1 - ok
    {
        Log.v(TAG,"Request for open "+serNum);
        Log.v(TAG,"baudRate = "+baudRate+", bits = "+bits+", stopBits = "+stopBits+", parity = "+parity);
        if (ftDevice != null)
        {
            ftDevice.close();
            ftDevice = null;
        }

        int res = 1;

        ftDevice = ftdid2xx.openBySerialNumber(pContext,serNum);
        if (ftDevice == null) return 0;

        //Set channel parameters
        if (ftDevice.isOpen())
        {

            if (!ftDevice.setBaudRate(baudRate)) res = -1;
            if (res > 0)
                if (!ftDevice.setDataCharacteristics(bits, stopBits, parity)) res = -2;

            //Unsupported
            /*if (res > 0)
                if (!ftDevice.setFlowControl(D2xxManager.FT_FLOW_NONE, (byte)0x0b, (byte)0x0d)) res = -3;*/

            //Cleanup buffers
            if (res > 0)
            {
                ftDevice.purge(D2xxManager.FT_PURGE_RX);
                ftDevice.purge(D2xxManager.FT_PURGE_TX);
            }

            if (res < 1)
            {
                ftDevice.close();
                ftDevice = null;
            }
            return res;

        }
        return 0;
    }

    public int write(byte[] data, int size)
    {
        if (ftDevice == null) return -1;
        int w = 0 ;
        w = ftDevice.write(data, size, false);
        return w;
    }

    public int bytesToRead()
    {
        if (ftDevice == null) return -1;
        if (!ftDevice.isOpen()) return -1;
        int r = ftDevice.getQueueStatus();
        Log.v(TAG,"public int bytesToRead() = "+r);
        return r;
    }

    public byte[] read(int len)
    {
        Log.v(TAG,"public byte[] read(int len) len = "+len);
        byte[] res = new byte[len];
        int readed = 0;
        if (ftDevice != null) readed = ftDevice.read(res, len, 100);
        return res;
    }

    public void close()
    {
        if (ftDevice == null) return;
        ftDevice.close();
        ftDevice = null;
    }


    public int prepareSendBuffer(int len)
    {
        sendBuffer = new byte[len];
        return sendBuffer.length;
    }

    public int setByte(byte data, int index)
    {
        if (index > sendBuffer.length) return 0;
        sendBuffer[index] = data;
        return 1;
    }

    public int writeSendBuffer()
    {
        return write(sendBuffer, sendBuffer.length);
    }

}
