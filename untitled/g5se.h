#ifndef G5SE_H
#define G5SE_H

#include <QMainWindow>

class G5SE : public QMainWindow
{
    Q_OBJECT
public:
    G5SE(QWidget *parent= nullptr );
    ~G5SE();

private:
    Ui::G5SE *ui;
signals:

};

#endif // G5SE_H
