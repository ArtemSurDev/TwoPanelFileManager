#pragma once

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>
#include <QString>

inline bool writeFile(const QString& path, const QByteArray& content) {
    QDir dir(QFileInfo(path).absolutePath());
    if (!dir.exists() && !dir.mkpath(".")) {
        return false;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return file.write(content) == content.size();
}

inline QByteArray readFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return file.readAll();
}

