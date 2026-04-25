#include "MainWindow.h"
#include "FileOperations.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow() {
    setupUI();
}

void MainWindow::setupUI() {
    auto central = new QWidget();
    auto layout = new QHBoxLayout();

    leftPanel = new FilePanel();
    rightPanel = new FilePanel();

    leftPanel->setPath(QDir::homePath());
    rightPanel->setPath(QDir::homePath());

    activePanel = leftPanel;

    connect(leftPanel, &FilePanel::enterPressed, this, &MainWindow::handleEnter);
    connect(rightPanel, &FilePanel::enterPressed, this, &MainWindow::handleEnter);

    layout->addWidget(leftPanel);
    layout->addWidget(rightPanel);

    central->setLayout(layout);
    setCentralWidget(central);
}

void MainWindow::switchPanel() {
    activePanel = (activePanel == leftPanel) ? rightPanel : leftPanel;
}

void MainWindow::handleEnter() {
    QString item = activePanel->getSelectedItem();
    QString path = activePanel->getCurrentPath() + "/" + item;

    QFileInfo info(path);

    if (item == "..") {
        QDir dir(activePanel->getCurrentPath());
        dir.cdUp();
        activePanel->setPath(dir.absolutePath());
    } else if (info.isDir()) {
        activePanel->setPath(path);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {

    FilePanel* targetPanel = (activePanel == leftPanel) ? rightPanel : leftPanel;

    QString srcPath = activePanel->getCurrentPath() + "/" + activePanel->getSelectedItem();
    QString dstPath = targetPanel->getCurrentPath() + "/" + activePanel->getSelectedItem();

    switch (event->key()) {

    case Qt::Key_Tab:
        switchPanel();
        break;

    case Qt::Key_F5: 
        if (!FileOperations::copy(srcPath, dstPath)) {
            QMessageBox::critical(this, "Error", "Copy failed");
        }
        break;

    case Qt::Key_F6: 
        if (!FileOperations::move(srcPath, dstPath)) {
            QMessageBox::critical(this, "Error", "Move failed");
        }
        break;

    case Qt::Key_F7: { 
        bool ok;
        QString name = QInputDialog::getText(this, "New Folder", "Folder name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            if (!FileOperations::createDir(activePanel->getCurrentPath() + "/" + name)) {
                QMessageBox::critical(this, "Error", "Cannot create folder");
            }
        }
        break;
    }

    case Qt::Key_F8: 
        if (QMessageBox::question(this, "Delete", "Are you sure?") == QMessageBox::Yes) {
            if (!FileOperations::remove(srcPath)) {
                QMessageBox::critical(this, "Error", "Delete failed");
            }
        }
        break;

    case Qt::Key_F2: { 
        bool ok;
        QString newName = QInputDialog::getText(this, "Rename", "New name:", QLineEdit::Normal, "", &ok);
        if (ok && !newName.isEmpty()) {
            QString newPath = activePanel->getCurrentPath() + "/" + newName;
            if (!FileOperations::move(srcPath, newPath)) {
                QMessageBox::critical(this, "Error", "Rename failed");
            }
        }
        break;
    }

    default:
        break;
    }

    
    leftPanel->setPath(leftPanel->getCurrentPath());
    rightPanel->setPath(rightPanel->getCurrentPath());

    QMainWindow::keyPressEvent(event);
}