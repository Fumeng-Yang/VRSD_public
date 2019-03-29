#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QMainWindow>
#include <QMatrix4x4>

#include "viswidget.h"
#include "abstractbehavior.h"
namespace Ui {
class VisWindow;
}

class SetupPage;
class InstructionView;
class VisWidget;

class VisWindow : public AbstractBehavior
{
   Q_OBJECT

public:
   VisWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~VisWindow();
   void prepareWindow() override;
   void setComponents() override;

public   slots:
   void receiveCall() {
      handleCall();
   }

signals:
   void callNextWindow();

protected:
   void keyPressEvent(QKeyEvent *event) override;

public slots:
   void nextWindow();
   QString nextPart();

private:
   Ui::VisWindow *ui;

   VisWidget *viswidget;
};

#endif // MAINWINDOW_H
