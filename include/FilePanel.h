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

    signals:
        void enterPressed();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    QString currentPath;
    void refresh();
};