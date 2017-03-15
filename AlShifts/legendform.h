#ifndef LEGENDFORM_H
#define LEGENDFORM_H

#include <QWidget>

namespace Ui {
class LegendForm;
}

class LegendForm : public QWidget
{
    Q_OBJECT

public:
    explicit LegendForm(QWidget *parent = 0);
    ~LegendForm();

private:
    Ui::LegendForm *ui;
};

#endif // LEGENDFORM_H
