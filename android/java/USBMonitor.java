package com.gongjianbo.demo;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;

import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;
import android.util.Log;

import java.util.HashMap;

public class USBMonitor {
    private static final String LogTag = "[USBMonitor]";
    private static final String ACTION_USB_PERMISSION = "$ACTION_USB_PERMISSION";
    private UsbManager manager;
    private PendingIntent permissionIntent;
    private Context context;
    private UsbDeviceConnection connection = null;
    private UsbDevice connectDevice;

    public USBMonitor(Context ctx) {
        Log.e(LogTag, String.format("init USBMonitor. 0x%x", Thread.currentThread().getId()));
        context = ctx;

        // Qt 和安卓 UI 线程不是同一个，所以把初始化 post 到安卓 UI 线程去执行
        Looper mainLooper = Looper.getMainLooper();
        Handler handler = new Handler(mainLooper);
        handler.post(initRunable);
    }

    protected void finalize() {
        Log.e(LogTag, "free USBMonitor");
        if (connection != null) {
            closeDevice();
            connection.close();
            connection = null;
        }
    }

    // 初始化放到 UI 线程执行
    private Runnable initRunable = new Runnable() {
        @Override
        public void run() {
            Log.e(LogTag, String.format("init Runable doing. 0x%x", Thread.currentThread().getId()));
            manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
            permissionIntent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_UPDATE_CURRENT);

            IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
            filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
            filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
            context.registerReceiver(stateReceiver, filter);

            HashMap<String, UsbDevice> usbList = manager.getDeviceList();
            for (String key : usbList.keySet()) {
                UsbDevice usb_device = usbList.get(key);
                if (usb_device == null)
                    continue;
                if (!manager.hasPermission(usb_device)) {
                    manager.requestPermission(usb_device, permissionIntent);
                    continue;
                }
                connection = manager.openDevice(usb_device);
                if (connection == null)
                    continue;

                String msg = "Attached vid:" + usb_device.getVendorId() + " pid:" + usb_device.getProductId();
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();

                int fd = connection.getFileDescriptor();
                int vid = usb_device.getVendorId();
                int pid = usb_device.getProductId();
                if (openDevice(fd, vid, pid)) {
                    connectDevice = usb_device;
                    break;
                } else {
                    connection.close();
                    connection = null;
                }
            }
        }
    };

    // 处理USB事件广播
    private BroadcastReceiver stateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            Log.e(LogTag, String.format("state onReceive %s. 0x%x", action, Thread.currentThread().getId()));
            UsbDevice usb_device = (UsbDevice) intent.getExtras().get("device");
            if (usb_device == null)
                return;
            if (action == UsbManager.ACTION_USB_DEVICE_ATTACHED || action == ACTION_USB_PERMISSION) {
                if (!manager.hasPermission(usb_device)) {
                    manager.requestPermission(usb_device, permissionIntent);
                    return;
                }
                connection = manager.openDevice(usb_device);
                if (connection == null)
                    return;

                String msg = "Attached vid:" + usb_device.getVendorId() + " pid:" + usb_device.getProductId();
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();

                int fd = connection.getFileDescriptor();
                int vid = usb_device.getVendorId();
                int pid = usb_device.getProductId();
                if (openDevice(fd, vid, pid)) {
                    connectDevice = usb_device;
                } else {
                    connection.close();
                    connection = null;
                }
            } else if (action == UsbManager.ACTION_USB_DEVICE_DETACHED) {
                String msg = "Detached vid:" + usb_device.getVendorId() + " pid:" + usb_device.getProductId();
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
                // 这里只判断了 vid pid，但不一定就是实际打开的设备
                if (connection != null &&
                    connectDevice.getVendorId() == usb_device.getVendorId() &&
                    connectDevice.getProductId() == usb_device.getProductId()) {
                    closeDevice();
                    connection.close();
                    connection = null;
                }
            }
        }
    };

    public native boolean openDevice(int fd, int vid, int pid);

    public native void closeDevice();
}
