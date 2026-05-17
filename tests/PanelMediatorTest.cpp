#include <catch2/catch_test_macros.hpp>
#include <QTemporaryDir>
#include <QDir>
#include "PanelMediator.h"
#include "TestUtils.h"

TEST_CASE("PanelMediator starts with left panel active") {
    FilePanel left;
    FilePanel right;
    PanelMediator mediator(&left, &right);

    CHECK(mediator.getActivePanel() == &left);
    CHECK(mediator.getInactivePanel() == &right);
}

TEST_CASE("PanelMediator switches active panel") {
    FilePanel left;
    FilePanel right;
    PanelMediator mediator(&left, &right);

    mediator.switchActivePanel();
    CHECK(mediator.getActivePanel() == &right);
    CHECK(mediator.getInactivePanel() == &left);
}

TEST_CASE("PanelMediator switchActivePanel toggles back to left") {
    FilePanel left;
    FilePanel right;
    PanelMediator mediator(&left, &right);

    mediator.switchActivePanel();
    mediator.switchActivePanel();
    CHECK(mediator.getActivePanel() == &left);
    CHECK(mediator.getInactivePanel() == &right);
}

TEST_CASE("PanelMediator switchActivePanel updates active styles") {
    FilePanel left;
    FilePanel right;
    PanelMediator mediator(&left, &right);

    mediator.switchActivePanel();
    CHECK(left.styleSheet().contains("border: 1px"));
    CHECK(right.styleSheet().contains("border: 2px"));
}

TEST_CASE("PanelMediator switchActivePanel updates inactive styles") {
    FilePanel left;
    FilePanel right;
    PanelMediator mediator(&left, &right);

    mediator.switchActivePanel();
    mediator.switchActivePanel();
    CHECK(left.styleSheet().contains("border: 2px"));
    CHECK(right.styleSheet().contains("border: 1px"));
}

TEST_CASE("PanelMediator switchActivePanel keeps inactive pointer consistent") {
    FilePanel left;
    FilePanel right;
    PanelMediator mediator(&left, &right);

    mediator.switchActivePanel();
    CHECK(mediator.getInactivePanel() == &left);
    mediator.switchActivePanel();
    CHECK(mediator.getInactivePanel() == &right);
}

TEST_CASE("PanelMediator handleBackspace goes to parent directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString nested = base.filePath("nested");

    REQUIRE(QDir().mkpath(nested));
    FilePanel left;
    FilePanel right;
    left.setPath(nested);
    PanelMediator mediator(&left, &right);

    mediator.handleBackspace();
    CHECK(left.getCurrentPath() == base.absolutePath());
}

TEST_CASE("PanelMediator handleBackspace stays at root") {
    FilePanel left;
    FilePanel right;
    left.setPath("/");
    PanelMediator mediator(&left, &right);

    mediator.handleBackspace();
    CHECK(left.getCurrentPath() == "/");
}

TEST_CASE("PanelMediator handleBackspace goes up one level from deep path") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString nested = base.filePath("a/b/c");

    REQUIRE(QDir().mkpath(nested));
    FilePanel left;
    FilePanel right;
    left.setPath(nested);
    PanelMediator mediator(&left, &right);

    QDir parentDir(QDir(nested).absolutePath());
    parentDir.cdUp();
    const QString expected = parentDir.absolutePath();

    mediator.handleBackspace();
    CHECK(left.getCurrentPath() == expected);
}

TEST_CASE("PanelMediator handleBackspace goes up from home directory") {
    const QString home = QDir::homePath();
    QDir dir(home);
    dir.cdUp();
    const QString parent = dir.absolutePath();

    FilePanel left;
    FilePanel right;
    left.setPath(home);
    PanelMediator mediator(&left, &right);

    mediator.handleBackspace();
    CHECK(left.getCurrentPath() == parent);
}

TEST_CASE("PanelMediator handleBackspace does not change when already root") {
    FilePanel left;
    FilePanel right;
    left.setPath("/");
    PanelMediator mediator(&left, &right);

    mediator.handleBackspace();
    CHECK(left.getCurrentPath() == "/");
}

TEST_CASE("PanelMediator handleEnter goes up on parent item") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString nested = base.filePath("nested");

    REQUIRE(QDir().mkpath(nested));
    FilePanel left;
    FilePanel right;
    left.setPath(nested);
    left.setCurrentRow(0);
    PanelMediator mediator(&left, &right);

    mediator.handleEnter();
    CHECK(left.getCurrentPath() == base.absolutePath());
}

