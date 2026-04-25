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
    auto central = new QWidget();
    auto layout = new QHBoxLayout();
    leftPanel = new FilePanel();
    rightPanel = new FilePanel();
    leftPanel->setPath(QDir::homePath());
    rightPanel->setPath(QDir::homePath());
    mediator = new PanelMediator(leftPanel, rightPanel);
    connect(leftPanel, &FilePanel::enterPressed, [this]() { mediator->handleEnter(); });
    connect(rightPanel, &FilePanel::enterPressed, [this]() { mediator->handleEnter(); });
    layout->addWidget(leftPanel);
    layout->addWidget(rightPanel);
    central->setLayout(layout);
    setCentralWidget(central);
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

void MainWindow::keyPressEvent(QKeyEvent* event) {
    FilePanel* active = mediator->getActivePanel();
    FilePanel* target = mediator->getInactivePanel();
    QString selected = active->getSelectedItem();
    if (selected.isEmpty() && event->key() != Qt::Key_F7) {
        QMainWindow::keyPressEvent(event);
        return;
    }
    QString srcPath = active->getCurrentPath() + "/" + selected;
    QString dstPath = target->getCurrentPath() + "/" + selected;
    switch (event->key()) {
        case Qt::Key_Tab:
            mediator->switchActivePanel();
            break;
        case Qt::Key_F5:
            if (!selected.isEmpty()) {
                executeCommand(new CopyCommand(srcPath, dstPath));
            }
            break;
        case Qt::Key_F6:
            if (!selected.isEmpty()) {
                executeCommand(new MoveCommand(srcPath, dstPath));
            }
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
            if (!selected.isEmpty() && QMessageBox::question(this, "Delete", "Delete " + selected + "?") == QMessageBox::Yes) {
                executeCommand(new DeleteCommand(srcPath));
            }
            break;
        case Qt::Key_F2: {
            if (!selected.isEmpty()) {
                bool ok;
                QString newName = QInputDialog::getText(this, "Rename", "New name:", QLineEdit::Normal, "", &ok);
                if (ok && !newName.isEmpty()) {
                    QString newPath = active->getCurrentPath() + "/" + newName;
                    executeCommand(new RenameCommand(srcPath, newPath));
                }
            }
            break;
        }
        default:
            QMainWindow::keyPressEvent(event);
            return;
    }
    QMainWindow::keyPressEvent(event);
}