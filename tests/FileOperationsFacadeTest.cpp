#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include "FileOperationsFacade.h"
#include "FileCommand.h"

class FileOperationsFacadeTest : public QObject {
    Q_OBJECT

private:
    static bool writeFile(const QString& path, const QByteArray& content) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return false;
        }
        return file.write(content) == content.size();
    }

    static QByteArray readFile(const QString& path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            return {};
        }
        return file.readAll();
    }

private slots:
    void copyFileCreatesDestinationAndCopiesContent() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("source.txt");
        const QString dstDir = QDir(tempDir.path()).filePath("dest");
        const QString dst = QDir(dstDir).filePath("source.txt");

        QVERIFY(writeFile(src, "test-data"));
        auto facade = FileOperationsFacade::getInstance();
        QVERIFY(facade->copy(src, dst));
        QVERIFY(QFile::exists(dst));
        QCOMPARE(readFile(dst), QByteArray("test-data"));
    }

    void copyDirectoryRecursively() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString srcDir = QDir(tempDir.path()).filePath("srcDir");
        const QString nestedDir = QDir(srcDir).filePath("nested");
        const QString dstDir = QDir(tempDir.path()).filePath("dstDir");
        QVERIFY(QDir().mkpath(nestedDir));
        QVERIFY(writeFile(QDir(nestedDir).filePath("file.txt"), "nested"));

        auto facade = FileOperationsFacade::getInstance();
        QVERIFY(facade->copy(srcDir, dstDir));
        QVERIFY(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
    }

    void moveFileMovesAndRemovesSource() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("move.txt");
        const QString dst = QDir(tempDir.path()).filePath("target/move.txt");
        QVERIFY(writeFile(src, "move"));

        auto facade = FileOperationsFacade::getInstance();
        QVERIFY(facade->move(src, dst));
        QVERIFY(!QFile::exists(src));
        QVERIFY(QFile::exists(dst));
    }

    void createAndRemoveDirectory() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString dirPath = QDir(tempDir.path()).filePath("newdir");

        auto facade = FileOperationsFacade::getInstance();
        QVERIFY(facade->createDir(dirPath));
        QVERIFY(QDir(dirPath).exists());
        QVERIFY(facade->remove(dirPath));
        QVERIFY(!QDir(dirPath).exists());
    }

    void removeDirectoryRecursively() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString dirPath = QDir(tempDir.path()).filePath("dir");
        const QString filePath = QDir(dirPath).filePath("file.txt");
        QVERIFY(QDir().mkpath(dirPath));
        QVERIFY(writeFile(filePath, "content"));

        auto facade = FileOperationsFacade::getInstance();
        QVERIFY(facade->remove(dirPath));
        QVERIFY(!QDir(dirPath).exists());
    }

    void copyMissingFileSetsError() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("missing.txt");
        const QString dst = QDir(tempDir.path()).filePath("dest/missing.txt");

        auto facade = FileOperationsFacade::getInstance();
        QVERIFY(!facade->copy(src, dst));
        QVERIFY(!facade->getLastError().isEmpty());
    }
};

class FileCommandTest : public QObject {
    Q_OBJECT

private:
    static bool writeFile(const QString& path, const QByteArray& content) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return false;
        }
        return file.write(content) == content.size();
    }

private slots:
    void copyCommandExecuteAndUndo() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("copy.txt");
        const QString dst = QDir(tempDir.path()).filePath("dest/copy.txt");
        QVERIFY(writeFile(src, "copy"));

        CopyCommand command(src, dst);
        QVERIFY(command.execute());
        QVERIFY(QFile::exists(dst));
        QVERIFY(command.undo());
        QVERIFY(!QFile::exists(dst));
    }

    void moveCommandExecuteAndUndo() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("move.txt");
        const QString dst = QDir(tempDir.path()).filePath("dest/move.txt");
        QVERIFY(writeFile(src, "move"));

        MoveCommand command(src, dst);
        QVERIFY(command.execute());
        QVERIFY(!QFile::exists(src));
        QVERIFY(QFile::exists(dst));
        QVERIFY(command.undo());
        QVERIFY(QFile::exists(src));
        QVERIFY(!QFile::exists(dst));
    }

    void createDirCommandExecuteAndUndo() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString dirPath = QDir(tempDir.path()).filePath("dir");

        CreateDirCommand command(dirPath);
        QVERIFY(command.execute());
        QVERIFY(QDir(dirPath).exists());
        QVERIFY(command.undo());
        QVERIFY(!QDir(dirPath).exists());
    }

    void renameCommandExecuteAndUndo() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("old.txt");
        const QString dst = QDir(tempDir.path()).filePath("new.txt");
        QVERIFY(writeFile(src, "rename"));

        RenameCommand command(src, dst);
        QVERIFY(command.execute());
        QVERIFY(!QFile::exists(src));
        QVERIFY(QFile::exists(dst));
        QVERIFY(command.undo());
        QVERIFY(QFile::exists(src));
        QVERIFY(!QFile::exists(dst));
    }

    void deleteCommandExecuteAndUndo() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        const QString src = QDir(tempDir.path()).filePath("delete.txt");
        QVERIFY(writeFile(src, "delete"));

        DeleteCommand command(src);
        QVERIFY(command.execute());
        QVERIFY(!QFile::exists(src));
        QVERIFY(!command.undo());
    }
};

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    int status = 0;
    FileOperationsFacadeTest facadeTest;
    FileCommandTest commandTest;
    status |= QTest::qExec(&facadeTest, argc, argv);
    status |= QTest::qExec(&commandTest, argc, argv);
    return status;
}

#include "FileOperationsFacadeTest.moc"
