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
    // Observer pattern: register callback on the Singleton Facade
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

    // Mediator pattern: create the mediator to coordinate panels
    mediator = new PanelMediator(leftPanel, rightPanel);

    // Set initial visual state - left panel is active by default
    leftPanel->setActive(true);
    rightPanel->setActive(false);
    leftPanel->setFocus();

    // Connect Enter signals via Mediator
    connect(leftPanel, &FilePanel::enterPressed, [this]() { mediator->handleEnter(); });
    connect(rightPanel, &FilePanel::enterPressed, [this]() { mediator->handleEnter(); });

    // Connect Backspace signals via Mediator
    connect(leftPanel, &FilePanel::backspacePressed, [this]() { mediator->handleBackspace(); });
    connect(rightPanel, &FilePanel::backspacePressed, [this]() { mediator->handleBackspace(); });

    // Connect Tab signals via Mediator
    connect(leftPanel, &FilePanel::tabPressed, [this]() { mediator->switchActivePanel(); });
    connect(rightPanel, &FilePanel::tabPressed, [this]() { mediator->switchActivePanel(); });

    // Connect function key signals (F2, F5, F6, F7, F8)
    connect(leftPanel, &FilePanel::functionKeyPressed, [this](int key) { handleFunctionKey(key); });
    connect(rightPanel, &FilePanel::functionKeyPressed, [this](int key) { handleFunctionKey(key); });

    layout->addWidget(leftPanel);
    layout->addWidget(rightPanel);
    central->setLayout(layout);
    setCentralWidget(central);

    statusBar()->showMessage("Ready. Tab=Switch Panel | Enter=Open | Backspace=Up | F2=Rename | F5=Copy | F6=Move | F7=New Folder | F8=Delete");
}

// Command pattern: execute a command and store it in history
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

    // F7 (create folder) doesn't require a selection
    if (selected.isEmpty() && key != Qt::Key_F7) {
        return;
    }

    // Skip ".." for file operations
    if (selected == ".." && key != Qt::Key_F7) {
        return;
    }

    QString srcPath = active->getCurrentPath() + "/" + selected;
    QString dstPath = target->getCurrentPath() + "/" + selected;

    switch (key) {
        case Qt::Key_F5:
            // Command pattern: CopyCommand
            executeCommand(new CopyCommand(srcPath, dstPath));
            break;
        case Qt::Key_F6:
            // Command pattern: MoveCommand
            executeCommand(new MoveCommand(srcPath, dstPath));
            break;
        case Qt::Key_F7: {
            bool ok;
            QString name = QInputDialog::getText(this, "New Folder", "Folder name:", QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                QString newPath = active->getCurrentPath() + "/" + name;
                // Command pattern: CreateDirCommand
                executeCommand(new CreateDirCommand(newPath));
            }
            break;
        }
        case Qt::Key_F8:
            if (QMessageBox::question(this, "Delete", "Delete " + selected + "?") == QMessageBox::Yes) {
                // Command pattern: DeleteCommand
                executeCommand(new DeleteCommand(srcPath));
            }
            break;
        case Qt::Key_F2: {
            bool ok;
            QString newName = QInputDialog::getText(this, "Rename", "New name:", QLineEdit::Normal, selected, &ok);
            if (ok && !newName.isEmpty()) {
                QString newPath = active->getCurrentPath() + "/" + newName;
                // Command pattern: RenameCommand
                executeCommand(new RenameCommand(srcPath, newPath));
            }
            break;
        }
        default:
            break;
    }
}