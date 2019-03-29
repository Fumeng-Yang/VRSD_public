#ifndef BREAKWINDOW_H
#define BREAKWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"
namespace Ui {
class BreakWindow;
}

class BreakWindow : public AbstractBehavior
{
   Q_OBJECT

public:
   BreakWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~BreakWindow();
   void setNextTwoWindow(AbstractBehavior *next2);
   void setComponents() override;
   void prepareWindow() override;
   void setNextWindow(AbstractBehavior *next);

public   slots:
   void receiveCall() {
      handleCall();
   }

signals:
   void callNextWindow();

private slots:
   void on_nextButton_clicked();
   void checkTime();

protected:
   void keyPressEvent(QKeyEvent *event) override;

private:
   Ui::BreakWindow *ui;
   int             m_time;
   QMainWindow     *m_next2;
   QTimer          *timer;
};

#endif // BREAKWINDOW_H
