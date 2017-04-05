#ifndef ALSHIFTSSETTINGSDIALOG_H
#define ALSHIFTSSETTINGSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

class EmployeeEditorForm;

namespace Ui {
class AlShiftsSettingsDialog;
}

class AlShiftsSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlShiftsSettingsDialog(QWidget *parent = 0);
    ~AlShiftsSettingsDialog();

private:
    Ui::AlShiftsSettingsDialog *ui;
    EmployeeEditorForm *eEditForm;

public Q_SLOTS:
    void accept();
    void reject();
    void applyChanges();
private slots:
    void on_listWidget_itemChanged(QListWidgetItem *item);
    void on_listWidget_itemClicked(QListWidgetItem *item);
};

#endif // ALSHIFTSSETTINGSDIALOG_H
