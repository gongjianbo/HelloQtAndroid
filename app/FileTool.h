#pragma once
#include <QObject>

class FileTool : public QObject
{
    Q_OBJECT
public:
    explicit FileTool(QObject *parent = nullptr);

    Q_INVOKABLE int writeTest();
    Q_INVOKABLE int readTest();

};
