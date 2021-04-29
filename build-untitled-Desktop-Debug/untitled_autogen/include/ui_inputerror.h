/********************************************************************************
** Form generated from reading UI file 'inputerror.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INPUTERROR_H
#define UI_INPUTERROR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_InputError
{
public:
    QDialogButtonBox *buttonBox;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *InputError)
    {
        if (InputError->objectName().isEmpty())
            InputError->setObjectName(QString::fromUtf8("InputError"));
        InputError->resize(388, 186);
        buttonBox = new QDialogButtonBox(InputError);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 150, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textBrowser = new QTextBrowser(InputError);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(20, 30, 351, 111));

        retranslateUi(InputError);
        QObject::connect(buttonBox, SIGNAL(rejected()), InputError, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), InputError, SLOT(accept()));

        QMetaObject::connectSlotsByName(InputError);
    } // setupUi

    void retranslateUi(QDialog *InputError)
    {
        InputError->setWindowTitle(QCoreApplication::translate("InputError", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InputError: public Ui_InputError {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INPUTERROR_H
