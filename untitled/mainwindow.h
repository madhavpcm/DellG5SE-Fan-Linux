#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool is_auto_mode_running = false;

    Ui::MainWindow *ui;
private slots:
    void on_auto_Exec_clicked();
    void on_manual_Exec_clicked();
};
#endif // MAINWINDOW_H
