#include "mainwindow.h"

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //check if sensors are addressable
    Hwmon_get();
    ui->setupUi(this);
    //sensors polling at 500ms
    QTimer* sens_probe = new QTimer(this);
    connect(sens_probe,SIGNAL(timeout()),this,SLOT(update_vars()) );
    sens_probe->start(500);


}


int MainWindow::check_fan_write_permission()
{
    if (getuid() != 0){
        QMessageBox::warning(this, "Error", "This program needs elevated permissions to run. Run as sudo");
        close();
    }
    // Checks if ec_sys is correctly loaded.
    try {
        fs::path f(ECio);
    }  catch (fs::filesystem_error) {
        QMessageBox::warning(this, "Error", "Cannot find ECio path. Try 'sudo modprobe ec_sys write_support=1' or add 'ec_sys.write_support=1' as kernel parameter if that's not working.");
        close();
    }
    return 1;

}
MainWindow::~MainWindow()
{
    if(is_manual)
        manual_fan_mode(false);
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

void MainWindow::on_cpu_val_valueChanged(int arg1)
{
    ui->dial_cpu->setValue((uint8_t)arg1);
}

void MainWindow::on_gpu_val_valueChanged(int arg1)
{
    ui->dial_gpu->setValue((uint8_t)arg1);
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
    return std::min(std::max(255-x,91 ),255);
};

void MainWindow::Hwmon_get()
{
    for (const auto &entry : fs::directory_iterator(hwmon))
      {
          std::ifstream namepath = std::ifstream(entry.path().string() + "/name");
          std::string name = std::string((std::istreambuf_iterator<char>(namepath)),
                                         (std::istreambuf_iterator<char>()));
          if (name == "dell_smm\n")
          {
              dellsmm = entry.path().string();
              for (const auto & file : fs::directory_iterator(dellsmm)){

                  std::ifstream a;
                  std::string file_path = file.path().string();
                  a.open(file_path);
                  std::string sensor_name;
                  a >> sensor_name;
                  a.close();

                  if (sensor_name == "GPU"){
                      GPU_path = file_path;
                      GPU_path.replace(GPU_path.length()-5,5,"input");
                  }
                  else if (sensor_name =="CPU")
                  {
                      CPU_path = file_path;
                      CPU_path.replace(CPU_path.length()-5,5,"input");
                  }
                  else if (sensor_name =="Processor")
                  {
                      CPU_fan_path = file_path;
                      CPU_fan_path.replace(CPU_fan_path.length()-5,5,"input");
                  }
                  else if (sensor_name =="Video")
                  {
                      GPU_fan_path = file_path;
                      GPU_fan_path.replace(GPU_fan_path.length()-5,5,"input");
                  }
              }
          }

      }
      if (dellsmm == ""){
          QMessageBox::warning(this, "Error", "It seems dell hardware monitor kernel module 'dell_smm_hwmon' has not been activated. Please try 'sudo modprobe dell_smm_hwmon' and run the program again");
          close();
      }
};

void MainWindow::update_vars(){
    std::ifstream a;
    a.open(CPU_path); //CPU dellsmm
    a >> cpu_temp;
    a.close();
    a.open(GPU_path); //GPU dellsmm
    a >> gpu_temp;
    a.close();
    a.open(CPU_fan_path); //Processor fan
    a >> cpu_fan;
    a.close();
    a.open(GPU_fan_path); // Video fan
    a >> gpu_fan;
    a.close();
    normalize();
    ui->cpu_temp->display(cpu_temp);
    ui->gpu_temp->display(gpu_temp);
    ui->cpu_rpm->display(cpu_fan);
    ui->gpu_rpm->display(gpu_fan);
}

void MainWindow::normalize(){
    cpu_temp/=1000;
    gpu_temp/=1000;
}
