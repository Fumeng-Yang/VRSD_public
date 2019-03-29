#ifndef OPENQUESTIONSWINDOW_H
#define OPENQUESTIONSWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"
namespace Ui {
class OpenQuestionsWindow;
}

class OpenQuestionsWindow : public AbstractBehavior
{
   Q_OBJECT

public:
   OpenQuestionsWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~OpenQuestionsWindow();

   void setComponents() override;
   void prepareWindow() override;
   void setNextWindow(AbstractBehavior *next);

   void setNextSecondWindow(AbstractBehavior *);

public   slots:
   void receiveCall() {
      handleCall();
   }

signals:
   void callNextWindow1();
   void callNextWindow2();
   void callNextWindow();

protected:
   void keyPressEvent(QKeyEvent *event) override;

private slots:
   void on_nextButton_clicked();

private:
   Ui::OpenQuestionsWindow *ui;

   AbstractBehavior *m_next2;
};

#endif // OPENQUESTIONSWINDOW_H
