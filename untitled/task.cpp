#include "task.h"
void Task::loop_break(){
    char s;
    while(true){
        s=getchar();
        if(s=='\n' || s==EOF)
            break;
    }
    stop=true;
    return;

}
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
                hightemp=argv[i+1].toInt()*1000;
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
    std::thread t(&Task::loop_break, this);
        // Fan update loop
    while (!stop)
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
    t.join();
    set_cpu_fan(0);
    set_gpu_fan(0);

    emit finished();
}


 // either k10temp or zenpower

//  Gets the Hwmon ids of dellsmm, k10temp/zenpower, and dGPU.
void Task::Hwmon_get()
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
        // else if (name == "zenpower\n")
        // {
        //     CPU = entry.path().string();
        // }
        // else if (name == "k10temp\n")
        // {
        //     CPU = entry.path().string();
        // }
        // else if (name == "amdgpu\n")
        // {
        //     // There are two amd gpus (iGPU and dGPU) an easy way to differentiate them is to check the presence of pwm1. Another way would be to check vbios version (may be more robust).
        //     if (fs::exists(entry.path().string() + "/pwm1"))
        //     {
        //         dGPU = entry.path().string();
        //     }
        // }
    }
    if (dellsmm == ""){
        std::cout << "Cannot find Dell-smm-hwmon. Try 'modprobe dell-smm-hwmon restricted=0 ignore_dmi=1'. " << std::endl;
        exit(EXIT_FAILURE);
    }
};

// Updates the thermals and fan variables.
void Task::update_vars()
{
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
    // Handle the left (cpu) fan
    if (cpu_temp < lowtemp)
    {
        if (cpu_fan > 2500 || cpu_fan <1500)
        {
            set_cpu_fan(0);
            // write_to_ec(CPUaddr,SLOW); // Not working...
        }
    }
    else if (cpu_temp < hightemp)
    {
        if (cpu_fan <= 1900 || cpu_fan >= 3500)
        {
            set_cpu_fan(128);
        }
    }
    else
    {
        if (cpu_fan < 3500)
        {
            set_cpu_fan(255);
        }
    }
    // Handles the right (GPU) fan
    if (gpu_temp < lowtemp)
    {
        if (gpu_fan > 1900 || gpu_fan < 1500)
        {
            write_to_ec(GPUaddr,SLOW);
        }
    }
    else if (gpu_temp < hightemp)
    {
        
        if (gpu_fan >= 2900 || cpu_fan >1000)
        {
            write_to_ec(GPUaddr,MEDIUM);
        }
        else if (gpu_fan <= 2100)
        {
            set_gpu_fan(128);
        }
    }
    else
    {
        if (gpu_fan < 3500)
        {
            set_gpu_fan(255);
            // write_to_ec(GPUaddr,FAST);
        }
    }
};

void Task::print_status()
{
    std::cout << "Current fan speeds : " << cpu_fan << " RPM and " << gpu_fan << " RPM.      " << std::endl;
    std::cout << "CPU and GPU temperatures : " << cpu_temp/1000 << "°C and " << gpu_temp/1000 << "°C.  " << std::endl;
    std::cout << "Press <Space> to quit"<<std::endl;
    std::cout << "\033[3F";
};


