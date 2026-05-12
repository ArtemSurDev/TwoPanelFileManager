#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include "FileOperationsFacade.h"

namespace {
bool writeFile(const QString& path, const QByteArray& content) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return file.write(content) == content.size();
}

QByteArray readFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return file.readAll();
}
}

TEST(FileOperationsFacadeCopy, CopiesFileAndContents) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    ASSERT_TRUE(writeFile(src, "test-data"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->copy(src, dst));
    EXPECT_TRUE(QFile::exists(dst));
    EXPECT_EQ(readFile(dst), QByteArray("test-data"));
}

TEST(FileOperationsFacadeCopy, CreatesDestinationDirectory) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/nested/source.txt");

    ASSERT_TRUE(writeFile(src, "nested"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->copy(src, dst));
    EXPECT_TRUE(QDir(base.filePath("dest/nested")).exists());
}

TEST(FileOperationsFacadeCopy, OverwritesExistingDestinationFile) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    ASSERT_TRUE(writeFile(src, "new-content"));
    ASSERT_TRUE(writeFile(dst, "old-content"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->copy(src, dst));
    EXPECT_EQ(readFile(dst), QByteArray("new-content"));
}

TEST(FileOperationsFacadeCopy, CopiesDirectoryRecursively) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");

    ASSERT_TRUE(QDir().mkpath(nestedDir));
    ASSERT_TRUE(writeFile(QDir(nestedDir).filePath("file.txt"), "nested"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->copy(srcDir, dstDir));
    EXPECT_TRUE(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
}

TEST(FileOperationsFacadeCopy, MissingSourceReturnsFalse) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->copy(src, dst));
}

TEST(FileOperationsFacadeMove, MovesFileAndRemovesSource) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("target/move.txt");

    ASSERT_TRUE(writeFile(src, "move"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->move(src, dst));
    EXPECT_FALSE(QFile::exists(src));
    EXPECT_TRUE(QFile::exists(dst));
}

TEST(FileOperationsFacadeMove, CreatesDestinationDirectory) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("newdir/sub/move.txt");

    ASSERT_TRUE(writeFile(src, "move"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->move(src, dst));
    EXPECT_TRUE(QDir(base.filePath("newdir/sub")).exists());
    EXPECT_TRUE(QFile::exists(dst));
}

TEST(FileOperationsFacadeMove, OverwritesExistingDestinationFile) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("dest/move.txt");

    ASSERT_TRUE(writeFile(src, "new"));
    ASSERT_TRUE(writeFile(dst, "old"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->move(src, dst));
    EXPECT_FALSE(QFile::exists(src));
    EXPECT_EQ(readFile(dst), QByteArray("new"));
}

TEST(FileOperationsFacadeMove, MovesDirectoryRecursively) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");

    ASSERT_TRUE(QDir().mkpath(nestedDir));
    ASSERT_TRUE(writeFile(QDir(nestedDir).filePath("file.txt"), "nested"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->move(srcDir, dstDir));
    EXPECT_FALSE(QDir(srcDir).exists());
    EXPECT_TRUE(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
}

TEST(FileOperationsFacadeMove, MissingSourceReturnsFalse) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->move(src, dst));
}

TEST(FileOperationsFacadeRemove, RemovesFile) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("remove.txt");

    ASSERT_TRUE(writeFile(path, "remove"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->remove(path));
    EXPECT_FALSE(QFile::exists(path));
}

TEST(FileOperationsFacadeRemove, RemovesEmptyDirectory) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("emptydir");

    ASSERT_TRUE(QDir().mkpath(dirPath));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->remove(dirPath));
    EXPECT_FALSE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeRemove, RemovesDirectoryWithContents) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");
    const QString filePath = QDir(dirPath).filePath("file.txt");

    ASSERT_TRUE(QDir().mkpath(dirPath));
    ASSERT_TRUE(writeFile(filePath, "content"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->remove(dirPath));
    EXPECT_FALSE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeRemove, RemovesNestedDirectoryTree) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");
    const QString nestedDir = QDir(dirPath).filePath("nested");
    const QString filePath = QDir(nestedDir).filePath("file.txt");

    ASSERT_TRUE(QDir().mkpath(nestedDir));
    ASSERT_TRUE(writeFile(filePath, "nested"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->remove(dirPath));
    EXPECT_FALSE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeRemove, MissingPathReturnsFalse) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->remove(path));
}

TEST(FileOperationsFacadeCreateDir, CreatesDirectory) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("newdir");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->createDir(dirPath));
    EXPECT_TRUE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeCreateDir, CreatesNestedDirectories) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("a/b/c");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->createDir(dirPath));
    EXPECT_TRUE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeCreateDir, ReturnsTrueWhenExists) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("exists");

    ASSERT_TRUE(QDir().mkpath(dirPath));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->createDir(dirPath));
    EXPECT_TRUE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeCreateDir, FailsWhenPathIsFile) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("file.txt");

    ASSERT_TRUE(writeFile(path, "file"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->createDir(path));
}

TEST(FileOperationsFacadeCreateDir, SupportsSpacesInPath) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir with spaces");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_TRUE(facade->createDir(dirPath));
    EXPECT_TRUE(QDir(dirPath).exists());
}

TEST(FileOperationsFacadeGetLastError, CopyFailureContainsCannotCopy) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->copy(src, dst));
    EXPECT_TRUE(facade->getLastError().contains("Cannot copy file"));
}

TEST(FileOperationsFacadeGetLastError, MoveFailureContainsCannotMove) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->move(src, dst));
    EXPECT_TRUE(facade->getLastError().contains("Cannot move"));
}

TEST(FileOperationsFacadeGetLastError, RemoveFailureContainsCannotRemove) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->remove(path));
    EXPECT_TRUE(facade->getLastError().contains("Cannot remove"));
}

TEST(FileOperationsFacadeGetLastError, CreateDirFailureContainsCannotCreate) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("file.txt");

    ASSERT_TRUE(writeFile(path, "file"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->createDir(path));
    EXPECT_TRUE(facade->getLastError().contains("Cannot create directory"));
}

TEST(FileOperationsFacadeGetLastError, MoveFailureContainsCannotCreateDestinationDirectory) {
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString blockingFile = base.filePath("blocked");
    const QString dst = QDir(blockingFile).filePath("target.txt");

    ASSERT_TRUE(writeFile(src, "content"));
    ASSERT_TRUE(writeFile(blockingFile, "block"));
    auto facade = FileOperationsFacade::getInstance();
    EXPECT_FALSE(facade->move(src, dst));
    EXPECT_TRUE(facade->getLastError().contains("Cannot create destination directory"));
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
