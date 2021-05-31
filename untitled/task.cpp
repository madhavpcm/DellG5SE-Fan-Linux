#include "task.h"

void Task::loop_break(){
    char s;
    while(true){
        s=getchar();
        if(s=='\n' || s==EOF)
            break;
    }
    stop=true;
    manual_fan_mode(false);
    return;

}
void Task::exit_handler(int x){
    std::cout<<"<--------------------->\n";
    if(x==SIGINT){
        std::cout<<"Keyboard interrupt. Exiting\n";
    }else if(x==SIGTERM){
        std::cout<<"Kill initiated by system. Exiting\n";
    }
    manual_fan_mode(false);
    exit(EXIT_FAILURE);
}
void Task::run(){
    QStringList argv=QCoreApplication::arguments();
    uint32_t argc= argv.size();
    int timer =10;
    t1=t2=t3=t4=-1;
    update_vars();
    check_fan_write_permission();
    for(uint32_t i=1 ; i<argc ; i++){
        if ((argv[i])=="--restore" || (argv[i])=="-r")
               {

                   manual_fan_mode(false);
                   exit(EXIT_SUCCESS);
               }
               if ((argv[i])=="--manual" || (argv[i])=="-m")
               {

                   manual_fan_mode(true);
                   std::cin.get();
                   exit(EXIT_SUCCESS);
               }
               if ((argv[i])=="--boost" || (argv[i])=="-b")
               {

                   manual_fan_mode(true);
                   set_gpu_fan(BOOST);
                   set_cpu_fan(BOOST);
                   std::cout<<"Boost speed. Be carefull, manual fan mode is on.\n Press <Enter> to quit";
                   std::cin.get();
                   exit(EXIT_SUCCESS);
               }
               if ((argv[i])=="--set" || (argv[i])=="-s")
               {

                   if (argc < i+3)
                   {
                       printf("Need more arguments.\n");
                       exit(EXIT_FAILURE);
                   }

                   manual_fan_mode(true);

                   uint8_t left = (argv[i+1]).toUInt();
                   set_cpu_fan(hex_to_EC(left));
                   uint8_t right = (argv[i+2]).toUInt();
                   set_gpu_fan(hex_to_EC(right));

                   std::cout << "Set fans to "<< argv[i+1].toUInt() <<" and " << argv[i+2].toUInt()<< ". Be carefull, manual fan mode is on."<< std::endl;
                   exit(EXIT_SUCCESS);
               }
               if ((argv[i])=="--timer" || (argv[i])=="-t")
               {
                   if (argc <i+1)
                   {
                       exit(EXIT_FAILURE);
                   }
                   timer = argv[i+1].toInt();
               }
               if ((argv[i])=="--loop" || (argv[i])=="-l")
               {
                   check_fan_write_permission();
                   if (argc < i+5)
                   {
                       printf("Need 4 temperature thresholds.\n");
                       exit(EXIT_FAILURE);
                   }
                   t1 = argv[i+1].toInt();
                   t2 = argv[i+2].toInt();
                   t3 = argv[i+3].toInt();
                   t4 = argv[i+4].toInt();
               }
               if ((argv[i])=="--help" ||(argv[i])=="-h"){
                   usage(argv[0].toStdString(),EXIT_SUCCESS);//qstring to  char*

               }
               if ((argv[i])=="--verbose" || (argv[i])=="-v")
                   verbose=true;

    }


    // std::cout << "Script launched with arguments : " << lowtemp/1000 << " " << hightemp/1000 << " " << timer <<std::endl; //Debug

    // Get hwmon variables
    Hwmon_get();
    // Check if launched with enough permissions.
    check_fan_write_permission();

    std::thread t(&Task::loop_break, this);
    signal(SIGINT, &Task::exit_handler);
    signal(SIGTERM, &Task::exit_handler);

    manual_fan_mode(true);
    set_cpu_fan(SLOW);
    set_gpu_fan(SLOW);    // Fan update loop
    while (!stop)
    {
        //First update the variables.
        update_vars();
        //Then update the fan speed accordingly.
        update_fans();
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
uint8_t Task::hex_to_EC(uint8_t hex){
    return std::min(std::max(255-hex, 91),255);
};

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
//    int l = std::max(0, std::min(255, left));
    // Writes to hwmon
   // std::ofstream pwm;
  //  pwm.open(dellsmm + "/pwm1");
    //pwm << l;
    //pwm.close();
    write_to_ec(CPUaddr, left);
};
// Set gpu fans to selected speed. Input should be in the set {0,128,256}.
void Task::set_gpu_fan(int right)
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


void Task::write_to_ec(int byte_offset, uint8_t value){
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
        err(EXIT_FAILURE, "Error Writing to EC");
}
void Task::manual_fan_mode(bool on)
{
    std::cout<<"\n\033c";
    if(on){
        write_to_ec(ManualECMode_cpu, 255);
        write_to_ec(ManualECMode_gpu, 255);
        std::cout<<"Set fans to manual mode.\n";
    }
    else
    {
        write_to_ec(ManualECMode_cpu, 4);
        write_to_ec(ManualECMode_gpu, 4);
        std::cout<<"Returned to BIOS Fan control.\n";
    }

}

void Task::check_fan_write_permission()
{
    if (getuid() != 0){
        std::cout << "Cannot change fan speed. Are you running the script with root permission ?" << "\n";
        exit(EXIT_FAILURE);
    } 
    // Checks if ec_sys is correctly loaded.
    fs::path f(ECio);
    if(!fs::exists(f)){
        std::cout << "Cannot find ECio path. Try 'sudo modprobe ec_sys write_support=1' or add 'ec_sys.write_support=1' as kernel parameter if that's not working."<< std::endl;
        exit(EXIT_FAILURE);
    }
};
int Task::fan_curve(uint8_t current_temp, uint32_t current_fan){
    if (current_temp <t1)
    {
        if(current_temp <t1-mou && current_fan > 1500){
            return ZERO;
        }
        return -1;
    }
    if (current_temp <t2)
    {
        if( (current_temp <t2-mou && current_fan >2500) || current_fan < 1500){
            return SLOW;
        }
        return -1;
    }
    if (current_temp <t3)
    {
        if((current_temp <t3-mou && current_fan > 3500 ) || current_fan < 2500){
            return NORMAL;
        }
        return -1;
    }
    if (current_temp <t4 )
    {
        if((current_temp <t4-mou && current_fan > 4900) || current_fan < 3500){
            return FAST;
        }
        return -1;
    }
    return BOOST;
}

void Task::update_fans(){
    int cpu_update = fan_curve(cpu_temp/1000,cpu_fan);
    if (cpu_update != -1 )
        set_cpu_fan(cpu_update);

    int gpu_update = fan_curve(gpu_temp/1000,gpu_fan);
    if (gpu_update != -1 )
        set_gpu_fan(gpu_update);
    if (verbose)
    {
        std::cout <<"CPU and GPU fans update : "<<  cpu_update <<" and "<< gpu_update << ".   \n";
    }

}
void Task::usage(std::string prog_name, int status){
    std::cout<<"Usage :\n";
    std::cout<<"sudo"<<prog_name<<" [-s left_fan_speed right_fan_speed] [-l t1 t2 t3 t4] [-t timer] [-r ] [-b]\n\n";
    std::cout<<"Arguments description :\n";
    std::cout<<" -h, --help             show this help message and exit.\n";
    std::cout<<" -s, --set left right   sets left/rights fan to selected speed (from 0 to 255).\n";
    std::cout<<" -l, --loop t1 t2 t3 t4 given the temperature thresholds t1, t2, t3, t4 (in °C),\n";
    std::cout<<"                        adjusts the fan speed accordingly with a loop.\n";
    std::cout<<" -t, --timer t          set the loop timer (in seconds). Default is 20s.\n";
    std::cout<<" -m, --manual           Switch to manual fan mode.\n";
    std::cout<<" -r, --restore          Gives back the fan control to the BIOS.\n";
    std::cout<<" -b, --boost            Set fan speed to BOOST (as fn+F7).\n";
    std::cout<<" -v, --verbose          Prints loop fan updates. -1 means no fan update made.\n";
    std::cin.get();
    exit(status);
}

// Updates fans accordings to temp.
/*void Task::update_fans(int lowtemp, int hightemp)
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
}*/


void Task::print_status()
{
    std::cout << "Current fan speeds : " << cpu_fan << " RPM and " << gpu_fan << " RPM.      \n" ;
    std::cout << "CPU and GPU temperatures : " << cpu_temp/1000 << "°C and " << gpu_temp/1000 << "°C.  \n" ;
    std::cout << "Press <Enter> to quit\n";
    std::cout << "\033[3F";
    if(verbose) std::cout<<"\033[1F";
};



