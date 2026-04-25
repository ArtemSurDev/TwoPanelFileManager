#include "FileOperationsFacade.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

FileOperationsFacade* FileOperationsFacade::instance = nullptr;

FileOperationsFacade* FileOperationsFacade::getInstance() {
    if (!instance) {
        instance = new FileOperationsFacade();
    }
    return instance;
}

void FileOperationsFacade::setOnFileSystemChanged(std::function<void()> callback) {
    onFileSystemChanged = callback;
}

void FileOperationsFacade::notifyChanged() {
    if (onFileSystemChanged) {
        onFileSystemChanged();
    }
}

QString FileOperationsFacade::getLastError() const {
    return lastError;
}

bool FileOperationsFacade::copy(const QString& src, const QString& dst) {
    QFileInfo info(src);
    if (info.isDir()) {
        QDir dstDir(dst);
        if (!dstDir.exists()) {
            if (!dstDir.mkpath(".")) {
                lastError = "Cannot create destination directory: " + dst;
                return false;
            }
        }
        QDir sourceDir(src);
        for (const QString& file : sourceDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
            if (!copy(src + "/" + file, dst + "/" + file)) {
                return false;
            }
        }
        notifyChanged();
        return true;
    } else {
        QDir dstDir(QFileInfo(dst).absolutePath());
        if (!dstDir.exists()) {
            if (!dstDir.mkpath(".")) {
                lastError = "Cannot create destination directory: " + dstDir.absolutePath();
                return false;
            }
        }
        if (QFile::exists(dst)) {
            QFile::remove(dst);
        }
        bool result = QFile::copy(src, dst);
        if (result) {
            notifyChanged();
        } else {
            lastError = "Cannot copy file: " + src + " to " + dst;
        }
        return result;
    }
}

bool FileOperationsFacade::move(const QString& src, const QString& dst) {
    QDir dstDir(QFileInfo(dst).absolutePath());
    if (!dstDir.exists()) {
        if (!dstDir.mkpath(".")) {
            lastError = "Cannot create destination directory: " + dstDir.absolutePath();
            return false;
        }
    }
    if (QFile::exists(dst)) {
        QFile::remove(dst);
    }
    bool result = QFile::rename(src, dst);
    if (result) {
        notifyChanged();
    } else {
        lastError = "Cannot move: " + src + " to " + dst;
    }
    return result;
}

bool FileOperationsFacade::remove(const QString& path) {
    QFileInfo info(path);
    bool result = info.isDir() ? QDir(path).removeRecursively() : QFile::remove(path);
    if (result) {
        notifyChanged();
    } else {
        lastError = "Cannot remove: " + path;
    }
    return result;
}

bool FileOperationsFacade::createDir(const QString& path) {
    QDir dir;
    bool result = dir.mkpath(path);
    if (result) {
        notifyChanged();
    } else {
        lastError = "Cannot create directory: " + path;
    }
    return result;
}