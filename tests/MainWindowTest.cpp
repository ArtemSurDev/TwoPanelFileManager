#include <catch2/catch_test_macros.hpp>
#include <QDir>
#include <QStatusBar>
#include <QLabel>
#include "MainWindow.h"

TEST_CASE("MainWindow sets window title") {
    MainWindow window;
    CHECK(window.windowTitle() == "Two Panel File Manager");
}

TEST_CASE("MainWindow has two file panels") {
    MainWindow window;
    const auto panels = window.findChildren<FilePanel*>();
    CHECK(panels.size() == 2);
}

TEST_CASE("MainWindow initializes panel paths") {
    MainWindow window;
    const auto panels = window.findChildren<FilePanel*>();
    REQUIRE(panels.size() == 2);

    const QString home = QDir::homePath();
    const QString pathA = panels.at(0)->getCurrentPath();
    const QString pathB = panels.at(1)->getCurrentPath();

    CHECK(pathA == home);
    CHECK(pathB == home);
}

TEST_CASE("MainWindow status bar shows shortcut help") {
    MainWindow window;
    REQUIRE(window.statusBar() != nullptr);
    const auto labels = window.statusBar()->findChildren<QLabel*>();
    bool found = false;
    for (const auto* label : labels) {
        if (label->text().contains("Tab=Switch Panel")) {
            found = true;
            break;
        }
    }
    CHECK(found);
}

TEST_CASE("MainWindow sets one panel active and one inactive") {
    MainWindow window;
    const auto panels = window.findChildren<FilePanel*>();
    REQUIRE(panels.size() == 2);

    const QString styleA = panels.at(0)->styleSheet();
    const QString styleB = panels.at(1)->styleSheet();

    CHECK(((styleA.contains("border: 2px") && styleB.contains("border: 1px")) ||
           (styleA.contains("border: 1px") && styleB.contains("border: 2px"))));
}
