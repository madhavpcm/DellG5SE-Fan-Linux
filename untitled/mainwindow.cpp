#include "mainwindow.h"

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_fan_loop(new QTimer(this))
    , m_sens_probe(new QTimer(this))
{
    //check if sensors are addressable
    m_poll_interval = 300;
    Hwmon_get();
    ui->setupUi(this);
    init_graph();
    //sensors polling at 500ms

    connect(m_sens_probe,SIGNAL(timeout()),this,SLOT(update_vars()) );
    connect(m_fan_loop,SIGNAL(timeout()),this,SLOT(fan_loop()) );
    m_sens_probe->start(m_poll_interval);


    connect(ui->actionAuto,SIGNAL(triggered()),this,SLOT(on_auto_mode_clicked()));
    connect(ui->actionSet,SIGNAL(triggered()),this,SLOT(on_set_mode_clicked()));
}

void MainWindow::init_graph(){


    m_curve = new QCPCurve(ui->curve->xAxis, ui->curve->yAxis);
    m_curve->setName("Fan curve") ;
    ui->curve->xAxis->setRange(0,100);
    ui->curve->yAxis->setRange(0,6400);
    ui->curve->yAxis->setLabel("Fan RPM");
    ui->curve->xAxis->setLabel("Temp °C");
    ui->curve->setOpenGl(true, 8);
    QColor bgColor("#140403");
    QColor txtColor("#eeeeee");
    QColor gColor("#rr0000");

    QPen graphPen;
    graphPen.setWidth(2);
    graphPen.setColor(gColor);
    QPen tickPen;
    tickPen.setWidthF(0.5);
    tickPen.setColor(gColor);

    // Define the filler line vectors and graphs so they don't need to be recreated every update
    ui->curve->addGraph();
    ui->curve->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->curve->graph(0)->setData(m_pointsX,m_pointsY);
    ui->curve->graph(0)->setPen(graphPen);
    ui->curve->graph(0)->setBrush(QBrush(QColor(255,0,0,29)));
    ui->curve->graph(0)->setLineStyle(QCPGraph::lsNone);

    ui->curve->setBackground(bgColor);
    ui->curve->xAxis->setLabelColor(txtColor);
    ui->curve->yAxis->setLabelColor(txtColor);
    ui->curve->xAxis->setTickLabelColor(txtColor);
    ui->curve->yAxis->setTickLabelColor(txtColor);

    ui->curve->xAxis->setTickPen(tickPen);
    ui->curve->yAxis->setTickPen(tickPen);
    ui->curve->xAxis->setSubTickPen(tickPen);
    ui->curve->yAxis->setSubTickPen(tickPen);
    ui->curve->xAxis->setBasePen(tickPen);
    ui->curve->yAxis->setBasePen(tickPen);;


    connect(ui->curve, SIGNAL(mouseDoubleClick(QMouseEvent* )), this, SLOT(add_point(QMouseEvent*)));
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

void MainWindow::on_auto_mode_clicked(){
    ui->stackedWidget->setCurrentIndex(1) ;
}
void MainWindow::on_set_mode_clicked(){
    ui->stackedWidget->setCurrentIndex(0) ;
}
void MainWindow::on_ec_edit_clicked(){

}



void MainWindow::on_enable_pressed()
{
    if(!is_manual)
        manual_fan_mode(true);
    if(!is_auto)
        is_auto = true;

    m_sens_probe->stop();
    m_sens_probe->start(m_poll_interval);
    m_fan_loop->start(m_poll_interval);
}
void MainWindow::fan_loop(){
//calculations of fan rpm vs temp

}

void MainWindow::add_point(QMouseEvent* e){
    if(e->button() == Qt::LeftButton) {
        m_pointsX.push_back(e->pos().x());
        m_pointsY.push_back(e->pos().y());
    }
    draw_graph(0);
}
void MainWindow::draw_graph(uint32_t i){
    ui->curve->graph(i)->setData(m_pointsX,m_pointsY);
    ui->curve->replot();
    ui->curve->update();
}




void MainWindow::on_disable_pressed()
{
    if(is_auto) {
        is_auto = false;
        m_sens_probe->stop();
        m_fan_loop->stop();
    }
}

