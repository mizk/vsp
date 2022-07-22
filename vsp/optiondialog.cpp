#include "optiondialog.h"
#include "ui_optiondialog.h"

OptionDialog::OptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());
    connect(ui->openButton,&QPushButton::clicked,this,&OptionDialog::action1);
    connect(ui->exploreButton,&QPushButton::clicked,this,&OptionDialog::action2);
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

int OptionDialog::which() const
{
    return iWhich;
}

void OptionDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    iWhich= 0;
}

void OptionDialog::action1()
{
    iWhich=1;
    this->accept();
}

void OptionDialog::action2()
{
    iWhich=2;
    this->accept();
}
