/********************************************************************************
** Form generated from reading UI file 'records.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECORDS_H
#define UI_RECORDS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RecordsForm
{
public:
    QTabWidget *tabWidget;
    QWidget *tab_position;
    QTableView *tbview_position;
    QWidget *tab_records;
    QTableView *tbview_fills;

    void setupUi(QWidget *RecordsForm)
    {
        if (RecordsForm->objectName().isEmpty())
            RecordsForm->setObjectName(QStringLiteral("RecordsForm"));
        RecordsForm->resize(535, 513);
        tabWidget = new QTabWidget(RecordsForm);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 531, 481));
        tab_position = new QWidget();
        tab_position->setObjectName(QStringLiteral("tab_position"));
        tbview_position = new QTableView(tab_position);
        tbview_position->setObjectName(QStringLiteral("tbview_position"));
        tbview_position->setGeometry(QRect(20, 20, 431, 411));
        tabWidget->addTab(tab_position, QString());
        tab_records = new QWidget();
        tab_records->setObjectName(QStringLiteral("tab_records"));
        tbview_fills = new QTableView(tab_records);
        tbview_fills->setObjectName(QStringLiteral("tbview_fills"));
        tbview_fills->setGeometry(QRect(10, 20, 491, 421));
        tbview_fills->setSizeIncrement(QSize(2, 2));
        tabWidget->addTab(tab_records, QString());

        retranslateUi(RecordsForm);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(RecordsForm);
    } // setupUi

    void retranslateUi(QWidget *RecordsForm)
    {
        RecordsForm->setWindowTitle(QApplication::translate("RecordsForm", "\344\272\244\346\230\223\350\256\260\345\275\225\344\273\223\344\275\215", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_position), QApplication::translate("RecordsForm", "\344\273\223\344\275\215", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_records), QApplication::translate("RecordsForm", "\344\272\244\346\230\223\350\256\260\345\275\225", 0));
    } // retranslateUi

};

namespace Ui {
    class RecordsForm: public Ui_RecordsForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECORDS_H
