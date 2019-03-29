#ifndef POSTQUESWINDOW_H
#define POSTQUESWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"

namespace Ui {
class PostQuesWindow;
}

class PostQuesWindow :  public AbstractBehavior
{
   Q_OBJECT

public:
   PostQuesWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~PostQuesWindow();
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
   Ui::PostQuesWindow *ui;
};

#endif // POSTQUESWINDOW_H
