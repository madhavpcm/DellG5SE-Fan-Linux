#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <polkit-qt5-1/PolkitQt1/Gui/ActionButton>

bool isExec = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    check_fan_write_permission();
    //manual_fan_mode(true);
    auto bt = new PolkitQt1::Gui::ActionButton(kickPB, "org.qt.policykit.examples.kick", this);
    bt->setText("Kick... (long)");
    // here we set the behavior of PolKitResul = No
    bt->setVisible(true, PolkitQt1::Gui::Action::No);
    bt->setEnabled(true, PolkitQt1::Gui::Action::No);
    bt->setText("Kick (long)", PolkitQt1::Gui::Action::No);
    bt->setIcon(QPixmap(":/Icons/custom-no.png"), PolkitQt1::Gui::Action::No);
    bt->setToolTip("If your admin wasn't annoying, you could do this", PolkitQt1::Gui::Action::No);
    // here we set the behavior of PolKitResul = Auth
    bt->setVisible(true, PolkitQt1::Gui::Action::Auth);
    bt->setEnabled(true, PolkitQt1::Gui::Action::Auth);
    bt->setText("Kick... (long)", PolkitQt1::Gui::Action::Auth);
    bt->setIcon(QPixmap(":/Icons/PolkitQt1::Gui::Action-locked-default.png"), PolkitQt1::Gui::Action::Auth);
    bt->setToolTip("Only card carrying tweakers can do this!", PolkitQt1::Gui::Action::Auth);
    // here we set the behavior of PolKitResul = Yes
    bt->setVisible(true, PolkitQt1::Gui::Action::Yes);
    bt->setEnabled(true, PolkitQt1::Gui::Action::Yes);
    bt->setText("Kick! (long)", PolkitQt1::Gui::Action::Yes);
    bt->setIcon(QPixmap(":/Icons/custom-yes.png"), PolkitQt1::Gui::Action::Yes);
    bt->setToolTip("Go ahead, kick kick kick!", PolkitQt1::Gui::Action::Yes);
    ui->setupUi(this);

}

void MainWindow::manual_fan_mode(bool on)
{
    if(on){
        write_to_ec(ManualECMode_cpu, 255);
        write_to_ec(ManualECMode_gpu, 255);
        QMessageBox::information(this,"Fan status","Fans are now in Manual mode");
    }
    else
    {
        write_to_ec(ManualECMode_cpu, 4);
        write_to_ec(ManualECMode_gpu, 4);
        QMessageBox::information(this, "Fan status", "Fans are now under BIOS control");
    }

}

void MainWindow::check_fan_write_permission()
{
    if (getuid() != 0){
        QMessageBox::warning(this, "Error", "This program needs elevated permissions to run. Run as sudo");
        QApplication::quit();
    }
    // Checks if ec_sys is correctly loaded.
    fs::path f(ECio);
    if(!fs::exists(f)){
        QMessageBox::warning(this, "Error", "EC I/O address not found. Make sure ec_sys module is loaded correctly. Try sudo modprobe ec_sys write_support=1 or add these options to the required conf files ");
        QApplication::quit();
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_dial_cpu_valueChanged(int value)
{
    uint8_t fan = value;
    //set_cpu_fan(value);
    ui->cpu_rpm->display(value);
}


void MainWindow::on_dial_gpu_valueChanged(int value)
{
    uint8_t fan=value;
    //set_gpu_fan(value);
    ui->gpu_rpm->display(value);
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
