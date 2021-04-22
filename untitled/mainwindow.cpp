#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_Exec_clicked()
{
    uint32_t lowtemp= ui->lowtemp_input->text().toUInt()*1000;
    uint32_t hightemp= ui->hightemp_input->text().toUInt()*1000;
    uint32_t timer= ui->timer_input->text().toUInt()*1000;
    bool flag=true;
    if(!lowtemp){
        QMessageBox::warning(this, "Input Error" , "Please enter a valid lowtemp value.");
        flag=false;
    }else if(!hightemp){
        QMessageBox::warning(this, "Input Error" , "Please enter a valid hightemp value.");
        flag=false;
    }else if(!timer){
        QMessageBox::information(this, "Default timer will be used" ,
                                 "Since no input for timer has been provided, the default value of <10> s is being used");
        flag=false;
    }else if(hightemp <= lowtemp){
        QMessageBox::warning(this, "Input Error", "high temp should be greater than lowtemp");
    }


    if(flag){

    }else{

    }



    ui->lowtemp_input->setText(QString("0"));
    ui->hightemp_input->setText(QString("0"));
    ui->timer_input->setText(QString("0"));

}
