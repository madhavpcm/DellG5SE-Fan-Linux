#ifndef TASK_H
#define TASK_H

#include <QObject>

class Task : public QObject{
    Q_OBJECT
public:
    Task(QCoreApplication *a ):QObject(a->instance()){}
public slots:
    void run(){
        if(timer <=0 ){
                // In that case, we assume the user only wants to set fans once, and exit.
                const int left = std::min(std::max(lowtemp/1000,0),255);
                const int right = std::min(std::max(hightemp/1000,0),255);
                set_cpu_fan(left);
                set_gpu_fan(right);
                std::cout << "Set fans to " << left << " and " << right << "."<< std::endl;
                emit finished();
            }

            // Fan update loop
            while (true)
            {
                //First update the variables.
                update_vars();
                //Then update the fan speed accordingly.
                update_fans(lowtemp,hightemp);
                //Prints current status
                print_status();
                // wait $timer seconds
                sleep(timer);
            }
        emit finished();
    }
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
