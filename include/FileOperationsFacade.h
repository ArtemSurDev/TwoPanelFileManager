#pragma once
#include <QString>
#include <functional>

class FileOperationsFacade {
private:
    FileOperationsFacade() = default;
    static FileOperationsFacade* instance;
    QString lastError;
    std::function<void()> onFileSystemChanged;

public:
    static FileOperationsFacade* getInstance();
    bool copy(const QString& src, const QString& dst);
    bool move(const QString& src, const QString& dst);
    bool remove(const QString& path);
    bool createDir(const QString& path);
    QString getLastError() const;
    void setOnFileSystemChanged(std::function<void()> callback);
    void notifyChanged();
};