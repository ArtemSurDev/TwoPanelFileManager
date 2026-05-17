#include <QCoreApplication>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <iostream>
#include "FileOperationsFacade.h"
#include "TestUtils.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        std::cerr << "Temp dir error" << std::endl;
        return 1;
    }

    QDir base(tempDir.path());
    const QString dirPath = base.filePath("newdir");
    const QString oldFile = QDir(dirPath).filePath("old.txt");
    const QString newFile = QDir(dirPath).filePath("new.txt");

    auto facade = FileOperationsFacade::getInstance();
    if (!facade->createDir(dirPath)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    if (!writeFile(oldFile, "content")) {
        std::cerr << "Write error" << std::endl;
        return 1;
    }

    if (!facade->move(oldFile, newFile)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    if (!facade->remove(dirPath)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    std::cout << "Scenario Create/Rename/Delete: OK" << std::endl;
    return 0;
}

