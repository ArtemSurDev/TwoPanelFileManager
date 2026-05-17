#include <QCoreApplication>
#include <QTemporaryDir>
#include <QDir>
#include <iostream>
#include "FileOperationsFacade.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        std::cerr << "Temp dir error" << std::endl;
        return 1;
    }

    QDir base(tempDir.path());
    const QString missing = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    if (facade->copy(missing, dst)) {
        std::cerr << "Unexpected success" << std::endl;
        return 1;
    }

    if (!facade->getLastError().contains("Cannot copy file")) {
        std::cerr << "Error text mismatch" << std::endl;
        return 1;
    }

    std::cout << "Scenario Error Handling: OK" << std::endl;
    return 0;
}

