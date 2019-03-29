#ifndef ENDWINDOW_H
#define ENDWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"
namespace Ui {
class EndWindow;
}

class EndWindow : public AbstractBehavior
{
   Q_OBJECT

public:
   EndWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~EndWindow();
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

private slots:
   void on_nextButton_clicked();

private:
   Ui::EndWindow *ui;
};

#endif // ENDWINDOW_H
