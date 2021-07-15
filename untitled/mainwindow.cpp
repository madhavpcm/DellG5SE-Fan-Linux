#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    check_fan_write_permission();
    //manual_fan_mode(true);

    ui->setupUi(this);

}

void MainWindow::manual_fan_mode(bool on)
{
    if(on){
        write_to_ec(ManualECMode_cpu, 255);
        write_to_ec(ManualECMode_gpu, 255);
        QMessageBox::information(this,"Fan status","Fans are now in Manual mode");
    }
    else
    {
        write_to_ec(ManualECMode_cpu, 4);
        write_to_ec(ManualECMode_gpu, 4);
        QMessageBox::information(this, "Fan status", "Fans are now under BIOS control");
    }

}

void MainWindow::check_fan_write_permission()
{
    if (getuid() != 0){
        QMessageBox::warning(this, "Error", "This program needs elevated permissions to run. Run as sudo");
        QApplication::quit();
    }
    // Checks if ec_sys is correctly loaded.
    fs::path f(ECio);
    if(!fs::exists(f)){
        QMessageBox::warning(this, "Error", "EC I/O address not found. Make sure ec_sys module is loaded correctly. Try sudo modprobe ec_sys write_support=1 or add these options to the required conf files ");
        QApplication::quit();
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_dial_cpu_valueChanged(int value)
{
    uint8_t fan = value;
    //set_cpu_fan(value);
    ui->cpu_rpm->display(value);
}


void MainWindow::on_dial_gpu_valueChanged(int value)
{
    uint8_t fan=value;
    //set_gpu_fan(value);
    ui->gpu_rpm->display(value);
}
*/
