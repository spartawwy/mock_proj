/********************************************************************************
** Form generated from reading UI file 'demo.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEMO_H
#define UI_DEMO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_demoClass
{
public:
    QPushButton *pbt_test;

    void setupUi(QDialog *demoClass)
    {
        if (demoClass->objectName().isEmpty())
            demoClass->setObjectName(QStringLiteral("demoClass"));
        demoClass->resize(600, 400);
        pbt_test = new QPushButton(demoClass);
        pbt_test->setObjectName(QStringLiteral("pbt_test"));
        pbt_test->setGeometry(QRect(190, 100, 75, 23));

        retranslateUi(demoClass);

        QMetaObject::connectSlotsByName(demoClass);
    } // setupUi

    void retranslateUi(QDialog *demoClass)
    {
        demoClass->setWindowTitle(QApplication::translate("demoClass", "demo", 0));
        pbt_test->setText(QApplication::translate("demoClass", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class demoClass: public Ui_demoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEMO_H
