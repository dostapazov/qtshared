package org.qtproject.deviceinterface.usbftdi;


import android.util.Log;
import android.os.Bundle;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.hardware.usb.UsbManager;

import java.util.HashMap;
import java.lang.Integer;
import 	java.lang.String;
import android.app.Activity;

import android.content.Context;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;


import com.ftdi.j2xx.D2xxManager;

public class ftdiActivity extends org.qtproject.qt5.android.bindings.QtActivity
{

    public static D2xxManager ftD2xx = null;

    public static HashMap<Integer,qtFTDIInterface> chips = null;
    private static Activity mActivity = null;
    public static int nextHandle = 1;


    private static String TAG = "ftdiActivity";
    private static PowerManager.WakeLock wl = null;
    private static PowerManager pm = null;


    //
    //
    //

    public static String busScan()
    {
        String res = "";
        int devCount = 0;

        if (ftD2xx != null)
        {
            devCount = ftD2xx.createDeviceInfoList(mActivity);
            Log.i(TAG, "Device number = " + Integer.toString(devCount));
            for (int i = 0; i < devCount; i++)
            {
                D2xxManager.FtDeviceInfoListNode deviceListNode = ftD2xx.getDeviceInfoListDetail(i);
                res += deviceListNode.serialNumber+",";
            }
        }
        return res;
    }

    public static int bytesAvailable(int handle)
    {
        Log.i(TAG,"public static int bytesAvailable(int handle) hanlde = "+handle);
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return -1;
        return chFace.bytesToRead();
    }

    public static int open(String serNum, int baudRate, byte bits, byte stopBits, byte parity)
    {
        Log.i(TAG,"public static int open(String serNum, int baudRate, byte bits, byte stopBits, byte parity) [1]");
        qtFTDIInterface chFace = new qtFTDIInterface(mActivity, ftD2xx);
        if (chFace == null) return -1;
        int rs = chFace.open(serNum,baudRate,bits,stopBits,parity);
        int handle = 0;
        if (rs == 1)
        {
            handle = ++nextHandle;
            chips.put(handle, chFace);
        }
        else
        {
            handle = rs;
            chFace.close();
        }
        Log.i(TAG,"public static int open(String serNum, int baudRate, byte bits, byte stopBits, byte parity) [2]");
        return handle;
    }


    public static int write(int handle, byte[] data)
    {
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return -1;
        return chFace.write(data, data.length);
    }

    public static byte[] read(int handle, int len)
    {
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return new byte[0];
        return chFace.read(len);
    }



    public static int prepareSendBuffer(int handle, int len)
    {
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return -1;
        return chFace.prepareSendBuffer(len);
    }

    public static int setByte(int handle, byte data, int index)
    {
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return -1;
        return chFace.setByte(data,index);
    }

    public static int writeSendBuffer(int handle)
    {
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return -1;
        return chFace.writeSendBuffer();
    }

    public static void close(int handle)
    {
        qtFTDIInterface chFace = chips.get(handle);
        if (chFace == null) return;
        chFace.close();
        chips.remove(handle);
    }

    //
    //
    //

    @Override
        public void onCreate(Bundle savedInstanceState) {
            try {
                    ftD2xx = D2xxManager.getInstance(this);
            } catch (D2xxManager.D2xxException ex) {
                    ex.printStackTrace();
            }
            super.onCreate(savedInstanceState);

            IntentFilter filter = new IntentFilter();
            filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
            filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
            filter.setPriority(500);

            mActivity = this;
            nextHandle = 0;

            chips = new HashMap<Integer,qtFTDIInterface>();


            //disable sleep mode
            pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
            wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, "NO SLEEP");
            wl.acquire();
        }

    @Override
        protected void onStart()
        {
            super.onStart();
            //unlock device
            Window wind = this.getWindow();

            wind.addFlags(LayoutParams.FLAG_DISMISS_KEYGUARD);
            wind.addFlags(LayoutParams.FLAG_SHOW_WHEN_LOCKED);
            wind.addFlags(LayoutParams.FLAG_TURN_SCREEN_ON);
        }



    @Override
        protected void onDestroy()
        {
            //enable sleep mode
            if (wl != null) wl.release();
            super.onDestroy();
        }



    //
    //
    //
}
