// 根据包名在AndroidManifest.xml中指定Application的name路径（=包名.类名）
package com.gongjianbo.demo;
import org.qtproject.qt5.android.bindings.QtApplication;

import android.app.Application;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
 
public class MyApplication extends QtApplication {
    private static final String LogTag = "[MyApplication]";
    private static Context mContext;
    private static Activity mActivity;

    @Override
    public void onCreate() {
        super.onCreate();
        // getApplicationContext();
        mContext = this;
    }

    public static Context getContext() {
        return mContext;
    }

    public static Activity getActivity() {
        return mActivity;
    }

    public static void setActivity(Activity activity) {
        mActivity = activity;
    }
}
