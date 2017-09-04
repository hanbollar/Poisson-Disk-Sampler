#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <iostream>
#include "cameracontrolshelp.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

//-HB from here:
void MainWindow::on_actionLoad_My_Object_triggered() {
    // below is used to get the file path to a specified directory and when an item is clicked in that directory
    // the file path specifically to that file is returned.
    QString file = QFileDialog::getOpenFileName(this,
             tr("Opening one of Hannah's OBJ Files"), "./../../_Code/poissonSampler/_objFiles/myOBJ", tr("OBJ Files (*.obj)"));
    std::cout<<file.toStdString()<<std::endl;

    emit sig_filePathString(file);
}

void MainWindow::on_actionLoad_Given_Object_triggered() {
    QString file = QFileDialog::getOpenFileName(this,
             tr("Opening one of the given c560 OBJ File"), "./../../_Code/poissonSampler/_objFiles/givenOBJ", tr("OBJ Files (*.obj)"));
    std::cout<<file.toStdString()<<std::endl;

    emit sig_filePathString(file);
}
//-HB to here.
