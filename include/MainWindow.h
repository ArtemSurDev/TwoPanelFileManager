#pragma once
#include <QMainWindow>
#include "FilePanel.h"
#include "PanelMediator.h"
#include "FileCommand.h"
#include <QVector>

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    FilePanel* leftPanel;
    FilePanel* rightPanel;
    PanelMediator* mediator;
    QVector<FileCommand*> commandHistory;
    void setupUI();
    void executeCommand(FileCommand* command);
    void handleFunctionKey(int key);
};