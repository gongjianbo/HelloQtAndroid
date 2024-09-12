#include "AndroidTool.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QDebug>

AndroidTool::AndroidTool()
{
    qDebug() << __FUNCTION__;
}

AndroidTool *AndroidTool::getInstance()
{
    static AndroidTool instance;
    return &instance;
}

AndroidTool::~AndroidTool()
{
    qDebug() << __FUNCTION__;
}

void AndroidTool::restartApp()
{
    QAndroidJniObject::callStaticMethod<void>(
        "com/gongjianbo/demo/AndroidTool",
        "restartApp",
        "()V");
}

void AndroidTool::gotoHome()
{
    QAndroidJniObject::callStaticMethod<void>(
        "com/gongjianbo/demo/AndroidTool",
        "gotoHome",
        "()V");
}

void AndroidTool::shareFile(const QString &path, const QString &mimeType, const QString &title)
{
    if (path.isEmpty())
        return;
    QAndroidJniObject j_path = QAndroidJniObject::fromString(path);
    QAndroidJniObject j_mime = QAndroidJniObject::fromString(mimeType);
    QAndroidJniObject j_title = QAndroidJniObject::fromString(title);
    QAndroidJniObject::callStaticMethod<void>(
        "com/gongjianbo/demo/AndroidTool",
        "shareFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
        j_path.object<jstring>(),
        j_mime.object<jstring>(),
        j_title.object<jstring>());
}

void AndroidTool::shareFiles(const QStringList &paths, const QString &mimeType, const QString &title)
{
    if (paths.isEmpty())
        return;
    // 没有封装字符串数组，用jni接口创建一个，用完要释放
    QAndroidJniEnvironment env;
    jobjectArray j_arr = env->NewObjectArray(paths.size(), env->FindClass("java/lang/String"), nullptr);
    for (int i = 0; i < paths.size(); i++) {
        QByteArray path = paths.at(i).toUtf8();
        jstring j_path = env->NewStringUTF(path.data());
        env->SetObjectArrayElement(j_arr, i, j_path);
        env->DeleteLocalRef(j_path);
    }
    QAndroidJniObject j_mime = QAndroidJniObject::fromString(mimeType);
    QAndroidJniObject j_title = QAndroidJniObject::fromString(title);
    QAndroidJniObject::callStaticMethod<void>(
        "com/gongjianbo/demo/AndroidTool",
        "shareFiles",
        "([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
        j_arr,
        j_mime.object<jstring>(),
        j_title.object<jstring>());
    // 用完释放
    env->DeleteLocalRef(j_arr);
}
