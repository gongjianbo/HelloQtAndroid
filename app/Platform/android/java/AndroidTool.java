package com.gongjianbo.demo;

import android.app.Application;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

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
}
