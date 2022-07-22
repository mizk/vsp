#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);
    ~OptionDialog();
    QString file;
    int which() const;
protected:
    virtual void showEvent(QShowEvent* event) override;
private slots:
    void action1();
    void action2();
private:
    int iWhich=0;
    Ui::OptionDialog *ui;
};

#endif // OPTIONDIALOG_H
