#include "abstractbehavior.h"

AbstractBehavior::AbstractBehavior(QWidget *parent, AbstractBehavior *next) :    QMainWindow(parent) {
   this->next = next;
   this->setEnabled(false);

}


void AbstractBehavior::setNextWindow(AbstractBehavior *next) {
   //this->next = next;
}


void AbstractBehavior::handleCall() {
   setComponents();
   prepareWindow();
}


void AbstractBehavior::closeEvent(QCloseEvent *event) {
   event->ignore();
}
