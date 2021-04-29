/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *lowtemp;
    QLabel *hightemp;
    QLabel *timer;
    QLineEdit *lowtemp_input;
    QLineEdit *hightemp_input;
    QLineEdit *timer_input;
    QPushButton *Exec;
    QLCDNumber *cpu_temp;
    QLCDNumber *cpu_rpm;
    QLCDNumber *gpu_rpm;
    QLCDNumber *gpu_temp;
    QLabel *temp;
    QLabel *rpm;
    QFrame *line;
    QFrame *line_2;
    QLabel *cpu;
    QLabel *gpu;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(358, 397);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lowtemp = new QLabel(centralwidget);
        lowtemp->setObjectName(QString::fromUtf8("lowtemp"));
        lowtemp->setGeometry(QRect(10, 20, 91, 20));
        hightemp = new QLabel(centralwidget);
        hightemp->setObjectName(QString::fromUtf8("hightemp"));
        hightemp->setGeometry(QRect(10, 70, 101, 20));
        timer = new QLabel(centralwidget);
        timer->setObjectName(QString::fromUtf8("timer"));
        timer->setGeometry(QRect(10, 120, 73, 17));
        lowtemp_input = new QLineEdit(centralwidget);
        lowtemp_input->setObjectName(QString::fromUtf8("lowtemp_input"));
        lowtemp_input->setGeometry(QRect(10, 40, 101, 25));
        hightemp_input = new QLineEdit(centralwidget);
        hightemp_input->setObjectName(QString::fromUtf8("hightemp_input"));
        hightemp_input->setGeometry(QRect(10, 90, 101, 25));
        timer_input = new QLineEdit(centralwidget);
        timer_input->setObjectName(QString::fromUtf8("timer_input"));
        timer_input->setGeometry(QRect(10, 140, 101, 25));
        Exec = new QPushButton(centralwidget);
        Exec->setObjectName(QString::fromUtf8("Exec"));
        Exec->setGeometry(QRect(10, 180, 101, 91));
        cpu_temp = new QLCDNumber(centralwidget);
        cpu_temp->setObjectName(QString::fromUtf8("cpu_temp"));
        cpu_temp->setGeometry(QRect(190, 80, 64, 51));
        cpu_rpm = new QLCDNumber(centralwidget);
        cpu_rpm->setObjectName(QString::fromUtf8("cpu_rpm"));
        cpu_rpm->setGeometry(QRect(270, 80, 64, 51));
        gpu_rpm = new QLCDNumber(centralwidget);
        gpu_rpm->setObjectName(QString::fromUtf8("gpu_rpm"));
        gpu_rpm->setGeometry(QRect(270, 140, 64, 51));
        gpu_temp = new QLCDNumber(centralwidget);
        gpu_temp->setObjectName(QString::fromUtf8("gpu_temp"));
        gpu_temp->setGeometry(QRect(190, 140, 64, 51));
        temp = new QLabel(centralwidget);
        temp->setObjectName(QString::fromUtf8("temp"));
        temp->setGeometry(QRect(190, 40, 73, 17));
        rpm = new QLabel(centralwidget);
        rpm->setObjectName(QString::fromUtf8("rpm"));
        rpm->setGeometry(QRect(270, 40, 73, 17));
        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(120, 30, 16, 241));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(17, 280, 321, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        cpu = new QLabel(centralwidget);
        cpu->setObjectName(QString::fromUtf8("cpu"));
        cpu->setGeometry(QRect(140, 90, 41, 31));
        gpu = new QLabel(centralwidget);
        gpu->setObjectName(QString::fromUtf8("gpu"));
        gpu->setGeometry(QRect(140, 150, 41, 31));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 358, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        lowtemp->setText(QCoreApplication::translate("MainWindow", "LowTemp(\302\260C)", nullptr));
        hightemp->setText(QCoreApplication::translate("MainWindow", "HighTemp(\302\260C)", nullptr));
        timer->setText(QCoreApplication::translate("MainWindow", "Timer(s)", nullptr));
        Exec->setText(QCoreApplication::translate("MainWindow", "<Exec>", nullptr));
        temp->setText(QCoreApplication::translate("MainWindow", "Temp(\302\260C)", nullptr));
        rpm->setText(QCoreApplication::translate("MainWindow", "Fans(RPM)", nullptr));
        cpu->setText(QCoreApplication::translate("MainWindow", "CPU", nullptr));
        gpu->setText(QCoreApplication::translate("MainWindow", "GPU", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
