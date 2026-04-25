#pragma once

#include <QMainWindow>
#include "FilePanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private:
    FilePanel* leftPanel;
    FilePanel* rightPanel;
    FilePanel* activePanel;

    void setupUI();
    void switchPanel();
    void handleEnter();

protected:
    void keyPressEvent(QKeyEvent* event) override;
};