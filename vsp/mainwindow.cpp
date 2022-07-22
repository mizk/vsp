#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optiondialog.h"
#include "vlist.h"
#include "globaldef.h"
#include "moviedialog.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QPixmap>
#include <QMessageBox>
#include <QThread>
#include <QStandardPaths>
#include <QFileDialog>
#include "processorthread.h"
#include <QDesktopServices>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initUI();
    this->initEvent();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete vlist;
    delete processor;
    delete tableModel;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if(!bOnce){//do only once
        vlist->load();
        this->reloadData();
        bOnce=true;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(processor->isWorking()){
        QMessageBox::StandardButton button=QMessageBox::question(this,tr("Tips"),tr("msg_quit_working"));
        if(button==QMessageBox::Yes){
            qApp->exit();
            return;
        }
        event->ignore();
        return;
    }
}

void MainWindow::onAdd()
{
    if(processor->isWorking()){
        return;
    }
    QString dir=QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    QStringList files=QFileDialog::getOpenFileNames(this,tr("choose_v_file"),dir,"Video(*.mp4 *.mkv *.webm *.mov *.avi *.wmv *.flv)");
    if(files.isEmpty()){
        return;
    }
    vlist->addAll(files);
    this->reloadData();
    vlist->save();
}

void MainWindow::onDel()
{
    if(vlist->isEmpty()){
        return;
    }
    QModelIndexList selectionList=ui->tableView->selectionModel()->selectedRows();
    if(!selectionList.isEmpty()){
        QVariant data=selectionList.first().data(Qt::UserRole+1);
        if(data.isValid()){
            QString file=data.toString();
            vlist->remove(file);
            ui->tableView->selectionModel()->clearSelection();
            this->reloadData();
            vlist->save();
        }
    }

}

void MainWindow::onGo()
{
    if(!hasApp(AppKind::FFMPEG)||!hasApp(AppKind::FFPROBE)){
        QMessageBox::critical(this,tr("Tips"),tr("msg_missing_components"));
        return;
    }
    if(vlist->isEmpty()){
        return;
    }
    vlist->save();
    dialog->show();
    dialog->move(x() + (width() - dialog->width()) / 2,
                 y() + (height() - dialog->height()) / 2);
}
void MainWindow::go(){

    loadCellMap();
    ui->progressBar->setValue(0);
    updateEnabled(false);
    int volume=dialog->getVolume();
    this->processor->join(this->vlist->data(),volume);
    ProcessorThread* thread=new ProcessorThread(nullptr,processor);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    connect(processor,&Processor::quit,thread,&QThread::quit);
    thread->start();
}
void MainWindow::onProcessorGoing(const QString &file, qint64 now, qint64 max)
{
    if(cellMap.contains(file)){
        QStandardItem* item=cellMap[file];
        item->setText(QString("%1/%2").arg(QString::number(now),QString::number(max)));
    }
}

void MainWindow::onProcessorProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::onProcessorQuit()
{
    ui->progressBar->setValue(0);
    updateEnabled(true);
    QMessageBox::information(this,tr("Tips"),tr("msg_done"));
}

void MainWindow::onProcessorNotify(const QString &file)
{
    if(cellMap.contains(file)){
        QStandardItem* item=cellMap[file];
        QString text=processor->getStateString(file);
        QString icon=processor->getStateIcon(file);
        QColor color(processor->getStateColor(file));
        item->setForeground(color);
        item->setText(text);
        QPixmap pixmap(icon);
        item->setIcon(pixmap);
    }
}

void MainWindow::playback(const QModelIndex &index)
{
    if(index.isValid()){
        QVariant file=index.model()->data(index,Qt::UserRole+1);
        QString f=file.toString();
        if(!f.isEmpty()){
            optionDialog->file=f;
            optionDialog->show();
            optionDialog->move(x() + (width() - optionDialog->width()) / 2,
                               y() + (height() - optionDialog->height()) / 2);
        }

    }
}

void MainWindow::openOption()
{
    int which=optionDialog->which();
    if(which==1){
        openFile();
    }else{
        exploreFile();
    }
}

