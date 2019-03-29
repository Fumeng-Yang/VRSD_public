#ifndef SESSIONINTROWINDOW_H
#define SESSIONINTROWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"

namespace Ui {
class SessionIntroWindow;
}

class SessionIntroWindow :  public AbstractBehavior
{
   Q_OBJECT

public:
   SessionIntroWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~SessionIntroWindow();
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
   Ui::SessionIntroWindow *ui;
};

#endif // SESSIONINTROWINDOW_H
