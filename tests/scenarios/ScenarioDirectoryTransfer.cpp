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
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");
    const QString movedDir = base.filePath("movedDir");

    if (!QDir().mkpath(nestedDir)) {
        std::cerr << "mkdir error" << std::endl;
        return 1;
    }

    if (!writeFile(QDir(nestedDir).filePath("file.txt"), "data")) {
        std::cerr << "write error" << std::endl;
        return 1;
    }

    auto facade = FileOperationsFacade::getInstance();
    if (!facade->copy(srcDir, dstDir)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    if (!facade->move(dstDir, movedDir)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    if (!facade->remove(movedDir)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    std::cout << "Scenario Directory Transfer: OK" << std::endl;
    return 0;
}