void MainWindow::openFile()
{
    QString file=optionDialog->file;
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}
void MainWindow::exploreFile(){
    QString file=optionDialog->file;
    openWithSystemFileManager(file);
}
void MainWindow::initUI()
{
    vlist=new VList(this);
    processor=new Processor();
    this->setFixedSize(this->geometry().size());
    ui->progressBar->setVisible(false);
    tableModel=new QStandardItemModel(this);
    QStringList header;
    header<<tr("File")<<tr("State");
    tableModel->setHorizontalHeaderLabels(header);
    ui->tableView->setModel(tableModel);
    QHeaderView* headerView=ui->tableView->horizontalHeader();
    headerView->setSectionResizeMode(0,QHeaderView::Stretch);
    headerView->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->tableView->setColumnWidth(1,110);
    dialog=new MovieDialog(this);
    dialog->setModal(true);
    optionDialog=new OptionDialog(this);
    optionDialog->setModal(true);

}

void MainWindow::initEvent()
{
    connect(ui->addButton,&QPushButton::clicked,this,&MainWindow::onAdd);
    connect(ui->delButton,&QPushButton::clicked,this,&MainWindow::onDel);
    connect(ui->goButton,&QPushButton::clicked,this,&MainWindow::onGo);
    connect(processor,&Processor::ongoing,this,&MainWindow::onProcessorGoing);
    connect(processor,&Processor::quit,this,&MainWindow::onProcessorQuit);
    connect(processor,&Processor::progress,this,&MainWindow::onProcessorProgress);
    connect(processor,&Processor::notify,this,&MainWindow::onProcessorNotify);
    connect(dialog,&MovieDialog::accepted,this,&MainWindow::go);
    connect(ui->tableView,&QTableView::doubleClicked,this,&MainWindow::playback);
    connect(optionDialog,&OptionDialog::accepted,this,&MainWindow::openOption);

}

void MainWindow::setupTableItems(){
    const qsizetype required=vlist->size();
    const qsizetype rowCount=tableModel->rowCount();
    if(rowCount>required){
        tableModel->removeRows(required,rowCount-required);
    }else if(rowCount<required){
        int diff=required-rowCount;
        while(diff>0){
            diff--;
            QList<QStandardItem*> row;
            QStandardItem* nameItem=new QStandardItem();
            nameItem->setEditable(false);
            QStandardItem* statusItem=new QStandardItem();
            statusItem->setEditable(false);
            row<<nameItem<<statusItem;
            tableModel->appendRow(row);
        }
    }

}
void MainWindow::reloadData(){
    setupTableItems();
    const QStringList& dataList=vlist->data();
    const qsizetype max=dataList.size();
    for(int index=0;index<max;index++){
        const QString& file=dataList.at(index);
        QStandardItem* fileItem=tableModel->item(index,0);
        QFileInfo info(file);
        fileItem->setText(info.fileName());
        fileItem->setData(file);

        QStandardItem* statusItem=tableModel->item(index,1);
        statusItem->setData(file);
        QString image=processor->getStateIcon(file);
        QPixmap icon(image);
        statusItem->setIcon(icon);
        const QString stateString=processor->getStateString(file);
        statusItem->setText(stateString);
        QColor color(processor->getStateColor(file));
        statusItem->setForeground(color);

    }
    ui->tableView->setModel(tableModel);
}

void MainWindow::loadCellMap(){
    cellMap.clear();
    const int rowCount=tableModel->rowCount();
    if(rowCount>0){
        for(int index=0;index<rowCount;index++){
            QStandardItem* item=tableModel->item(index,1);
            if(item==nullptr){
                continue;
            }
            QString file=item->data(Qt::UserRole+1).toString();
            if(file.isNull()||file.isEmpty()){
                continue;
            }
            cellMap.insert(file,item);
        }
    }

}

void MainWindow::updateEnabled(bool enabled)
{
    ui->addButton->setEnabled(enabled);
    ui->delButton->setEnabled(enabled);
    ui->progressBar->setVisible(!enabled);
    ui->goButton->setEnabled(enabled);


}
