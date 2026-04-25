#include "FileOperations.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

bool FileOperations::copy(const QString& src, const QString& dst) {
    QFileInfo info(src);

    if (info.isDir()) {
        QDir().mkpath(dst);
        QDir sourceDir(src);

        for (const QString& file : sourceDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
            if (!copy(src + "/" + file, dst + "/" + file))
                return false;
        }
        return true;
    } else {
        return QFile::copy(src, dst);
    }
}

bool FileOperations::move(const QString& src, const QString& dst) {
    return QFile::rename(src, dst);
}

bool FileOperations::remove(const QString& path) {
    QFileInfo info(path);

    if (info.isDir()) {
        QDir dir(path);
        return dir.removeRecursively();
    } else {
        return QFile::remove(path);
    }
}

bool FileOperations::createDir(const QString& path) {
    return QDir().mkdir(path);
}