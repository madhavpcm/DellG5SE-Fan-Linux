#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <unistd.h>
#include <filesystem>
#include <fcntl.h>
#include <QApplication>
#include <QMessageBox>

#define ECio "/sys/kernel/debug/ec/ec0/io"
#include "ui_mainwindow.h"

namespace fs=std::filesystem;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define ManualECMode_cpu 147 // 0x93
#define ManualECMode_gpu 150 // 0x96
#define GPUaddr 151 // 0x97
#define CPUaddr 148 //0x94
#define ZERO 255 // 0xFF
#define SLOW 240 // 0xF0
#define MEDIUM 200 // 204 -- 0xCC
#define NORMAL 163 // 0xA3
#define FAST 102 // 0x66
#define BOOST 91 // 0x5B
#define mou 2

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    int check_fan_write_permission();

    ~MainWindow();

private:
    std::atomic_bool is_manual = false;

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
    void manual_fan_mode(bool on);
    void write_to_ec(int byte_offset, uint8_t value);
    void set_cpu_fan(uint8_t left);

    void set_gpu_fan(uint8_t right);
    uint8_t hex_to_EC(uint8_t x);
private slots:
    //void on_auto_Exec_clicked();
    //void on_manual_Exec_clicked();

    void on_dial_cpu_valueChanged(int value);

    void on_dial_gpu_valueChanged(int value);

    void on_setButton_clicked();




    void on_resetButton_clicked();

signals:
    //void update_cpu(int);
    //void update_cpu_fan(int);
    //void update_gpu(int);
    //void update_gpu_fan(int);


};
#endif // MAINWINDOW_H
