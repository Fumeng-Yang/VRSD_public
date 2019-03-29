#ifndef PREQUESWINDOW_H
#define PREQUESWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"

namespace Ui {
class PreQuesWindow;
}

class PreQuesWindow : public AbstractBehavior
{
   Q_OBJECT

public:
   PreQuesWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~PreQuesWindow();
   void setComponents() override;
   void prepareWindow() override;

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
   Ui::PreQuesWindow *ui;
};

#endif // PREQUESWINDOW_H
