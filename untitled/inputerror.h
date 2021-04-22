#ifndef INPUTERROR_H
#define INPUTERROR_H

#include <QDialog>

namespace Ui {
class InputError;
}

class InputError : public QDialog
{
    Q_OBJECT

public:
    explicit InputError(QWidget *parent = nullptr);
    ~InputError();

private:
    Ui::InputError *ui;
};

#endif // INPUTERROR_H
