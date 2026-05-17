#include <catch2/catch_test_macros.hpp>
#include <QTemporaryDir>
#include <QSignalSpy>
#include <QKeyEvent>
#include <QDir>
#include "FilePanel.h"
#include "TestUtils.h"

TEST_CASE("FilePanel setPath updates current path") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    panel.setPath(tempDir.path());
    CHECK(panel.getCurrentPath() == tempDir.path());
}

TEST_CASE("FilePanel setPath updates to root") {
    FilePanel panel;

    panel.setPath("/");
    CHECK(panel.getCurrentPath() == "/");
}

TEST_CASE("FilePanel setPath updates items when path changes") {
    QTemporaryDir tempA;
    QTemporaryDir tempB;
    REQUIRE(tempA.isValid());
    REQUIRE(tempB.isValid());

    FilePanel panel;
    REQUIRE(writeFile(QDir(tempA.path()).filePath("a.txt"), "a"));
    REQUIRE(writeFile(QDir(tempB.path()).filePath("b.txt"), "b"));

    panel.setPath(tempA.path());
    CHECK_FALSE(panel.findItems("a.txt", Qt::MatchExactly).isEmpty());

    panel.setPath(tempB.path());
    CHECK_FALSE(panel.findItems("b.txt", Qt::MatchExactly).isEmpty());
    CHECK(panel.findItems("a.txt", Qt::MatchExactly).isEmpty());
}

TEST_CASE("FilePanel setPath on missing path keeps parent entry only") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());

    FilePanel panel;
    panel.setPath(QDir(tempDir.path()).filePath("missing"));
    REQUIRE(panel.count() == 1);
    CHECK(panel.item(0)->text() == "..");
}

TEST_CASE("FilePanel setPath keeps current path on repeated call") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    panel.setPath(tempDir.path());
    panel.setPath(tempDir.path());
    CHECK(panel.getCurrentPath() == tempDir.path());
}

TEST_CASE("FilePanel refresh adds parent entry when not root") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    panel.setPath(tempDir.path());
    REQUIRE(panel.count() > 0);
    CHECK(panel.item(0)->text() == "..");
}

TEST_CASE("FilePanel refresh omits parent entry at root") {
    FilePanel panel;
    panel.setPath("/");
    if (panel.count() > 0) {
        CHECK(panel.item(0)->text() != "..");
    } else {
        CHECK(panel.count() == 0);
    }
}

TEST_CASE("FilePanel refresh selects first item") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    const QString filePath = QDir(tempDir.path()).filePath("file.txt");
    REQUIRE(writeFile(filePath, "data"));
    panel.setPath(tempDir.path());
    CHECK(panel.currentRow() == 0);
}

TEST_CASE("FilePanel refresh lists new file after creation") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    panel.setPath(tempDir.path());
    REQUIRE(writeFile(QDir(tempDir.path()).filePath("file.txt"), "data"));
    panel.refresh();
    CHECK_FALSE(panel.findItems("file.txt", Qt::MatchExactly).isEmpty());
}

TEST_CASE("FilePanel refresh lists directories") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    const QString dirPath = QDir(tempDir.path()).filePath("subdir");
    REQUIRE(QDir().mkpath(dirPath));
    panel.setPath(tempDir.path());
    panel.refresh();
    CHECK_FALSE(panel.findItems("subdir", Qt::MatchExactly).isEmpty());
}

TEST_CASE("FilePanel getSelectedItem returns current item text") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    const QString filePath = QDir(tempDir.path()).filePath("file.txt");
    REQUIRE(writeFile(filePath, "data"));
    panel.setPath(tempDir.path());
    CHECK_FALSE(panel.getSelectedItem().isEmpty());
}

TEST_CASE("FilePanel getSelectedItem returns empty when no selection") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    panel.setPath(tempDir.path());
    panel.setCurrentRow(-1);
    CHECK(panel.getSelectedItem().isEmpty());
}

