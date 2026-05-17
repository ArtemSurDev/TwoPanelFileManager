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
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");
    const QString moved = base.filePath("moved/source.txt");

    if (!writeFile(src, "scenario")) {
        std::cerr << "Write error" << std::endl;
        return 1;
    }

    auto facade = FileOperationsFacade::getInstance();
    if (!facade->copy(src, dst)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    if (!facade->move(dst, moved)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    if (!facade->remove(moved)) {
        std::cerr << facade->getLastError().toStdString() << std::endl;
        return 1;
    }

    std::cout << "Scenario Copy/Move/Delete: OK" << std::endl;
    return 0;
}

