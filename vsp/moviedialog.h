#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H

#include <QDialog>

namespace Ui {
class MovieDialog;
}

class MovieDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieDialog(QWidget *parent = nullptr);
    ~MovieDialog();
    int getVolume() const;
private slots:
    void dismiss();

private:
    Ui::MovieDialog *ui;
    int iVolume;
};

#endif // MOVIEDIALOG_H