TEST_CASE("FilePanel getSelectedItem returns parent entry text") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    panel.setPath(tempDir.path());
    panel.setCurrentRow(0);
    CHECK(panel.getSelectedItem() == "..");
}

TEST_CASE("FilePanel getSelectedItem returns selected file name") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel panel;

    const QString filePath = QDir(tempDir.path()).filePath("item.txt");
    REQUIRE(writeFile(filePath, "data"));
    panel.setPath(tempDir.path());
    auto items = panel.findItems("item.txt", Qt::MatchExactly);
    REQUIRE_FALSE(items.isEmpty());
    panel.setCurrentItem(items.first());
    CHECK(panel.getSelectedItem() == "item.txt");
}

TEST_CASE("FilePanel getSelectedItem stays empty for empty list") {
    FilePanel panel;
    panel.setPath("/");
    panel.clear();
    panel.setCurrentRow(-1);
    CHECK(panel.getSelectedItem().isEmpty());
}

TEST_CASE("FilePanel setActive updates stylesheet for active state") {
    FilePanel panel;

    panel.setActive(true);
    CHECK(panel.styleSheet().contains("border: 2px"));
    panel.setActive(false);
    CHECK(panel.styleSheet().contains("border: 1px"));
}

TEST_CASE("FilePanel setActive true uses active colors") {
    FilePanel panel;
    panel.setActive(true);
    CHECK(panel.styleSheet().contains("background-color"));
    CHECK(panel.styleSheet().contains("#4A90D9"));
}

TEST_CASE("FilePanel setActive false uses inactive colors") {
    FilePanel panel;
    panel.setActive(false);
    CHECK(panel.styleSheet().contains("background-color"));
    CHECK(panel.styleSheet().contains("#181825"));
}

TEST_CASE("FilePanel setActive toggles to inactive") {
    FilePanel panel;
    panel.setActive(true);
    panel.setActive(false);
    CHECK(panel.styleSheet().contains("border: 1px"));
}

TEST_CASE("FilePanel setActive toggles to active") {
    FilePanel panel;
    panel.setActive(false);
    panel.setActive(true);
    CHECK(panel.styleSheet().contains("border: 2px"));
}

TEST_CASE("FilePanel emits enterPressed on Enter key") {
    FilePanel panel;
    QSignalSpy spy(&panel, &FilePanel::enterPressed);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QCoreApplication::sendEvent(&panel, &event);
    CHECK(spy.count() == 1);
}

TEST_CASE("FilePanel emits backspacePressed on Backspace key") {
    FilePanel panel;
    QSignalSpy spy(&panel, &FilePanel::backspacePressed);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    QCoreApplication::sendEvent(&panel, &event);
    CHECK(spy.count() == 1);
}

TEST_CASE("FilePanel emits functionKeyPressed on F5 key") {
    FilePanel panel;
    QSignalSpy spy(&panel, &FilePanel::functionKeyPressed);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_F5, Qt::NoModifier);
    QCoreApplication::sendEvent(&panel, &event);
    REQUIRE(spy.count() == 1);
    CHECK(spy.takeFirst().at(0).toInt() == Qt::Key_F5);
}

TEST_CASE("FilePanel emits functionKeyPressed on F2 key") {
    FilePanel panel;
    QSignalSpy spy(&panel, &FilePanel::functionKeyPressed);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_F2, Qt::NoModifier);
    QCoreApplication::sendEvent(&panel, &event);
    REQUIRE(spy.count() == 1);
    CHECK(spy.takeFirst().at(0).toInt() == Qt::Key_F2);
}

TEST_CASE("FilePanel emits tabPressed on Tab key") {
    FilePanel panel;
    QSignalSpy spy(&panel, &FilePanel::tabPressed);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    QCoreApplication::sendEvent(&panel, &event);
    CHECK(spy.count() == 1);
}

TEST_CASE("FilePanel emits tabPressed on Backtab key") {
    FilePanel panel;
    QSignalSpy spy(&panel, &FilePanel::tabPressed);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier);
    QCoreApplication::sendEvent(&panel, &event);
    CHECK(spy.count() == 1);
}

