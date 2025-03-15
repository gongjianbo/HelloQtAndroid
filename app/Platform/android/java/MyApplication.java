// 根据包名在AndroidManifest.xml中指定Application的name路径（=包名.类名）
package com.gongjianbo.demo;
import org.qtproject.qt5.android.bindings.QtApplication;

import android.app.Application;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
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
        startTracking(this);
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

    // 用于监听所有Activity的生命周期回调
    // 而Qt程序只在一个Activity中渲染，相当于监听整个应用的状态
    // 程序进入后台后被冻结，QtCpp代码中的逻辑也没法执行，就可以在回调中做一些操作
    private static final String LifecycleTag = "[Lifecycle]";
    private Application.ActivityLifecycleCallbacks lifecycleCallbacks;
    // 注册回调
    public void startTracking(Context context) {
        Log.d(LifecycleTag, "startTracking");
        Application application = (Application) context.getApplicationContext();
        lifecycleCallbacks = new MyActivityLifecycleCallbacks();
        application.registerActivityLifecycleCallbacks(lifecycleCallbacks);
    }
    // 注销回调
    public void stopTracking(Context context) {
        Log.d(LifecycleTag, "stopTracking");
        Application application = (Application) context.getApplicationContext();
        if (lifecycleCallbacks != null) {
            application.unregisterActivityLifecycleCallbacks(lifecycleCallbacks);
        }
    }
    // 切换到后台：onPause->onStop->onSaveInstanceState
    // 从后台切回来：onStart->onResume
    // 弹授权提示框也会触发onPause和onResume，那就把onStop作为切换到后台，onStart作为从后台恢复
    private class MyActivityLifecycleCallbacks implements Application.ActivityLifecycleCallbacks {
        @Override
        public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
            Log.d(LifecycleTag, "Activity Created: " + activity.getLocalClassName());
        }
        @Override
        public void onActivityStarted(Activity activity) {
            Log.d(LifecycleTag, "Activity Started: " + activity.getLocalClassName());
        }
        @Override
        public void onActivityResumed(Activity activity) {
            Log.d(LifecycleTag, "Activity Resumed: " + activity.getLocalClassName());
        }
        @Override
        public void onActivityPaused(Activity activity) {
            Log.d(LifecycleTag, "Activity Paused: " + activity.getLocalClassName());
        }
        @Override
        public void onActivityStopped(Activity activity) {
            Log.d(LifecycleTag, "Activity Stopped: " + activity.getLocalClassName());
        }
        @Override
        public void onActivitySaveInstanceState(Activity activity, Bundle outState) {
            Log.d(LifecycleTag, "Activity SaveInstanceState: " + activity.getLocalClassName());
        }
        @Override
        public void onActivityDestroyed(Activity activity) {
            Log.d(LifecycleTag, "Activity Destroyed: " + activity.getLocalClassName());
        }
    }
}
