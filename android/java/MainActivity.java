// 根据包名在AndroidManifest.xml中指定Activity的name路径（=包名.类名）
package com.gongjianbo.demo;
import com.gongjianbo.demo.*;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.view.WindowManager;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
 
public class MainActivity extends QtActivity {
    private String LogTag = "Main Activity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(LogTag, "onCreate MainActivity start");
        super.onCreate(savedInstanceState);
        Log.d(LogTag, String.format("getApplicationContext() equals MyApplication.getContext() ? %b",
            getApplicationContext().equals(MyApplication.getContext())));
        Log.d(LogTag, String.format("getApplicationContext() equals Activity ? %b",
            getApplicationContext().equals(this)));
        // 设置屏幕常量
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Log.d(LogTag, "onCreate MainActivity finish");
    }
}
