// 根据包名在AndroidManifest.xml中指定Activity的name路径（=包名.类名）
package com.gongjianbo.demo;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.view.WindowManager;
import android.os.Bundle;
import android.util.Log;
 
public class MainActivity extends QtActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.e("MainActivity", "onCreate MainActivity start");
        super.onCreate(savedInstanceState);
        // 设置屏幕常量
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Log.e("MainActivity", "onCreate MainActivity finish");
    }
}
