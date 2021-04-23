#ifndef TASK_H
#define TASK_H

#include <QObject>
#include<QCoreApplication>


// #include "FindHwmon.h"
#include <stdio.h>
#include <iostream>
#include <string>
// #include <cstring>
#include <algorithm>  // std::max
#include <fstream>    //read  and write files
#include <filesystem> // file browsing
#include <unistd.h>   // sleep library
// #include <stdexcept> //error handling
// #include <gtk/gtk.h> // gtk interface


#include <err.h>
#include <fcntl.h>
#include <unistd.h>

namespace fs = std::filesystem;

#define ECio "/sys/kernel/debug/ec/ec0/io"
#define GPUaddr 151 // 0x97
#define CPUaddr 148 //0x94
#define ZERO 255 // 0xFF
#define SLOW 240 // 0xF0
#define MEDIUM 200 // 204 -- 0xCC
#define NORMAL 163 // 0xA3
#define FAST 102 // 0x66
#define BOOST 91 // 0x5B

class Task : public QObject{
    Q_OBJECT
public:
    Task(QCoreApplication *a ):QObject(a->instance()){}
public slots:
    void run();
signals:
    void finished();
private:
    int cpu_temp;
    int gpu_temp;
    int cpu_fan;
    int gpu_fan;
    std::string dellsmm;
    std::string dGPU;
    std::string CPU;
    const std::string hwmon = "/sys/class/hwmon";


    void Hwmon_get();
    void update_vars();
    void set_cpu_fan(int left);
    void set_gpu_fan(int right);
    void write_to_ec(int byte_offset, uint8_t value);
    void check_fan_write_permission();
    void update_fans(int lowtemp, int hightemp);
    void print_status();

};

#endif // TASK_H
