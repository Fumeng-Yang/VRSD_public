#ifndef ABSTRACTBEHAVIOR_H
#define ABSTRACTBEHAVIOR_H

#include <QMainWindow>
#include <QCloseEvent>

class AbstractBehavior : public QMainWindow
{
public:
   AbstractBehavior() {}
   explicit AbstractBehavior(QWidget *parent = nullptr, AbstractBehavior *next = nullptr);
   virtual ~AbstractBehavior() {}
   virtual void setComponents() {}
   virtual void prepareWindow() {}
   void setNextWindow(AbstractBehavior *next);
   void handleCall();
   void closeEvent(QCloseEvent *event) override;

protected:
   AbstractBehavior *next;
};

#endif // ABSTRACTBEHAVIOR_H
