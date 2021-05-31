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
#include <thread>
#include <signal.h>
// #include <stdexcept> //error handling
// #include <gtk/gtk.h> // gtk interface


#include <err.h>
#include <fcntl.h>
#include <unistd.h>

namespace fs = std::filesystem;

#define ECio "/sys/kernel/debug/ec/ec0/io"
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

class Task : public QObject{
    Q_OBJECT
public:
    Task(QCoreApplication *a ):QObject(a->instance()){}
public slots:
    void run();
signals:
    void finished();
private:
    uint32_t cpu_temp;
    uint32_t gpu_temp;
    uint32_t cpu_fan;
    uint32_t gpu_fan;

    uint8_t t1;
    uint8_t t2;
    uint8_t t3;
    uint8_t t4=0;

    std::string GPU_path;
    std::string CPU_path;
    std::string CPU_fan_path;
    std::string GPU_fan_path; 
    std::string dellsmm="";
    const std::string hwmon = "/sys/class/hwmon";
    std::atomic_bool stop=false;
    bool verbose= false;

    int fan_curve(uint8_t current_temp, uint32_t current_fan);
    uint8_t hex_to_EC(uint8_t hex);
    static void exit_handler(int x);
    static void write_to_ec(int byte_offset, uint8_t value);
    static void manual_fan_mode(bool on);

    void Hwmon_get();
    void update_vars();
    void set_cpu_fan(int left);
    void set_gpu_fan(int right);
    void check_fan_write_permission();
   // void update_fans(int lowtemp, int hightemp);
    void update_fans();
    void print_status();
    void loop_break();
    void usage(std::string prog_name, int status);


};

#endif // TASK_H
