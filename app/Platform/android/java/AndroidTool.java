package com.gongjianbo.demo;

import android.app.Application;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.net.Uri;
import android.os.Build;
import androidx.core.content.FileProvider;
import java.io.File;
import java.util.ArrayList;

public class AndroidTool {
    private static final String LogTag = "[AndroidTool]";

    // 重启
    public static void restartApp() {
        Log.e(LogTag, "finishActivity");
        MyApplication.getActivity().finish();
        Intent intent = new Intent(MyApplication.getContext(), MainActivity.class);
        intent.addFlags(intent.FLAG_ACTIVITY_NEW_TASK);
        Log.e(LogTag, "startActivity");
        MyApplication.getContext().startActivity(intent);
        int pid = android.os.Process.myPid();
        if (pid != 0) {
            Log.e(LogTag, "killProcess");
            android.os.Process.killProcess(pid);
        }
    }

    // 回到Home界面
    public static void gotoHome() {
        Log.e(LogTag, "gotoHome");
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_HOME);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        MyApplication.getContext().startActivity(intent);
    }

    // 分享单个文件
    public static void shareFile(String path, String mimeType, String title) {
        Log.e(LogTag, String.format("shareFile %s %s", title, mimeType));
        Log.e(LogTag, String.format("file path: %s", path));
        File file = new File(path);
        if (file.exists()) {
            Uri uri;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                uri = FileProvider.getUriForFile(MyApplication.getActivity(), BuildConfig.APPLICATION_ID + ".fileprovider", file);
            } else {
                uri = Uri.fromFile(file);
            }
            Intent intent = new Intent(Intent.ACTION_SEND);
            intent.setType(mimeType);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            // 赋予读写权限
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            intent.putExtra(Intent.EXTRA_STREAM, uri);
            MyApplication.getActivity().startActivity(Intent.createChooser(intent, title));
        } else {
            Log.e(LogTag, "File does not exist");
        }
    }

    // 分享多个文件
    public static void shareFiles(String []paths, String mimeType, String title) {
        Log.e(LogTag, String.format("shareFiles %s %s", title, mimeType));
        ArrayList<Uri> uris = new ArrayList<>();
        for (int i = 0; i < paths.length; i++)
        {
            Log.e(LogTag, String.format("file %d path: %s", i, paths[i]));
            File file = new File(paths[i]);
            if (file.exists()) {
                Uri uri;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                    uri = FileProvider.getUriForFile(MyApplication.getActivity(), BuildConfig.APPLICATION_ID + ".fileprovider", file);
                } else {
                    uri = Uri.fromFile(file);
                }
                uris.add(uri);
            }
        }

        if (!uris.isEmpty()) {
            Intent intent = new Intent(Intent.ACTION_SEND_MULTIPLE);
            intent.setType(mimeType);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            // 赋予读写权限
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            intent.putParcelableArrayListExtra(Intent.EXTRA_STREAM, uris);
            MyApplication.getActivity().startActivity(Intent.createChooser(intent, title));
        } else {
            Log.e(LogTag, "File does not exist");
        }
    }

    // 在线程中执行shareFile
    public static void shareFileAsync(String path, String mimeType, String title) {
        new Thread(() -> {
            shareFile(path, mimeType, title);
        }).start();
    }

    // 在线程中执行shareFiles
    public static void shareFilesAsync(String []paths, String mimeType, String title) {
        new Thread(() -> {
            shareFiles(paths, mimeType, title);
        }).start();
    }
}
