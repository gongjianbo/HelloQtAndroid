#pragma once
#include <QObject>
#include "jni.h"

// 静态注册 JNI 接口
// #ifdef __cplusplus
// extern "C" {
// #endif
// // 触摸释放
// JNIEXPORT void JNICALL
// Java_com_gongjianbo_demo_MainActivity_jniTouchEventCanceled(JNIEnv * /*env*/, jobject /*thiz*/);
// #ifdef __cplusplus
// }
// #endif

// Android Activity 的一些操作
class ActivityManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ActivityManager)
private:
    ActivityManager();
public:
    static ActivityManager *getInstance();
    ~ActivityManager();
    // 动态注册 JNI 接口
    void initJNI();

signals:
    void touchEventCanceled();

};
