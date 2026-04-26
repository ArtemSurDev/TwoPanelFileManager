#pragma once
#include "FilePanel.h"

class PanelMediator {
private:
    FilePanel* leftPanel;
    FilePanel* rightPanel;
    FilePanel* activePanel;

public:
    PanelMediator(FilePanel* left, FilePanel* right);
    void switchActivePanel();
    FilePanel* getActivePanel() const;
    FilePanel* getInactivePanel() const;
    void handleEnter();
    void handleBackspace();
    void refreshAllPanels();
};