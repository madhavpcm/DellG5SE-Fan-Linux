#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->Exec, SIGNAL(clicked()),this, SLOT(on_auto_Exec_clicked()));
    connect(ui->exec2, SIGNAL(clicked()),this, SLOT(on_manual_Exec_clicked()));
    connect(this, SIGNAL(update_cpu(int)),ui->cpu_temp,SLOT(display(int)));
    connect(this, SIGNAL(update_gpu_fan(int)),ui->cpu_rpm,SLOT(display(int)));
    connect(this, SIGNAL(update_cpu(int)),ui->gpu_temp,SLOT(display(int)));
    connect(this, SIGNAL(update_gpu_fan(int)),ui->gpu_rpm,SLOT(display(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_auto_Exec_clicked()
{
    uint32_t lowtemp= ui->lowtemp_input->text().toUInt()*1000;
    uint32_t hightemp= ui->hightemp_input->text().toUInt()*1000;
    uint32_t timer= ui->timer_input->text().toUInt()*1000;

    if(!is_auto_mode_running){
        bool flag=true;
        if(!lowtemp){
            QMessageBox::warning(this, "Input Error" , "Please enter a valid lowtemp value.");
            flag=false;
            ui->lowtemp_input->setText("0");
        }else if(!hightemp){
            QMessageBox::warning(this, "Input Error" , "Please enter a valid hightemp value.");
            flag=false;
            ui->hightemp_input->setText("0");
        }else if(!timer){
            QMessageBox::information(this, "Default timer will be used" ,
                                     "Since no valid input for timer has been provided, the default value of <10> s is being used");
            flag=false;
            ui->timer_input->setText("10");
        }else if(hightemp <= lowtemp){
            QMessageBox::warning(this, "Input Error", "high temp should be greater than lowtemp");
            ui->hightemp_input->setText("0");
        }


        if(flag){
            //set values
            is_auto_mode_running=true;
            updateAutoExec();

            int i=5;
            while(is_auto_mode_running){
                ui->cpu_temp->display(i);

               // sleep(timer);
            }

        }else{



        }





  }else{
        //stop stuff, set values to zero
        is_auto_mode_running=false;
        updateAutoExec();


    }
   updateAutoExec();
}
void MainWindow::on_manual_Exec_clicked(){

}
void MainWindow::updateAutoExec(){
    if(is_auto_mode_running){
        ui->Exec->setText("<Stop>");
    }else{
        ui->Exec->setText("<Exec>");
    }
}
