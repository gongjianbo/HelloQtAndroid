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

// USB 插拔检测
// TargetSdkVersion > 27 需要先获取 CAMERA 权限，不然 USB Camera 不能弹出授权弹框
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
            for (String name : usbList.keySet()) {
                UsbDevice usb_device = usbList.get(name);
                if (usb_device == null) {
                    Log.e(LogTag, String.format("usb_device is null."));
                    continue;
                }
                String msg = "Attached vid:" + usb_device.getVendorId() + " pid:" + usb_device.getProductId();
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
                if (!manager.hasPermission(usb_device)) {
                    manager.requestPermission(usb_device, permissionIntent);
                    continue;
                }
                if (onAttach(usb_device))
                    break;
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
            if (usb_device == null) {
                Log.e(LogTag, String.format("usb_device is null."));
                return;
            }
            // String name = usb_device.getDeviceName();
            if (action == ACTION_USB_PERMISSION) {
                if (!manager.hasPermission(usb_device)) {
                    Log.e(LogTag, String.format("no permission."));
                    return;
                }
                onAttach(usb_device);
            } else if (action == UsbManager.ACTION_USB_DEVICE_ATTACHED) {
                String msg = "Attached vid:" + usb_device.getVendorId() + " pid:" + usb_device.getProductId();
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
                if (!manager.hasPermission(usb_device)) {
                    Log.e(LogTag, String.format("request permission."));
                    manager.requestPermission(usb_device, permissionIntent);
                    return;
                }
                onAttach(usb_device);
            } else if (action == UsbManager.ACTION_USB_DEVICE_DETACHED) {
                String msg = "Detached vid:" + usb_device.getVendorId() + " pid:" + usb_device.getProductId();
                Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
                onDetach(usb_device);
            }
        }
    };

    private boolean onAttach(final UsbDevice device) {
        if (connection != null)
            return false;
        connection = manager.openDevice(device);
        if (connection == null) {
            Log.e(LogTag, String.format("UsbManager openDeivce failed."));
            return false;
        }

        int fd = connection.getFileDescriptor();
        int vid = device.getVendorId();
        int pid = device.getProductId();
        if (openDevice(fd, vid, pid)) {
            connectDevice = device;
            return true;
        } else {
            Log.e(LogTag, String.format("open device failed."));
            connection.close();
            connection = null;
        }
        return false;
    }

    private void onDetach(final UsbDevice device) {
        // 这里只判断了 vid pid，但不一定就是实际打开的设备
        if (connection != null &&
            connectDevice.getVendorId() == device.getVendorId() &&
            connectDevice.getProductId() == device.getProductId()) {
            closeDevice();
            connection.close();
            connection = null;
        }
    }

    public native boolean openDevice(int fd, int vid, int pid);

    public native void closeDevice();
}
