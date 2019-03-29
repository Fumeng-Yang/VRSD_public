#ifndef MAINUI_H
#define MAINUI_H

#include <QButtonGroup>
#include <QMainWindow>
#include <QMatrix4x4>
#include "setuppage.h"
#include "instructionview.h"
#include "visview.h"
#include "testui.h"

namespace Ui {
class MainWindowFrame;
}


class SetupPage;
class InstructionView;
class VisView;
class TestUI;

class MainWindowFrame : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowFrame(QWidget *parent = nullptr);
    ~MainWindowFrame();

    void firstAdjustComponentsSize();
    void nextSession();
    void cleanWidgets();

private:
    Ui::MainWindowFrame *ui;

    VisView         *vispage;
    SetupPage       *setuppage;
    InstructionView *instrpage;
    TestUI          *testui;
};

#endif // MAINUI_H
