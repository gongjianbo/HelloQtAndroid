// 根据包名在AndroidManifest.xml中指定Application的name路径（=包名.类名）
package com.gongjianbo.demo;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.app.Application;
import android.content.Context;
 
public class MyApplication extends QtApplication {
    private static Context mContext;

    @Override
    public void onCreate() {
        super.onCreate();
        // getApplicationContext();
        mContext = this;
    }

    public static Context getContext() {
        return mContext;
    }
}
