#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();
    void on_actionCamera_Controls_triggered();

    void on_actionLoad_My_Object_triggered(); //-HB
    void on_actionLoad_Given_Object_triggered(); //-HB

private:
    Ui::MainWindow *ui;

signals:
    void sig_filePathString(QString filePath);
};


#endif // MAINWINDOW_H
