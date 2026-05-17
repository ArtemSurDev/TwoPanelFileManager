#include <catch2/catch_test_macros.hpp>
#include <QTemporaryDir>
#include <QDir>
#include <QFileInfo>
#include "FileCommand.h"
#include "TestUtils.h"

TEST_CASE("CopyCommand executes on file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    REQUIRE(writeFile(src, "data"));
    CopyCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK(QFile::exists(dst));
    CHECK(readFile(dst) == QByteArray("data"));
}

TEST_CASE("CopyCommand creates destination directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("nested/dir/source.txt");

    REQUIRE(writeFile(src, "data"));
    CopyCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK(QDir(base.filePath("nested/dir")).exists());
}

TEST_CASE("CopyCommand overwrites destination") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    REQUIRE(writeFile(src, "new"));
    REQUIRE(writeFile(dst, "old"));
    CopyCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK(readFile(dst) == QByteArray("new"));
}

TEST_CASE("CopyCommand copies directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");

    REQUIRE(QDir().mkpath(nestedDir));
    REQUIRE(writeFile(QDir(nestedDir).filePath("file.txt"), "content"));
    CopyCommand command(srcDir, dstDir);
    REQUIRE(command.execute());
    CHECK(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
}

TEST_CASE("CopyCommand undo removes copied target") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("source.txt");
    const QString dst = base.filePath("dest/source.txt");

    REQUIRE(writeFile(src, "data"));
    CopyCommand command(src, dst);
    REQUIRE(command.execute());
    REQUIRE(command.undo());
    CHECK_FALSE(QFile::exists(dst));
}

TEST_CASE("MoveCommand executes on file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("target/move.txt");

    REQUIRE(writeFile(src, "move"));
    MoveCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK_FALSE(QFile::exists(src));
    CHECK(QFile::exists(dst));
}

TEST_CASE("MoveCommand creates destination directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("newdir/sub/move.txt");

    REQUIRE(writeFile(src, "move"));
    MoveCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK(QDir(base.filePath("newdir/sub")).exists());
    CHECK(QFile::exists(dst));
}

TEST_CASE("MoveCommand overwrites destination") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("dest/move.txt");

    REQUIRE(writeFile(src, "new"));
    REQUIRE(writeFile(dst, "old"));
    MoveCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK_FALSE(QFile::exists(src));
    CHECK(readFile(dst) == QByteArray("new"));
}

TEST_CASE("MoveCommand moves directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("srcDir");
    const QString nestedDir = QDir(srcDir).filePath("nested");
    const QString dstDir = base.filePath("dstDir");

    REQUIRE(QDir().mkpath(nestedDir));
    REQUIRE(writeFile(QDir(nestedDir).filePath("file.txt"), "content"));
    MoveCommand command(srcDir, dstDir);
    REQUIRE(command.execute());
    CHECK_FALSE(QDir(srcDir).exists());
    CHECK(QFile::exists(QDir(dstDir).filePath("nested/file.txt")));
}

TEST_CASE("MoveCommand undo restores file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("move.txt");
    const QString dst = base.filePath("dest/move.txt");

    REQUIRE(writeFile(src, "move"));
    MoveCommand command(src, dst);
    REQUIRE(command.execute());
    REQUIRE(command.undo());
    CHECK(QFile::exists(src));
    CHECK_FALSE(QFile::exists(dst));
}

TEST_CASE("DeleteCommand removes file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("remove.txt");

    REQUIRE(writeFile(path, "remove"));
    DeleteCommand command(path);
    REQUIRE(command.execute());
    CHECK_FALSE(QFile::exists(path));
}

TEST_CASE("DeleteCommand removes directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");

    REQUIRE(QDir().mkpath(dirPath));
    DeleteCommand command(dirPath);
    REQUIRE(command.execute());
    CHECK_FALSE(QDir(dirPath).exists());
}

TEST_CASE("DeleteCommand removes nested tree") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");
    const QString nestedDir = QDir(dirPath).filePath("nested");

    REQUIRE(QDir().mkpath(nestedDir));
    REQUIRE(writeFile(QDir(nestedDir).filePath("file.txt"), "content"));
    DeleteCommand command(dirPath);
    REQUIRE(command.execute());
    CHECK_FALSE(QDir(dirPath).exists());
}

TEST_CASE("DeleteCommand missing path returns false") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("missing.txt");

    DeleteCommand command(path);
    CHECK_FALSE(command.execute());
}

TEST_CASE("DeleteCommand undo is not supported") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("remove.txt");

    REQUIRE(writeFile(path, "remove"));
    DeleteCommand command(path);
    REQUIRE(command.execute());
    CHECK_FALSE(command.undo());
    CHECK_FALSE(QFile::exists(path));
}

TEST_CASE("CreateDirCommand creates directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("newdir");

    CreateDirCommand command(dirPath);
    REQUIRE(command.execute());
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("CreateDirCommand creates nested directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("a/b/c");

    CreateDirCommand command(dirPath);
    REQUIRE(command.execute());
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("CreateDirCommand returns true on existing directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("exists");

    REQUIRE(QDir().mkpath(dirPath));
    CreateDirCommand command(dirPath);
    REQUIRE(command.execute());
    CHECK(QDir(dirPath).exists());
}

TEST_CASE("CreateDirCommand fails when path is file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString path = base.filePath("file.txt");

    REQUIRE(writeFile(path, "file"));
    CreateDirCommand command(path);
    CHECK_FALSE(command.execute());
}

TEST_CASE("CreateDirCommand undo removes created directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("undo_dir");

    CreateDirCommand command(dirPath);
    REQUIRE(command.execute());
    REQUIRE(command.undo());
    CHECK_FALSE(QDir(dirPath).exists());
}

TEST_CASE("RenameCommand renames file") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("old.txt");
    const QString dst = base.filePath("new.txt");

    REQUIRE(writeFile(src, "content"));
    RenameCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK_FALSE(QFile::exists(src));
    CHECK(QFile::exists(dst));
}

TEST_CASE("RenameCommand renames directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString srcDir = base.filePath("oldDir");
    const QString dstDir = base.filePath("newDir");

    REQUIRE(QDir().mkpath(srcDir));
    RenameCommand command(srcDir, dstDir);
    REQUIRE(command.execute());
    CHECK_FALSE(QDir(srcDir).exists());
    CHECK(QDir(dstDir).exists());
}

TEST_CASE("RenameCommand fails for missing source") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("missing.txt");
    const QString dst = base.filePath("new.txt");

    RenameCommand command(src, dst);
    CHECK_FALSE(command.execute());
}

TEST_CASE("RenameCommand undo restores original name") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("old.txt");
    const QString dst = base.filePath("new.txt");

    REQUIRE(writeFile(src, "content"));
    RenameCommand command(src, dst);
    REQUIRE(command.execute());
    REQUIRE(command.undo());
    CHECK(QFile::exists(src));
    CHECK_FALSE(QFile::exists(dst));
}

TEST_CASE("RenameCommand creates destination directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString src = base.filePath("old.txt");
    const QString dst = base.filePath("nested/new.txt");

    REQUIRE(writeFile(src, "content"));
    RenameCommand command(src, dst);
    REQUIRE(command.execute());
    CHECK(QDir(base.filePath("nested")).exists());
    CHECK(QFile::exists(dst));
}

