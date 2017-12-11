#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mgraphics.h"
#include "worker.h"

#include <QDebug>
#include <QThread>
#include <QString>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QAction>
#include <QMenu>
#include <QDate>
#include <QTime>
#include <QMessageBox>
#include <QKeyEvent>

const qreal zoomMultiple = 1.05;
const QString Data_dir = QString("DATA/DATA ")
        + QDate::currentDate().toString(QString("dd_MM_yyyy")) + QString(" ")
            + QTime::currentTime().toString(QString("hh_mm_ss"));

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    View = new MGraphics;
    ui->gridLayout->addWidget(View);
    connect(this,&MainWindow::send_filePath,
            View,&MGraphics::load_from_file);
    connect(this,&MainWindow::send_Cursor_mode,
            View,&MGraphics::setCursor_mode);
    connect(this,&MainWindow::send_Obj_color,
            View,&MGraphics::ObjectsColorChange);
    connect(this,&MainWindow::send_colorize_obj,
            View,&MGraphics::RandomColorize);
    connect(this,&MainWindow::send_thickness,
            View,&MGraphics::setThickness);
}

MainWindow::~MainWindow()
{
    delete View;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() && e->modifiers() & Qt::ControlModifier)
    {
        switch (e->key()) {
        case Qt::Key_O: emit send_filePath(getFileName()); break;
        case Qt::Key_T: View->autoThreshold(); break;
        case Qt::Key_R: MainWindow::on_actionStart_algo_triggered(); break;
        case Qt::Key_A: emit send_colorize_obj(); break;
        case Qt::Key_S: MainWindow::on_action5_triggered(); break;
        }
    }
}

void MainWindow::on_actionLoad_file_triggered()
{//Signal from ui to load image
    emit send_filePath(getFileName());
}

void MainWindow::on_actionStart_algo_triggered()
{
// if image exist
// Create new thread and new Worker
// Worker is class which forms vector of objects from bin image
// all is safe and auto self-destroying
     if (View->get_bin_img().isNull())
     {
         QMessageBox::information(nullptr,
          QObject::tr("Сообщение"),tr("Бинаризуйте изображение..."));
         return;
     }

     View->setthread_ON_WORK(true);
     QThread* thread = new QThread();
     Worker*    task = new Worker(View->get_bin_img());
//--------------------------------------------------------------------
     task -> moveToThread(thread);
//--------------------------------------------------------------------
     View->setEnabled(false);

     connect(thread,&QThread::started,
             task, &Worker::doWork,Qt::DirectConnection);
     connect(task,&Worker::workFinished,
             thread,&QThread::quit,Qt::DirectConnection);
     connect(task,&Worker::workFinished,
             this,[=](){
         View->setEnabled(true);
     });
     //automatically delete thread and task object when work is done:
     connect(thread,&QThread::finished,
             task,&Worker::deleteLater,Qt::DirectConnection);
     connect(thread,&QThread::finished,
             thread,&QThread::deleteLater,Qt::DirectConnection);
     connect(thread,&QThread::finished,this,[=](){
         View->setthread_ON_WORK(false);
     });
//--------------------------------------------------------------------
     thread->start();
}

void MainWindow::on_action1_triggered()
{//zoom in button
  View->scale(zoomMultiple,zoomMultiple);
}

void MainWindow::on_action2_triggered()
{//zoom out button
  View->scale(1/zoomMultiple,1/zoomMultiple);
}

void MainWindow::on_action3_triggered()
{//draw tool button
 //with simple logic
 //Only one button (draw or erase) can be active
 //emit signal to Graphics class
    bool isActive = ui->action3->isChecked();
    ui->action3->setChecked(isActive);

    if (ui->action4->isChecked() && isActive)
        ui->action4->setChecked(false);

    char mode = isActive ? 1 : 0;
    emit send_Cursor_mode(mode);
}

void MainWindow::on_action4_triggered()
{//similarly

    bool isActive = ui->action4->isChecked();
    ui->action4->setChecked(isActive);
    if (ui->action3->isChecked() && isActive)
        ui->action3->setChecked(false);

    char mode = isActive ? 2 : 0;
    emit send_Cursor_mode(mode);
}

void MainWindow::on_actionColor_of_objects_triggered()
{//emit signal to MGraphics with new color of objects

    QColor col = QColorDialog::getColor
            (Qt::white,this,"Choose the objects color");
    if (col.isValid())
    emit send_Obj_color(col);
}

void MainWindow::on_actionColorize_all_triggered()
{//just send signal to MGraphics colorize All
 //objects and contours by random colors
    emit send_colorize_obj();
}

bool txtIsValid(const QString& str)
{//predicate which allow strings only in
 //this format 123px or 123 (suffix)
    QString s = str;
    if (s.endsWith(QString("px")))
        s.resize(s.size() - 2);
    for (int i = 0; i < s.size(); ++i)
    {
        if (!s[i].isDigit()) return false;
    }
    return true;
}

QString thickness_default = "2px";
void MainWindow::on_lineEdit_selectionChanged()
{
    ui->lineEdit->setText(QString());
}

void MainWindow::on_lineEdit_editingFinished()
{//setter of pen thickness
 //without px suffix
    QString txt = ui->lineEdit->text();
    if (txtIsValid(txt))
    {
        if (!txt.endsWith(QString("px")))
            txt += QString("px");
    }else{
        txt = thickness_default;
    }
    ui->lineEdit->setText(txt);
    txt.resize(txt.size() - 2);
    emit send_thickness(txt.toInt());
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{//setter of pen thickness
    ui->lineEdit->setText(QString::number(value) + QString("px"));
    emit send_thickness(value);
}

void MainWindow::on_commandLinkButton_clicked()
{//backward drawing/erase on scene actions
    View->backward();
}

void MainWindow::on_commandLinkButton_2_clicked()
{//forward drawing/erase on scene actions
    View->forward();
}

void MainWindow::on_actionauto_threshold_Breadley_Rot_triggered()
{//auto threshold by Breadley-Rot algo
    View->autoThreshold();
}

void MainWindow::on_action5_triggered()
{//save data

    if (!View->dataIsReady()) {
        QMessageBox::information(nullptr,QObject::tr("Сообщение"),tr("Объекты еще не выделены..."));
        return;
    }

 //--------------------------------------------------------------------
      QString path = QFileDialog::getSaveFileName
               (this,"SAVE DATA",Data_dir,"*.dat");
      if (path.isNull()) return;

      QThread* thread = new QThread();
      Worker*  SaveTask = new Worker(path);
 //--------------------------------------------------------------------
      SaveTask -> moveToThread(thread);
 //--------------------------------------------------------------------
      connect(thread,&QThread::started,
              SaveTask, &Worker::saveData,Qt::DirectConnection);
      connect(SaveTask,&Worker::dataIsSaved,
              thread,&QThread::quit,Qt::DirectConnection);

      //automatically delete thread and task object when work is done:
      connect(thread,&QThread::finished,
              SaveTask,&Worker::deleteLater,Qt::DirectConnection);
      connect(thread,&QThread::finished,
              thread,&QThread::deleteLater,Qt::DirectConnection);

 //--------------------------------------------------------------------
      thread->start();
}
