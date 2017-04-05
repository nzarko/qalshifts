#include <QtDebug>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QListWidget>
#include <QLayout>

#include "alshiftssettingsdialog.h"
#include "ui_alshiftssettingsdialog.h"
#include "employeeeditorform.h"


AlShiftsSettingsDialog::AlShiftsSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlShiftsSettingsDialog)
{
    ui->setupUi(this);
    ui->searchLE->setPlaceholderText(tr("Type here to search"));

    eEditForm = new EmployeeEditorForm();
    ui->s_stackedWidget->insertWidget(1,eEditForm);
    ui->s_stackedWidget->setCurrentIndex(0);

    QPushButton *btn = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(btn,&QPushButton::clicked,this, &AlShiftsSettingsDialog::applyChanges);
    btn = ui->buttonBox->button(QDialogButtonBox::Ok);
    connect(btn, &QPushButton::clicked,this, &AlShiftsSettingsDialog::accept);
    btn = ui->buttonBox->button(QDialogButtonBox::Cancel);
    connect(btn, &QPushButton::clicked,this, &AlShiftsSettingsDialog::reject);

}

AlShiftsSettingsDialog::~AlShiftsSettingsDialog()
{
    delete eEditForm;
    delete ui;
}

void AlShiftsSettingsDialog::accept()
{
    applyChanges();
    QDialog::accept();
}

void AlShiftsSettingsDialog::reject()
{
    QDialog::reject();
}

void AlShiftsSettingsDialog::applyChanges()
{
    qDebug() << "Apply clicked!" << endl;
}

void AlShiftsSettingsDialog::on_listWidget_itemChanged(QListWidgetItem *item)
{
    ui->s_stackedWidget->setCurrentIndex(ui->listWidget->currentIndex().row());
    ui->titleLabel->setText(item->text());
}

void AlShiftsSettingsDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->s_stackedWidget->setCurrentIndex(ui->listWidget->currentIndex().row());
    ui->titleLabel->setText(item->text());
}
