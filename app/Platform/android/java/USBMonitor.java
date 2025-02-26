package com.gongjianbo.demo;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;
import android.text.TextUtils;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;
import java.util.HashSet;
import java.util.Set;

// USB 插拔检测
// TargetSdkVersion > 27 需要先获取 CAMERA 权限，不然 USB Camera 不能弹出授权弹框
public class USBMonitor {
    private static final String LogTag = "[USBMonitor]";
    private static final String ACTION_USB_PERMISSION = "$ACTION_USB_PERMISSION";
    private UsbManager manager;
    private Context context;
    // 保存连接，用于下次插拔信号时判断
    private HashMap<UsbDevice, UsbDeviceConnection> deviceMap = new HashMap<>();
    // 保存拒绝授权的设备，下次枚举时不请求
    private HashSet<UsbDevice> disableSet = new HashSet<>();

    public USBMonitor() {
        Log.e(LogTag, String.format("init USBMonitor. 0x%x", Thread.currentThread().getId()));
        context = MyApplication.getContext();

        // Qt 和安卓 UI 线程不是同一个，所以把初始化 post 到安卓 UI 线程去执行
        Looper mainLooper = Looper.getMainLooper();
        Handler handler = new Handler(mainLooper);
        handler.post(initRunable);
    }

    protected void finalize() {
        Log.e(LogTag, "free USBMonitor");
        synchronized (this) {
            for (Map.Entry<UsbDevice, UsbDeviceConnection> entry : deviceMap.entrySet()) {
                UsbDeviceConnection connection = entry.getValue();
                final int fd = connection.getFileDescriptor();
                jniDeviceDetach(fd);
                connection.close();
            }
            deviceMap.clear();
            disableSet.clear();
        }
    }

    // 初始化放到 UI 线程执行
    private Runnable initRunable = new Runnable() {
        @Override
        public void run() {
            Log.e(LogTag, String.format("init Runable doing. 0x%x", Thread.currentThread().getId()));
            manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);

            IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
            filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
            filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
            context.registerReceiver(stateReceiver, filter);

            enumDevice();
        }
    };

    // 处理USB事件广播
    private BroadcastReceiver stateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            Log.e(LogTag, String.format("state onReceive %s. 0x%x", action, Thread.currentThread().getId()));
            synchronized (this) {
                UsbDevice usb_device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                if (usb_device == null) {
                    Log.e(LogTag, String.format("usb_device is null."));
                    return;
                }
                // String name = usb_device.getDeviceName();
                if (action == ACTION_USB_PERMISSION) {
                    if (!manager.hasPermission(usb_device)) {
                        Log.e(LogTag, String.format("no permission."));
                        disableSet.add(usb_device);
                    }
                    enumDevice();
                } else if (action == UsbManager.ACTION_USB_DEVICE_ATTACHED) {
                    String msg = String.format("Attached vid: 0x%x  pid: 0x%x", usb_device.getVendorId(), usb_device.getProductId());
                    Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
                    if (!manager.hasPermission(usb_device)) {
                        Log.e(LogTag, String.format("attach request permission. %d", disableSet.size()));
                        requestPermission(usb_device);
                        return;
                    }
                    onAttach(usb_device);
                } else if (action == UsbManager.ACTION_USB_DEVICE_DETACHED) {
                    String msg = String.format("Detached vid: 0x%x  pid: 0x%x", usb_device.getVendorId(), usb_device.getProductId());
                    Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
                    onDetach(usb_device);
                }
            }
        }
    };

    // 设备类型deviceType：0一般设备，1UVC，2其他不需要处理的设备
    private int getDeviceType(UsbDevice device) {
        for (int i = 0; i < device.getInterfaceCount(); i++)
        {
            UsbInterface inter = device.getInterface(i);
            Log.e(LogTag, String.format("interface class %d.", inter.getInterfaceClass()));
            switch (inter.getInterfaceClass()) {
                case UsbConstants.USB_CLASS_VIDEO: return 1;
                case UsbConstants.USB_CLASS_CDC_DATA: return 2;
                case UsbConstants.USB_CLASS_MASS_STORAGE: return 2;
            }
        }
        return 0;
    }

    // 请求权限
    private void requestPermission(UsbDevice device) {
        Intent intent = new Intent(ACTION_USB_PERMISSION);
        intent.setPackage(context.getPackageName());
        // API 31 之后 flag 不能单独设置 PendingIntent.FLAG_UPDATE_CURRENT
        // PendingIntent.FLAG_IMMUTABLE 没法在授权后获取到设备信息，所以用 PendingIntent.FLAG_MUTABLE
        int flags = (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S ? PendingIntent.FLAG_MUTABLE : 0) | PendingIntent.FLAG_UPDATE_CURRENT;
        PendingIntent permission = PendingIntent.getBroadcast(context, device.getDeviceId(), intent, flags);
        manager.requestPermission(device, permission);
    }

    // 枚举设备，请求权限
    private void enumDevice() {
        HashMap<String, UsbDevice> usbList = manager.getDeviceList();
        for (String name : usbList.keySet()) {
            UsbDevice usb_device = usbList.get(name);
            if (usb_device == null) {
                Log.e(LogTag, String.format("usb_device is null."));
                continue;
            }
            UsbDeviceConnection connection = deviceMap.get(usb_device);
            if (connection != null) {
                Log.e(LogTag, String.format("usb_device is connected."));
                continue;
            }
            // String msg = String.format("Attached vid: 0x%x  pid: 0x%x", usb_device.getVendorId(), usb_device.getProductId());
            // Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
            if (!manager.hasPermission(usb_device)) {
                if (!disableSet.contains(usb_device)) {
                    Log.e(LogTag, String.format("enum request permission."));
                    requestPermission(usb_device);
                    return;
                }
                continue;
            }
            onAttach(usb_device);
        }
    }

    private boolean onAttach(final UsbDevice device) {
        Log.e(LogTag, String.format("onAttach vid: 0x%x  pid: 0x%x", device.getVendorId(), device.getProductId()));
        UsbDeviceConnection connection = manager.openDevice(device);
        if (connection == null) {
            Log.e(LogTag, String.format("UsbManager openDeivce failed."));
            return false;
        }

        final int fd = connection.getFileDescriptor();
        final int vid = device.getVendorId();
        final int pid = device.getProductId();
        final int device_type = getDeviceType(device);
        final String device_name = device.getDeviceName();
        final String product_name = device.getProductName();
        if (jniDeviceAttach(fd, vid, pid, device_type, device_name, product_name)) {
            deviceMap.put(device, connection);
            return true;
        } else {
            Log.e(LogTag, String.format("open device failed."));
            connection.close();
        }
        return false;
    }

    private void onDetach(final UsbDevice device) {
        Log.e(LogTag, String.format("onDetach vid: 0x%x  pid: 0x%x", device.getVendorId(), device.getProductId()));
        UsbDeviceConnection connection = deviceMap.get(device);
        if (connection != null) {
            final int fd = connection.getFileDescriptor();
            jniDeviceDetach(fd);
            connection.close();
        }
        deviceMap.remove(device);
        disableSet.remove(device);
    }

    // [JNI] 设备插入
    public native boolean jniDeviceAttach(int fd, int vid, int pid, int deviceType, String deviceName, String productName);
    // [JNI] 设备拔出
    public native void jniDeviceDetach(int fd);
}
