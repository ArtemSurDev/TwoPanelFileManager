#include "MainWindow.h"
#include "FileOperationsFacade.h"
#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow() {
    setupUI();
    
    FileOperationsFacade::getInstance()->setOnFileSystemChanged([this]() {
        mediator->refreshAllPanels();
    });
}

MainWindow::~MainWindow() {
    for (auto cmd : commandHistory) {
        delete cmd;
    }
}

void MainWindow::setupUI() {
    setWindowTitle("Two Panel File Manager");
    auto central = new QWidget();
    auto layout = new QHBoxLayout();
    leftPanel = new FilePanel();
    rightPanel = new FilePanel();
    leftPanel->setPath(QDir::homePath());
    rightPanel->setPath(QDir::rootPath());

    
    mediator = new PanelMediator(leftPanel, rightPanel);

    
    leftPanel->setActive(true);
    rightPanel->setActive(false);
    leftPanel->setFocus();

    
    connect(leftPanel, &FilePanel::enterPressed, [this]() { mediator->handleEnter(); });
    connect(rightPanel, &FilePanel::enterPressed, [this]() { mediator->handleEnter(); });

    
    connect(leftPanel, &FilePanel::backspacePressed, [this]() { mediator->handleBackspace(); });
    connect(rightPanel, &FilePanel::backspacePressed, [this]() { mediator->handleBackspace(); });

    
    connect(leftPanel, &FilePanel::tabPressed, [this]() { mediator->switchActivePanel(); });
    connect(rightPanel, &FilePanel::tabPressed, [this]() { mediator->switchActivePanel(); });

    
    connect(leftPanel, &FilePanel::functionKeyPressed, [this](int key) { handleFunctionKey(key); });
    connect(rightPanel, &FilePanel::functionKeyPressed, [this](int key) { handleFunctionKey(key); });

    layout->addWidget(leftPanel);
    layout->addWidget(rightPanel);
    central->setLayout(layout);
    setCentralWidget(central);

    statusBar()->showMessage("Ready. Tab=Switch Panel | Enter=Open | Backspace=Up | F2=Rename | F5=Copy | F6=Move | F7=New Folder | F8=Delete");
}


void MainWindow::executeCommand(FileCommand* command) {
    if (command->execute()) {
        commandHistory.append(command);
        statusBar()->showMessage(command->getDescription(), 2000);
    } else {
        QMessageBox::critical(this, "Error", FileOperationsFacade::getInstance()->getLastError());
        delete command;
    }
}

void MainWindow::handleFunctionKey(int key) {
    FilePanel* active = mediator->getActivePanel();
    FilePanel* target = mediator->getInactivePanel();
    QString selected = active->getSelectedItem();

    
    if (selected.isEmpty() && key != Qt::Key_F7) {
        return;
    }

    
    if (selected == ".." && key != Qt::Key_F7) {
        return;
    }

    QString srcPath = active->getCurrentPath() + "/" + selected;
    QString dstPath = target->getCurrentPath() + "/" + selected;

    switch (key) {
        case Qt::Key_F5:
            
            executeCommand(new CopyCommand(srcPath, dstPath));
            break;
        case Qt::Key_F6:
            
            executeCommand(new MoveCommand(srcPath, dstPath));
            break;
        case Qt::Key_F7: {
            bool ok;
            QString name = QInputDialog::getText(this, "New Folder", "Folder name:", QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                QString newPath = active->getCurrentPath() + "/" + name;
                
                executeCommand(new CreateDirCommand(newPath));
            }
            break;
        }
        case Qt::Key_F8:
            if (QMessageBox::question(this, "Delete", "Delete " + selected + "?") == QMessageBox::Yes) {
                
                executeCommand(new DeleteCommand(srcPath));
            }
            break;
        case Qt::Key_F2: {
            bool ok;
            QString newName = QInputDialog::getText(this, "Rename", "New name:", QLineEdit::Normal, selected, &ok);
            if (ok && !newName.isEmpty()) {
                QString newPath = active->getCurrentPath() + "/" + newName;
                
                executeCommand(new RenameCommand(srcPath, newPath));
            }
            break;
        }
        default:
            break;
    }
}