TEST_CASE("PanelMediator handleEnter opens directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString nested = base.filePath("child");

    REQUIRE(QDir().mkpath(nested));
    FilePanel left;
    FilePanel right;
    left.setPath(base.absolutePath());
    auto items = left.findItems("child", Qt::MatchExactly);
    REQUIRE_FALSE(items.isEmpty());
    left.setCurrentItem(items.first());
    PanelMediator mediator(&left, &right);

    mediator.handleEnter();
    CHECK(left.getCurrentPath() == nested);
}

TEST_CASE("PanelMediator handleEnter ignores empty selection") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel left;
    FilePanel right;
    left.setPath(tempDir.path());
    left.setCurrentRow(-1);
    PanelMediator mediator(&left, &right);

    const QString before = left.getCurrentPath();
    mediator.handleEnter();
    CHECK(left.getCurrentPath() == before);
}

TEST_CASE("PanelMediator handleEnter opens directory with spaces") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString nested = base.filePath("dir with spaces");

    REQUIRE(QDir().mkpath(nested));
    FilePanel left;
    FilePanel right;
    left.setPath(base.absolutePath());
    auto items = left.findItems("dir with spaces", Qt::MatchExactly);
    REQUIRE_FALSE(items.isEmpty());
    left.setCurrentItem(items.first());
    PanelMediator mediator(&left, &right);

    mediator.handleEnter();
    CHECK(left.getCurrentPath() == nested);
}

TEST_CASE("PanelMediator handleEnter keeps path when selection cleared") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel left;
    FilePanel right;
    left.setPath(tempDir.path());
    left.clear();
    left.setCurrentRow(-1);
    PanelMediator mediator(&left, &right);

    const QString before = left.getCurrentPath();
    mediator.handleEnter();
    CHECK(left.getCurrentPath() == before);
}

TEST_CASE("PanelMediator refreshAllPanels updates panel items") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString filePath = base.filePath("file.txt");

    FilePanel left;
    FilePanel right;
    left.setPath(base.absolutePath());
    right.setPath(base.absolutePath());
    REQUIRE(writeFile(filePath, "content"));
    PanelMediator mediator(&left, &right);

    mediator.refreshAllPanels();
    CHECK_FALSE(left.findItems("file.txt", Qt::MatchExactly).isEmpty());
    CHECK_FALSE(right.findItems("file.txt", Qt::MatchExactly).isEmpty());
}

TEST_CASE("PanelMediator refreshAllPanels updates after new directory") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString dirPath = base.filePath("dir");

    FilePanel left;
    FilePanel right;
    left.setPath(base.absolutePath());
    right.setPath(base.absolutePath());
    REQUIRE(QDir().mkpath(dirPath));
    PanelMediator mediator(&left, &right);

    mediator.refreshAllPanels();
    CHECK_FALSE(left.findItems("dir", Qt::MatchExactly).isEmpty());
    CHECK_FALSE(right.findItems("dir", Qt::MatchExactly).isEmpty());
}

TEST_CASE("PanelMediator refreshAllPanels works with different paths") {
    QTemporaryDir tempA;
    QTemporaryDir tempB;
    REQUIRE(tempA.isValid());
    REQUIRE(tempB.isValid());

    REQUIRE(writeFile(QDir(tempA.path()).filePath("a.txt"), "a"));
    REQUIRE(writeFile(QDir(tempB.path()).filePath("b.txt"), "b"));

    FilePanel left;
    FilePanel right;
    left.setPath(tempA.path());
    right.setPath(tempB.path());
    PanelMediator mediator(&left, &right);

    mediator.refreshAllPanels();
    CHECK_FALSE(left.findItems("a.txt", Qt::MatchExactly).isEmpty());
    CHECK_FALSE(right.findItems("b.txt", Qt::MatchExactly).isEmpty());
}

TEST_CASE("PanelMediator refreshAllPanels removes deleted items") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    QDir base(tempDir.path());
    const QString filePath = base.filePath("temp.txt");

    FilePanel left;
    FilePanel right;
    left.setPath(base.absolutePath());
    right.setPath(base.absolutePath());
    REQUIRE(writeFile(filePath, "data"));
    PanelMediator mediator(&left, &right);
    mediator.refreshAllPanels();
    REQUIRE_FALSE(left.findItems("temp.txt", Qt::MatchExactly).isEmpty());

    QFile::remove(filePath);
    mediator.refreshAllPanels();
    CHECK(left.findItems("temp.txt", Qt::MatchExactly).isEmpty());
}

TEST_CASE("PanelMediator refreshAllPanels keeps parent entry") {
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());
    FilePanel left;
    FilePanel right;
    left.setPath(tempDir.path());
    right.setPath(tempDir.path());
    PanelMediator mediator(&left, &right);

    mediator.refreshAllPanels();
    REQUIRE(left.count() > 0);
    CHECK(left.item(0)->text() == "..");
}

