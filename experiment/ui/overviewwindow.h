#ifndef INSTRUCTIONWINDOW_H
#define INSTRUCTIONWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"

namespace Ui {
class InstructionWindow;
}

class OverviewWindow :   public AbstractBehavior
{
   Q_OBJECT

public:
   OverviewWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~OverviewWindow() override;

   void setComponents() override;
   void prepareWindow() override;
   void updateInstructions(std::string instr, std::string path);

public   slots:
   void receiveCall() {
      handleCall();
   }

signals:
   void callNextWindow();

protected:
   void keyPressEvent(QKeyEvent *event) override;

private slots:
   void on_nextButton_clicked();

private:
   Ui::InstructionWindow *ui;
};

#endif // INSTRUCTIONWINDOW_H
