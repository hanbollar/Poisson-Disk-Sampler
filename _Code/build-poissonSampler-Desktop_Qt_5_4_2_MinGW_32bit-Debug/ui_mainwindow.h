/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>
#include "mygl.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionCamera_Controls;
    QAction *actionLoad_My_Object;
    QAction *actionLoad_Given_Object;
    QWidget *centralWidget;
    MyGL *mygl;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuLoad_Object;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(610, 552);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionCamera_Controls = new QAction(MainWindow);
        actionCamera_Controls->setObjectName(QStringLiteral("actionCamera_Controls"));
        actionLoad_My_Object = new QAction(MainWindow);
        actionLoad_My_Object->setObjectName(QStringLiteral("actionLoad_My_Object"));
        actionLoad_Given_Object = new QAction(MainWindow);
        actionLoad_Given_Object->setObjectName(QStringLiteral("actionLoad_Given_Object"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        mygl = new MyGL(centralWidget);
        mygl->setObjectName(QStringLiteral("mygl"));
        mygl->setGeometry(QRect(10, 10, 581, 501));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 610, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuLoad_Object = new QMenu(menuBar);
        menuLoad_Object->setObjectName(QStringLiteral("menuLoad_Object"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuBar->addAction(menuLoad_Object->menuAction());
        menuFile->addAction(actionQuit);
        menuHelp->addAction(actionCamera_Controls);
        menuLoad_Object->addAction(actionLoad_My_Object);
        menuLoad_Object->addAction(actionLoad_Given_Object);

        retranslateUi(MainWindow);
        QObject::connect(MainWindow, SIGNAL(sig_filePathString(QString)), mygl, SLOT(slot_loadFileData(QString)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "OpenGLDemo", 0));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0));
        actionCamera_Controls->setText(QApplication::translate("MainWindow", "Camera Controls", 0));
        actionLoad_My_Object->setText(QApplication::translate("MainWindow", "My Object", 0));
        actionLoad_Given_Object->setText(QApplication::translate("MainWindow", "Given Object", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
        menuLoad_Object->setTitle(QApplication::translate("MainWindow", "Load Object", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
