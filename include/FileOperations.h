#pragma once
#include <QString>

class FileOperations {
public:
    static bool copy(const QString& src, const QString& dst);
    static bool move(const QString& src, const QString& dst);
    static bool remove(const QString& path);
    static bool createDir(const QString& path);
};