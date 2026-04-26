#pragma once
#include <QListWidget>
#include <QString>

class FilePanel : public QListWidget {
Q_OBJECT

public:
    explicit FilePanel(QWidget* parent = nullptr);
    void setPath(const QString& path);
    QString getCurrentPath() const;
    QString getSelectedItem() const;
    void refresh();
    void setActive(bool active);

signals:
    void enterPressed();
    void backspacePressed();
    void tabPressed();
    void functionKeyPressed(int key);
    void panelFocused(FilePanel* panel);

protected:
    bool event(QEvent* e) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString currentPath;
};