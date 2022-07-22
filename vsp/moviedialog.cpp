#include "moviedialog.h"
#include "ui_moviedialog.h"

MovieDialog::MovieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());
    connect(ui->doneButton,&QPushButton::clicked,this,&MovieDialog::dismiss);
}

MovieDialog::~MovieDialog()
{
    delete ui;
}

int MovieDialog::getVolume() const
{
    return iVolume;
}

void MovieDialog::dismiss()
{
    iVolume=ui->volume->value();
    this->accept();
}
