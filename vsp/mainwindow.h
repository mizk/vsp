#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "processor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QStandardItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void closeEvent(QCloseEvent* event) override;
private slots:
    void onAdd();
    void onDel();
    void onGo();
    void go();
    void onProcessorGoing(const QString& file,qint64 now,qint64 max);
    void onProcessorProgress(int progress);
    void onProcessorQuit();
    void onProcessorNotify(const QString& file);
    void playback(const QModelIndex& index);
    void openOption();

private:
    void initUI();
    void initEvent();
    void setupTableItems();
    void reloadData();
    void loadCellMap();
    void updateEnabled(bool enabled);
    void openFile();
    void exploreFile();
private:
    bool bOnce=false;
    Ui::MainWindow *ui;
    class VList* vlist;
    class QStandardItemModel* tableModel;
    Processor* processor;
    QMap<QString,QStandardItem*> cellMap;
    class MovieDialog* dialog;
    class OptionDialog* optionDialog;
};
#endif // MAINWINDOW_H
