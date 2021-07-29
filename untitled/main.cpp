#include "mainwindow.h"
#include <iostream>
#include <QObject>
#include <QApplication>
#include <qtimer.h>
#include "task.h"

#include <polkit-qt5-1/PolkitQt1/Agent/Session>

QCoreApplication* createapp(int& argc, char* argv[] ){
    for (int i = 1 ; i < argc ; i++){
        if(!qstrcmp(argv[i], "-no-gui")){
            return new QCoreApplication(argc, argv);
        }
    }
    return new QApplication(argc, argv);
}
int main(int argc, char *argv[])
{

    QCoreApplication *a(createapp(argc, argv));
    std::cout<< "QT version :: "<< QT_VERSION_STR << '\n';
    QApplication* aGui = qobject_cast<QApplication*>(QCoreApplication::instance());

    if(aGui){
       //gui

        MainWindow w;
        if(w.check_fan_write_permission()){
            w.show();
            return aGui->exec();
        }else{
            QApplication::exit();
        }

    }else{
        //no-gui
        Task *task = new Task(a);
            // This will cause the application to exit when
            // the task signals finished.
        QObject::connect(task, SIGNAL(finished()), a, SLOT(quit()));

            // This will run the task from the application event loop.
        QTimer::singleShot(0, task, SLOT(run()));
        return a->exec();

    }
}
