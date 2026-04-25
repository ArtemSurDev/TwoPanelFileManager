#include "FilePanel.h"
#include <QDir>
#include <QKeyEvent>

FilePanel::FilePanel(QWidget* parent) : QListWidget(parent) {}

void FilePanel::setPath(const QString& path) {
    currentPath = path;
    refresh();
}

QString FilePanel::getCurrentPath() const {
    return currentPath;
}

QString FilePanel::getSelectedItem() const {
    auto item = currentItem();
    if (!item) return "";
    return item->text();
}

void FilePanel::refresh() {
    clear();
    QDir dir(currentPath);
    if (currentPath != "/") {
        addItem("..");
    }
    for (const auto& file : dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        addItem(file);
    }
}

void FilePanel::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit enterPressed();
    }
    QListWidget::keyPressEvent(event);
}