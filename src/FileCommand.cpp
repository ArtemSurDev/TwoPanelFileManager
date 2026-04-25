#include "FileCommand.h"
#include "FileOperationsFacade.h"
#include <QFileInfo>

CopyCommand::CopyCommand(const QString& src, const QString& tgt) : source(src), target(tgt) {}
bool CopyCommand::execute() { return FileOperationsFacade::getInstance()->copy(source, target); }
bool CopyCommand::undo() { return FileOperationsFacade::getInstance()->remove(target); }
QString CopyCommand::getDescription() const { return "Copy " + source + " -> " + target; }

MoveCommand::MoveCommand(const QString& src, const QString& tgt) : source(src), target(tgt) {}
bool MoveCommand::execute() { return FileOperationsFacade::getInstance()->move(source, target); }
bool MoveCommand::undo() { return FileOperationsFacade::getInstance()->move(target, source); }
QString MoveCommand::getDescription() const { return "Move " + source + " -> " + target; }

DeleteCommand::DeleteCommand(const QString& p) : path(p) {}
bool DeleteCommand::execute() { return FileOperationsFacade::getInstance()->remove(path); }
bool DeleteCommand::undo() { return false; }
QString DeleteCommand::getDescription() const { return "Delete " + path; }

CreateDirCommand::CreateDirCommand(const QString& p) : path(p) {}
bool CreateDirCommand::execute() { return FileOperationsFacade::getInstance()->createDir(path); }
bool CreateDirCommand::undo() { return FileOperationsFacade::getInstance()->remove(path); }
QString CreateDirCommand::getDescription() const { return "Create directory " + path; }

RenameCommand::RenameCommand(const QString& oldP, const QString& newP) : oldPath(oldP), newPath(newP) {}
bool RenameCommand::execute() { return FileOperationsFacade::getInstance()->move(oldPath, newPath); }
bool RenameCommand::undo() { return FileOperationsFacade::getInstance()->move(newPath, oldPath); }
QString RenameCommand::getDescription() const { return "Rename " + oldPath + " -> " + newPath; }