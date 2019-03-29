#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QMainWindow>
#include "abstractbehavior.h"
#include "settings.h"

namespace Ui {
class SetupWindow;
}

class SetupWindow :   public AbstractBehavior
{
   Q_OBJECT

public:
   SetupWindow(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   ~SetupWindow();
   void prepareWindow() override;
   void setComponents() override;

   void parseParticipantProfile();
   void parseDatasets();

   void parseTrialData(QString, TrialControl&, int, int, float);

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
   Ui::SetupWindow *ui;
   QMainWindow     *nextWindow;
};

#endif // SETUPWINDOW_H
