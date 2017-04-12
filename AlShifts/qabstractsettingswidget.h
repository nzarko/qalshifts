#ifndef QABSTRACTSETTINGSWIDGET_H
#define QABSTRACTSETTINGSWIDGET_H

#include <QObject>
#include <QWidget>

class QAbstractSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QAbstractSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    virtual bool applyChanges() const = 0;
};

#endif // QABSTRACTSETTINGSWIDGET_H
