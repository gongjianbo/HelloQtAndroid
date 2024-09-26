// 根据包名在AndroidManifest.xml中指定Activity的name路径（=包名.类名）
package com.gongjianbo.demo;
import org.qtproject.qt5.android.bindings.QtActivity;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import android.app.Activity;
import android.Manifest;
import android.view.View;
import android.view.Surface;
import android.view.Window;
import android.view.WindowManager;
import android.view.MotionEvent;
import android.view.OrientationEventListener;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;
 
public class MainActivity extends QtActivity {
    private String LogTag = "[MainActivity]";
    private OrientationEventListener orientationListener = null;
    private int activityRotation = -1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.e(LogTag, String.format("onCreate MainActivity start. 0x%x", Thread.currentThread().getId()));
        super.onCreate(savedInstanceState);
        Log.e(LogTag, String.format("getApplicationContext() equals MyApplication.getContext() ? %b",
            getApplicationContext().equals(MyApplication.getContext())));
        Log.e(LogTag, String.format("getApplicationContext() equals Activity ? %b",
            getApplicationContext().equals(this)));
        MyApplication.setActivity(this);
        // 设置屏幕常亮
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // 相机权限
        verifyCameraPermissions(this);
        // 文件读写权限
        verifyStoragePermissions(this);
        Log.e(LogTag, "onCreate MainActivity finish");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        disableRotationListen();
    }

    // 获取旋转角度
    public int getRotation() {
        int angle = getWindowManager().getDefaultDisplay().getRotation();
        switch (angle) {
            case Surface.ROTATION_0: return 0;
            case Surface.ROTATION_90: return 90;
            case Surface.ROTATION_180: return 180;
            case Surface.ROTATION_270: return 270;
        }
        return -1;
    }

    // 初始化角度设置，需要初始化JNI后调用
    public void initRotation() {
        // 监听方向变化
        orientationListener = new OrientationEventListener(this) {
            @Override
            public void onOrientationChanged(int orientation) {
                if (orientation == OrientationEventListener.ORIENTATION_UNKNOWN)
                    return;
                int rotation = getRotation();
                if (rotation != activityRotation) {
                    activityRotation = rotation;
                    jniActivityRotationChanged(activityRotation);
                }
            }
        };
    }

    // 开启监听方向变化
    public void enableRotationListen() {
        if (orientationListener == null)
            return;
        if (orientationListener.canDetectOrientation()) {
            orientationListener.enable();
        }
        activityRotation = getRotation();
        jniActivityRotationChanged(activityRotation);
    }

    // 结束监听方向变化
    public void disableRotationListen() {
        if (orientationListener == null)
            return;
        orientationListener.disable();
    }

    // 相机权限
    public static void verifyCameraPermissions(Activity activity) {
        int request_code = 1;
        if (ActivityCompat.checkSelfPermission(activity, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            // 如果没有，获取相机权限
            ActivityCompat.requestPermissions(activity, new String[]{
                    Manifest.permission.CAMERA
            }, request_code);
        }
    }

    // 文件读写权限
    public static void verifyStoragePermissions(Activity activity) {
        // >=11 外部文件需要特殊的权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                // APP当前程序，ALL所有程序
                try {
                    Uri uri = Uri.parse("package:" + BuildConfig.APPLICATION_ID);
                    Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION, uri);
                    activity.startActivityForResult(intent, 1024);
                } catch (Exception ex){
                    Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                    activity.startActivityForResult(intent, 1024);
                }
            }
        } else {
            // >=6 检查读取和写入动态权限
            if (ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                    || ActivityCompat.checkSelfPermission(activity, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                // 如果没有，获取读取和写入动态权限
                ActivityCompat.requestPermissions(activity, new String[]{
                        Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                }, 1024);
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode != 1024) return;
        Log.e(LogTag, String.format("onRequestPermissionsResult %d", requestCode));
        // if (grantResults[0] != PackageManager.PERMISSION_GRANTED || grantResults[1] != PackageManager.PERMISSION_GRANTED) {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                || ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            Toast.makeText(this, "Unauthorized file operation", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode != 1024) return;
        Log.e(LogTag, String.format("onRequestPermissionsResult %d %d", requestCode, resultCode));
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Toast.makeText(this, "Unauthorized file operation", Toast.LENGTH_SHORT).show();
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (event.getAction() == MotionEvent.ACTION_CANCEL) {
            jniTouchEventCanceled();
        }
        return super.onTouchEvent(event);
    }


    @Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {
        if (ev.getAction() == MotionEvent.ACTION_CANCEL) {
            jniTouchEventCanceled();
        }
        return super.dispatchTouchEvent(ev);
    }

    // [JNI] 触摸释放
    public static native void jniTouchEventCanceled();
    // [JNI] 方向变化
    public static native void jniActivityRotationChanged(int rotation);
}
