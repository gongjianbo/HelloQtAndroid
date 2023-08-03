// 根据包名在AndroidManifest.xml中指定Activity的name路径（=包名.类名）
package com.gongjianbo.demo;
import com.gongjianbo.demo.*;
import org.qtproject.qt5.android.bindings.QtActivity;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import android.app.Activity;
import android.Manifest;
import android.view.WindowManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;
 
public class MainActivity extends QtActivity {
    private String LogTag = "[MainActivity]";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.e(LogTag, String.format("onCreate MainActivity start. 0x%x", Thread.currentThread().getId()));
        super.onCreate(savedInstanceState);
        Log.e(LogTag, String.format("getApplicationContext() equals MyApplication.getContext() ? %b",
            getApplicationContext().equals(MyApplication.getContext())));
        Log.e(LogTag, String.format("getApplicationContext() equals Activity ? %b",
            getApplicationContext().equals(this)));
        // 设置屏幕常量
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // 相机权限
        verifyCameraPermissions(this);
        // 文件读写权限
        verifyStoragePermissions(this);

        Log.e(LogTag, "onCreate MainActivity finish");
    }

    // 相机权限
    public static void verifyCameraPermissions(Activity activity) {
        int request_code = 1;
        if (ActivityCompat.checkSelfPermission(activity, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            // 如果没有，获取读取和写入动态权限
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
                Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                activity.startActivityForResult(intent, 1024);
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
}
