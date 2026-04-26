#include "FilePanel.h"
#include <QDir>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QMouseEvent>

FilePanel::FilePanel(QWidget* parent) : QListWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

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
    // Select first item so user always has something selected
    if (count() > 0) {
        setCurrentRow(0);
    }
}

void FilePanel::setActive(bool active) {
    if (active) {
        setStyleSheet("QListWidget { border: 2px solid #4A90D9; background-color: #1E1E2E; color: #CDD6F4; }"
                       "QListWidget::item:selected { background-color: #4A90D9; color: white; }");
    } else {
        setStyleSheet("QListWidget { border: 1px solid #555; background-color: #181825; color: #A6ADC8; }"
                       "QListWidget::item:selected { background-color: #45475A; color: #CDD6F4; }");
    }
}

// Override event() to intercept Tab BEFORE Qt's focus system processes it.
// QWidget::event() handles Tab by calling focusNextChild() before keyPressEvent
// is ever reached, so keyPressEvent alone cannot reliably catch Tab.
bool FilePanel::event(QEvent* e) {
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Tab || ke->key() == Qt::Key_Backtab) {
            emit tabPressed();
            return true;  // Event consumed — Qt will NOT do focus traversal
        }
    }
    return QListWidget::event(e);
}

void FilePanel::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit enterPressed();
            return;  // Don't pass to QListWidget (prevents edit mode)
        case Qt::Key_Backspace:
            emit backspacePressed();
            return;
        case Qt::Key_F2:
        case Qt::Key_F5:
        case Qt::Key_F6:
        case Qt::Key_F7:
        case Qt::Key_F8:
            emit functionKeyPressed(event->key());
            return;
        default:
            QListWidget::keyPressEvent(event);
            break;
    }
}

// When panel receives focus (by click or programmatic setFocus), sync the mediator
void FilePanel::focusInEvent(QFocusEvent* event) {
    QListWidget::focusInEvent(event);
    emit panelFocused(this);
}

// Ensure click on the panel gives it focus and notifies the mediator
void FilePanel::mousePressEvent(QMouseEvent* event) {
    QListWidget::mousePressEvent(event);
    setFocus();
    emit panelFocused(this);
}