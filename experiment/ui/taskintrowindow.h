#ifndef TASKINTROWINDOW_H
#define TASKINTROWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"

namespace Ui {
    class TaskIntroWindow;
}

class TaskIntroductionWindow : public AbstractBehavior
{
    Q_OBJECT

   public:
    TaskIntroductionWindow(QWidget * parent = nullptr, AbstractBehavior * next = nullptr);
    ~TaskIntroductionWindow();
    void setComponents() override;
    void prepareWindow() override;

   public   slots:
    void receiveCall() {
        handleCall();
    }

   signals:
    void callNextWindow();

   protected:
    void keyPressEvent(QKeyEvent * event) override;

   private slots:
    void on_nextButton_clicked();

   private:
    Ui::TaskIntroWindow * ui;
};

#endif // TASKINTROWINDOW_H
