#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <unistd.h>
#include <filesystem>
#include <fcntl.h>
#include <QApplication>
#include <QMessageBox>
#include <fstream>
#define ECio "/sys/kernel/debug/ec/ec0/io"
#include "ui_mainwindow.h"
#include <QTimer>
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
    std::atomic_bool is_auto= false;
    Ui::MainWindow *ui;
    QTimer* m_fan_loop;
    QTimer* m_sens_probe;
    int cpu_temp;
    int gpu_temp;
    int cpu_fan;
    int gpu_fan;
    u_int32_t m_poll_interval;
    std::string GPU_path;
    std::string CPU_path;
    std::string CPU_fan_path;
    std::string GPU_fan_path;

    std::string dellsmm="";
    const std::string hwmon = "/sys/class/hwmon";
    void loop_break();
    void manual_fan_mode(bool on);
    void Hwmon_get();
    void write_to_ec(int byte_offset, uint8_t value);
    void set_cpu_fan(uint8_t left);
    void normalize();
    void set_gpu_fan(uint8_t right);
    uint8_t hex_to_EC(uint8_t x);
    void curve_update();
private slots:
    //void on_auto_Exec_clicked();
    //void on_manual_Exec_clicked();

    void on_dial_cpu_valueChanged(int value);

    void on_dial_gpu_valueChanged(int value);

    void on_setButton_clicked();

    void update_vars();


    void on_resetButton_clicked();

    void on_cpu_val_valueChanged(int arg1);

    void on_gpu_val_valueChanged(int arg1);

    void on_auto_mode_clicked();

    void on_set_mode_clicked();

    void on_ec_edit_clicked();

    void on_Enable_clicked();

    void enable_fan_loop();
signals:
    //void update_cpu(int);
    //void update_cpu_fan(int);
    //void update_gpu(int);
    //void update_gpu_fan(int);


};
#endif // MAINWINDOW_H
