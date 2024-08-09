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
    Q_PROPERTY(int rotation READ getRotation WRITE setRotation NOTIFY rotationChanged)
private:
    ActivityManager();
public:
    static ActivityManager *getInstance();
    ~ActivityManager();
    // 动态注册 JNI 接口
    void initJNI();

    // Activity 的显示方向
    int getRotation() const;
    void setRotation(int rotation);

    // 开启监听方向变化
    Q_INVOKABLE void enableRotationListen();
    // 结束监听方向变化
    Q_INVOKABLE void disableRotationListen();

signals:
    void rotationChanged();
    void touchEventCanceled();

private:
    // Activity 的显示方向
    int mRotation{0};
};
