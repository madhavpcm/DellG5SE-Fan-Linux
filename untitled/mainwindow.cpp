#include "mainwindow.h"

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //check_fan_write_permission();

    //manual_fan_mode(true);

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

void MainWindow::set_cpu_fan(uint8_t left)
{
    // Force left to be in [0,256]
//    int l = std::max(0, std::min(255, left));
    // Writes to hwmon
   // std::ofstream pwm;
  //  pwm.open(dellsmm + "/pwm1");
    //pwm << l;
    //pwm.close();
    write_to_ec(CPUaddr, left);
};
// Set gpu fans to selected speed. Input should be in the set {0,128,256}.
void MainWindow::set_gpu_fan(uint8_t right)
{
    // Force right to be in [0,256]
    //int r = std::max(0, std::min(255, right));
    //// Writes to hwmon
    //std::ofstream pwm;
    //pwm.open(dellsmm + "/pwm3");
    //pwm << r;
    //pwm.close();
    write_to_ec(GPUaddr,right);
};

void MainWindow::on_dial_cpu_valueChanged(int value)
{
    uint8_t fan = value;
    //set_cpu_fan(value);
    ui->cpu_val->setValue(fan);
}
void MainWindow::on_dial_gpu_valueChanged(int value)
{
    uint8_t fan=value;
    //set_gpu_fan(value);
    ui->gpu_val->setValue(fan);
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


void MainWindow::on_setButton_clicked()
{
    if(!is_manual){
        is_manual=true;
        manual_fan_mode(true);

    }
    uint8_t c_target= ui->cpu_val->value(),
            g_target= ui->gpu_val->value();
    set_cpu_fan(hex_to_EC(c_target));
    set_gpu_fan(hex_to_EC(g_target));


}
void MainWindow::manual_fan_mode(bool on)
{
    if(on){
        write_to_ec(ManualECMode_cpu, 255);
        write_to_ec(ManualECMode_gpu, 255);
        QMessageBox::warning(this,"Fan Mode", "Manual mode enabled");
    }
    else
    {
        write_to_ec(ManualECMode_cpu, 4);
        write_to_ec(ManualECMode_gpu, 4);
        QMessageBox::warning(this,"Fan Mode", "Returned to BIOS fan control");
    }

}

void MainWindow::on_resetButton_clicked()
{
    if(is_manual){
        is_manual=false;
        manual_fan_mode(false);
    }
}
uint8_t MainWindow::hex_to_EC(uint8_t x){
    return std::min(std::max(255-x, 91),255);
};
