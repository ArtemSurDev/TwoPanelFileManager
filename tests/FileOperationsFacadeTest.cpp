#include <catch2/catch_test_macros.hpp>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include "FileOperationsFacade.h"
#include "TestUtils.h"

TEST_CASE("FileOperationsFacade copy copies file and contents") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    REQUIRE(writeFile(src, "test-data"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->copy(src, dst));
    CHECK(QFile::exists(dst));
    CHECK(readFile(dst) == QByteArray("test-data"));
}

TEST_CASE("FileOperationsFacade copy creates destination directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/nested/source.txt");

    REQUIRE(writeFile(src, "nested"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->copy(src, dst));
    CHECK(QDir(base.filePath("dest/nested")).exists());
}

TEST_CASE("FileOperationsFacade copy overwrites existing destination file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    REQUIRE(writeFile(src, "new-content"));
    REQUIRE(QDir().mkpath(base.filePath("dest")));
    REQUIRE(writeFile(dst, "old-content"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->copy(src, dst));
    CHECK(readFile(dst) == QByteArray("new-content"));
}

TEST_CASE("FileOperationsFacade copy copies directory recursively") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");

    REQUIRE(QDir().mkpath(nestedDir));
    REQUIRE(writeFile(QDir(nestedDir).filePath("file.txt"), "nested"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->copy(srcDir, dstDir));
    CHECK(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
}

TEST_CASE("FileOperationsFacade copy missing source returns false") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->copy(src, dst));
}

TEST_CASE("FileOperationsFacade move moves file and removes source") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("target/move.txt");

    REQUIRE(writeFile(src, "move"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->move(src, dst));
    CHECK_FALSE(QFile::exists(src));
    CHECK(QFile::exists(dst));
}

TEST_CASE("FileOperationsFacade move creates destination directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("newdir/sub/move.txt");

    REQUIRE(writeFile(src, "move"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->move(src, dst));
    CHECK(QDir(base.filePath("newdir/sub")).exists());
    CHECK(QFile::exists(dst));
}

TEST_CASE("FileOperationsFacade move overwrites existing destination file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("dest/move.txt");

    REQUIRE(writeFile(src, "new"));
    REQUIRE(QDir().mkpath(base.filePath("dest")));
    REQUIRE(writeFile(dst, "old"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->move(src, dst));
    CHECK_FALSE(QFile::exists(src));
    CHECK(readFile(dst) == QByteArray("new"));
}

TEST_CASE("FileOperationsFacade move moves directory recursively") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");

    REQUIRE(QDir().mkpath(nestedDir));
    REQUIRE(writeFile(QDir(nestedDir).filePath("file.txt"), "nested"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->move(srcDir, dstDir));
    CHECK_FALSE(QDir(srcDir).exists());
    CHECK(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
}

TEST_CASE("FileOperationsFacade move missing source returns false") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->move(src, dst));
}

TEST_CASE("FileOperationsFacade remove removes file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("remove.txt");

    REQUIRE(writeFile(path, "remove"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->remove(path));
    CHECK_FALSE(QFile::exists(path));
}

TEST_CASE("FileOperationsFacade remove removes empty directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("emptydir");

    REQUIRE(QDir().mkpath(dirPath));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->remove(dirPath));
    CHECK_FALSE(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade remove removes directory with contents") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");
    const QString filePath = QDir(dirPath).filePath("file.txt");

    REQUIRE(QDir().mkpath(dirPath));
    REQUIRE(writeFile(filePath, "content"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->remove(dirPath));
    CHECK_FALSE(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade remove removes nested directory tree") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");
    const QString nestedDir = QDir(dirPath).filePath("nested");
    const QString filePath = QDir(nestedDir).filePath("file.txt");

    REQUIRE(QDir().mkpath(nestedDir));
    REQUIRE(writeFile(filePath, "nested"));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->remove(dirPath));
    CHECK_FALSE(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade remove missing path returns false") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->remove(path));
}

TEST_CASE("FileOperationsFacade createDir creates directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("newdir");

    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->createDir(dirPath));
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade createDir creates nested directories") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("a/b/c");

    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->createDir(dirPath));
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade createDir returns true when exists") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("exists");

    REQUIRE(QDir().mkpath(dirPath));
    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->createDir(dirPath));
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade createDir fails when path is file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("file.txt");

    REQUIRE(writeFile(path, "file"));
    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->createDir(path));
}

TEST_CASE("FileOperationsFacade createDir supports spaces in path") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir with spaces");

    auto facade = FileOperationsFacade::getInstance();
    REQUIRE(facade->createDir(dirPath));
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("FileOperationsFacade getLastError copy failure contains message") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->copy(src, dst));
    CHECK(facade->getLastError().contains("Cannot copy file"));
}

TEST_CASE("FileOperationsFacade getLastError move failure contains message") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("dest/missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->move(src, dst));
    CHECK(facade->getLastError().contains("Cannot move"));
}

TEST_CASE("FileOperationsFacade getLastError remove failure contains message") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("missing.txt");

    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->remove(path));
    CHECK(facade->getLastError().contains("Cannot remove"));
}

TEST_CASE("FileOperationsFacade getLastError createDir failure contains message") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("file.txt");

    REQUIRE(writeFile(path, "file"));
    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->createDir(path));
    CHECK(facade->getLastError().contains("Cannot create directory"));
}

TEST_CASE("FileOperationsFacade getLastError move failure cannot create destination directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString blockingFile = base.filePath("blocked");
    const QString dst = QDir(blockingFile).filePath("target.txt");

    REQUIRE(writeFile(src, "content"));
    REQUIRE(writeFile(blockingFile, "block"));
    auto facade = FileOperationsFacade::getInstance();
    CHECK_FALSE(facade->move(src, dst));
    CHECK(facade->getLastError().contains("Cannot create destination directory"));
}
