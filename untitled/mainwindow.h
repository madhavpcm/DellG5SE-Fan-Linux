#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <unistd.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::atomic_bool is_auto_mode_running = false;

    Ui::MainWindow *ui;

    int cpu_temp;
    int gpu_temp;
    int cpu_fan;
    int gpu_fan;

    std::string GPU_path;
    std::string CPU_path;
    std::string CPU_fan_path;
    std::string GPU_fan_path;

    std::string dellsmm="";
    const std::string hwmon = "/sys/class/hwmon";
    void loop_break();

private slots:
    void on_auto_Exec_clicked();
    void on_manual_Exec_clicked();
    void updateAutoExec();

signals:
    void update_cpu(int);
    void update_cpu_fan(int);
    void update_gpu(int);
    void update_gpu_fan(int);


};
#endif // MAINWINDOW_H
