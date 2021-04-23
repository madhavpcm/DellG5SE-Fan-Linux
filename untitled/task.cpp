#include "task.h"

void Task::run(){
    QStringList argv=QCoreApplication::arguments();
    uint32_t argc= argv.size();
    if (argc <= 2)
    {
        printf("Need more arguments.\n");
        printf("Usage : DellFan -lowtemp # -hightemp # <optional> -timer #\n");
        printf("or, if you only want to set speed once : DellFan leftspeed rightspeed -1\n");
        exit(EXIT_FAILURE);
    }
    int lowtemp =0;
    int hightemp =0;
    int timer =10;

    bool b_ltemp=false,b_htemp=false,b_timer=false;
    for(uint32_t i=1 ; i<argc ; i++){
        if(argv[i] == "-lowtemp"){
            if(argv[i+1].toInt()){
                lowtemp=argv[i+1].toInt()*1000;
                b_ltemp=true;
            }
        }
        if(argv[i] == "-hightemp"){
            if(argv[i+1].toInt()){
                lowtemp=argv[i+1].toInt()*1000;
                b_htemp=true;
            }
        }
        if(argv[i] == "-timer"){
            if(argv[i+1].toInt()){
                timer=argv[i+1].toInt();
                b_timer=true;
            }
        }
    }

    if(!b_ltemp || !b_htemp){
        std::cout<<"Invalid input for hightemp/lowtemp or missing hightemp/lowtemp arguments \n";
        exit(EXIT_FAILURE);
    }
    std::cout<<"here"<<'\n';

    // std::cout << "Script launched with arguments : " << lowtemp/1000 << " " << hightemp/1000 << " " << timer <<std::endl; //Debug

    // Get hwmon variables
    Hwmon_get();
    // Check if launched with enough permissions.
    check_fan_write_permission();

    if(!b_timer )
    {
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


 // either k10temp or zenpower

//  Gets the Hwmon ids of dellsmm, k10temp/zenpower, and dGPU.
void Task::Hwmon_get()
{
    // std::string dellsmm_path = "";
    // std::string cpu_path = "";
    for (const auto &entry : fs::directory_iterator(hwmon))
    {
        std::ifstream namepath = std::ifstream(entry.path().string() + "/name");
        std::string name = std::string((std::istreambuf_iterator<char>(namepath)),
                                       (std::istreambuf_iterator<char>()));
        if (name == "dell_smm\n")
        {
            // dellsmm_path = entry.path().string().back();
            dellsmm = entry.path().string();
        }
        else if (name == "zenpower\n")
        {
            // cpu_path = entry.path().string().back();
            CPU = entry.path().string();
        }
        else if (name == "k10temp\n")
        {
            // cpu_path = entry.path().string().back();
            CPU = entry.path().string();
        }
        else if (name == "amdgpu\n")
        {
            // There are two amd gpus (iGPU and dGPU) an easy way to differentiate them is to check the presence of pwm1. Another way would be to check vbios version (may be more robust).
            if (fs::exists(entry.path().string() + "/pwm1"))
            {
                dGPU = entry.path().string();
            }
        }
    }
};

// Updates the thermals and fan variables.
void Task::update_vars()
{
    std::ifstream a;
    a.open(CPU + "/temp2_input"); //Tdie cpu temp
    a >> cpu_temp;
    a.close();
    a.open(dGPU + "/temp2_input"); //Junction dGPU temp
    a >> gpu_temp;
    a.close();
    a.open(dellsmm + "/fan1_input"); //Processor fan
    a >> cpu_fan;
    a.close();
    a.open(dellsmm + "/fan3_input"); // Video fan
    a >> gpu_fan;
    a.close();
};

// Set cpu fans to selected speed. Input should be in the set {0,128,256}.
void Task::set_cpu_fan(int left)
{
    // Force left to be in [0,256]
    int l = std::max(0, std::min(255, left));
    // Writes to hwmon
    std::ofstream pwm;
    pwm.open(dellsmm + "/pwm1");
    pwm << l;
    pwm.close();
};
// Set gpu fans to selected speed. Input should be in the set {0,128,256}.
void Task::set_gpu_fan(int right)
{
    // Force right to be in [0,256]
    int r = std::max(0, std::min(255, right));
    // Writes to hwmon
    std::ofstream pwm;
    pwm.open(dellsmm + "/pwm3");
    pwm << r;
    pwm.close();
};


void Task::write_to_ec(int byte_offset, uint8_t value){
    int fd = open(ECio, O_WRONLY);
    int error;

    error = lseek(fd, byte_offset, SEEK_SET);
    if (error != byte_offset)
        err(EXIT_FAILURE, "Cannot set offset to 0x%.2x", byte_offset);

    error = write(fd, &value, 1);
    if (error != 1)
        err(EXIT_FAILURE, "Cannot write value 0x%.2x to offset 0x%.2x",
            value, byte_offset);
}

void Task::check_fan_write_permission()
{
    std::ofstream pwm;
    pwm.open(dellsmm + "/pwm1");
    if (!pwm.is_open())
    {
        std::cout << "Cannot change fan speed. Are you running the script with root permission ?" << std::endl;
        exit(EXIT_FAILURE);
    }
    pwm.close();
};

// Updates fans accordings to temp.
void Task::update_fans(int lowtemp, int hightemp)
{
    // int fan_update[2] = {-1, -1}; //To debug
    // Handle the left (cpu) fan
    if (cpu_temp < lowtemp)
    {
        if (cpu_fan > 2500 || cpu_fan <1500)
        {
            // set_cpu_fan(0);
            // new_fan_values[0] = 0;
            if (cpu_fan <1500){
                write_to_ec(CPUaddr,SLOW);
            }
            else{
                set_cpu_fan(0);
            }
        }
    }
    else if (cpu_temp < hightemp)
    {
        if (cpu_fan <= 1900 || cpu_fan >= 3500)
        {
            set_cpu_fan(128);
            // new_fan_values[0] = 128
        }
    }
    else
    {
        if (cpu_fan < 3500)
        {
            set_cpu_fan(255);
            // new_fan_values[0] = 255;
        }
    }
    // Handles the right (GPU) fan
    if (gpu_temp < lowtemp)
    {
        if (gpu_fan > 1900 || gpu_fan < 1500)
        {
            // if (cpu_fan >0 || gpu_fan <1500){
            //     write_to_ec(GPUaddr,SLOW);
            // }
            // else{
            //     set_gpu_fan(0);
            //     write_to_ec(GPUaddr,SLOW);
            // }
            write_to_ec(GPUaddr,SLOW);
            // new_fan_values[1] = 0;
        }
    }
    else if (gpu_temp < hightemp)
    {
        if (gpu_fan <= 1900)
        {
            set_gpu_fan(128);
        }
        else if (gpu_fan >= 2900)
        {
            write_to_ec(GPUaddr,MEDIUM);
        }
    }
    else
    {
        if (gpu_fan < 3500)
        {
            set_gpu_fan(255);
            // write_to_ec(GPUaddr,FAST);
            // new_fan_values[1] = 255;
        }
    }
};

void Task::print_status()
{
    std::cout << "Current fan speeds : " << cpu_fan << " RPM and " << gpu_fan << " RPM.      " << std::endl;
    std::cout << "CPU and GPU temperatures : " << cpu_temp/1000 << "°C and " << gpu_temp/1000 << "°C.  " << std::endl;
    std::cout << "\033[2F";
};



