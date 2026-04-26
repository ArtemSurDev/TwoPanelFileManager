#include "PanelMediator.h"
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

PanelMediator::PanelMediator(FilePanel* left, FilePanel* right)
        : leftPanel(left), rightPanel(right), activePanel(left) {}

void PanelMediator::switchActivePanel() {
    activePanel = (activePanel == leftPanel) ? rightPanel : leftPanel;
    leftPanel->setActive(activePanel == leftPanel);
    rightPanel->setActive(activePanel == rightPanel);
    activePanel->setFocus();
}

FilePanel* PanelMediator::getActivePanel() const {
    return activePanel;
}

FilePanel* PanelMediator::getInactivePanel() const {
    return (activePanel == leftPanel) ? rightPanel : leftPanel;
}

void PanelMediator::handleEnter() {
    QString item = activePanel->getSelectedItem();
    if (item.isEmpty()) return;
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

void PanelMediator::handleBackspace() {
    QString currentPath = activePanel->getCurrentPath();
    if (currentPath != "/") {
        QDir dir(currentPath);
        dir.cdUp();
        activePanel->setPath(dir.absolutePath());
    }
}

void PanelMediator::refreshAllPanels() {
    leftPanel->refresh();
    rightPanel->refresh();
}