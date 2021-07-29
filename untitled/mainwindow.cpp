#include "mainwindow.h"
#include "ui_mainwindow.h"

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //check_fan_write_permission();

    //manual_fan_mode(true);

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

int MainWindow::check_fan_write_permission()
{
    if (getuid() != 0){
        QMessageBox::warning(this, "Error", "This program needs elevated permissions to run. Run as sudo");

        return 0;
    }
    // Checks if ec_sys is correctly loaded.
    try {
        fs::path f(ECio);
    }  catch (fs::filesystem_error) {
        QMessageBox::warning(this, "Error", "Cannot find ECio path. Try 'sudo modprobe ec_sys write_support=1' or add 'ec_sys.write_support=1' as kernel parameter if that's not working.");
        return 0;
    }
    return 1;

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
void MainWindow::write_to_ec(int byte_offset, uint8_t value){
    int fd = open(ECio, O_WRONLY);
    int error;

    /*error = lseek(fd, byte_offset, SEEK_SET);
    if (error != byte_offset)
        err(EXIT_FAILURE, "Cannot set offset to 0x%.2x", byte_offset);

    error = write(fd, &value, 1);
    if (error != 1)
        err(EXIT_FAILURE, "Cannot write value 0x%.2x to offset 0x%.2x",
            value, byte_offset);*/
    error = lseek(fd, byte_offset, SEEK_SET);
    if (error == byte_offset)
        write(fd, &value, 1);
    else
        QMessageBox::warning(this,"title","Error Writing to EC");
}
