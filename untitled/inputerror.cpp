#include "inputerror.h"
#include "ui_inputerror.h"

InputError::InputError(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputError)
{
    ui->setupUi(this);
}

InputError::~InputError()
{
    delete ui;
}
