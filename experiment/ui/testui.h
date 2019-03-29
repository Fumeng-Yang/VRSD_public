#ifndef TESTUI_H
#define TESTUI_H

#include <QWidget>

namespace Ui {
class TestUI;
}

class TestUI : public QWidget
{
    Q_OBJECT

public:
    explicit TestUI(QWidget *parent = nullptr);
    ~TestUI();

private:
    Ui::TestUI *ui;
};

#endif // TESTUI_H
