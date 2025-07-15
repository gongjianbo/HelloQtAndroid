#include "AndroidTool.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <QFileInfo>
#include <QFile>
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

QString AndroidTool::androidId()
{
#if 0
    QAndroidJniObject resolver = QtAndroid::androidActivity().callObjectMethod(
        "getContentResolver",
        "()Landroid/content/ContentResolver;");
    QAndroidJniObject tag = QAndroidJniObject::fromString("android_id");
    QAndroidJniObject id = QAndroidJniObject::callStaticObjectMethod(
        "android/provider/Settings$Secure", "getString",
        "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;",
        resolver.object<jobject>(),
        tag.object<jstring>());
    qDebug() << id.toString();
#endif
    QAndroidJniObject j_id = QAndroidJniObject::callStaticObjectMethod(
        "com/gongjianbo/demo/AndroidTool",
        "androidId",
        "()Ljava/lang/String;"
        );
    return j_id.toString();
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

QString AndroidTool::getFileNameFromUri(const QUrl &url)
{
    // 安卓Dialog获取的url不能用toLocalFile转换成本地路径
    // 要保留url编码格式，不然java中没法正常转换成uri，比如带空格中文等情况
    // 如果是cpp对话框拿到的QString path，也可以QUrl::fromEncoded(path.toUtf8()).toString(QUrl::FullyEncoded)
    QAndroidJniObject j_url = QAndroidJniObject::fromString(url.toString(QUrl::FullyEncoded));
    QAndroidJniObject j_name = QAndroidJniObject::callStaticObjectMethod(
        "com/gongjianbo/demo/AndroidTool",
        "getFileNameFromUri",
        "(Ljava/lang/String;)Ljava/lang/String;",
        j_url.object()
        );
    return j_name.toString();
}
