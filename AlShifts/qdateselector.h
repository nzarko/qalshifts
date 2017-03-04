#ifndef QDATESELECTOR_H
#define QDATESELECTOR_H

#include <QWidget>
#include <QDialog>
#include <QDate>
QT_BEGIN_NAMESPACE
class QCalendarWidget;
class QDialogButtonBox;
class QVBoxLayout;
class QShowEvent;
class QKeyEvent;
QT_END_NAMESPACE

class QDateSelector : public QDialog
{
    Q_OBJECT
public:
    QDateSelector(QWidget *parent= Q_NULLPTR);
    QDate selectedDate() const;

protected:
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);
private:
    QWidget *widget;
    QCalendarWidget *calendarWidget;
    QDialogButtonBox* buttonBox;
    QVBoxLayout *verticalLayout;
};

#endif // QDATESELECTOR_H
