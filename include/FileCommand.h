#pragma once
#include <QString>

class FileCommand {
public:
    virtual ~FileCommand() = default;
    virtual bool execute() = 0;
    virtual bool undo() = 0;
    virtual QString getDescription() const = 0;
};

class CopyCommand : public FileCommand {
    QString source, target;
public:
    CopyCommand(const QString& src, const QString& tgt);
    bool execute() override;
    bool undo() override;
    QString getDescription() const override;
};

class MoveCommand : public FileCommand {
    QString source, target;
public:
    MoveCommand(const QString& src, const QString& tgt);
    bool execute() override;
    bool undo() override;
    QString getDescription() const override;
};

class DeleteCommand : public FileCommand {
    QString path;
public:
    DeleteCommand(const QString& p);
    bool execute() override;
    bool undo() override;
    QString getDescription() const override;
};

class CreateDirCommand : public FileCommand {
    QString path;
public:
    CreateDirCommand(const QString& p);
    bool execute() override;
    bool undo() override;
    QString getDescription() const override;
};

class RenameCommand : public FileCommand {
    QString oldPath, newPath;
public:
    RenameCommand(const QString& oldP, const QString& newP);
    bool execute() override;
    bool undo() override;
    QString getDescription() const override;
};