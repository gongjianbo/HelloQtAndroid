#pragma once
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "VideoItem.h"

namespace GongJianBo {

namespace Declarative {

inline void registerType(QQmlApplicationEngine *)
{
    // QQmlContext *context = engine->rootContext();
    qRegisterMetaType<QImage>("QImage");
    qmlRegisterType<VideoItem>("GongJianBo.Declarative", 1, 0, "VideoItem");
}

}

}
