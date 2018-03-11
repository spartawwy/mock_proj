/********************************************************************************
** Form generated from reading UI file 'flashingorder.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLASHINGORDER_H
#define UI_FLASHINGORDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FlashingOrderClass
{
public:
    QLabel *label;
    QLabel *label_2;
    QSpinBox *spinBox_buy_quantity;
    QSpinBox *spinBox_sell_quantity;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *pbtn_fresh;
    QPushButton *pbtn_save;
    QComboBox *combox_price_level;
    QLabel *label_price_level_2;

    void setupUi(QWidget *FlashingOrderClass)
    {
        if (FlashingOrderClass->objectName().isEmpty())
            FlashingOrderClass->setObjectName(QStringLiteral("FlashingOrderClass"));
        FlashingOrderClass->resize(600, 400);
        label = new QLabel(FlashingOrderClass);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(85, 30, 201, 31));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);
        label_2 = new QLabel(FlashingOrderClass);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(85, 80, 191, 31));
        label_2->setFont(font);
        spinBox_buy_quantity = new QSpinBox(FlashingOrderClass);
        spinBox_buy_quantity->setObjectName(QStringLiteral("spinBox_buy_quantity"));
        spinBox_buy_quantity->setGeometry(QRect(185, 130, 101, 31));
        spinBox_sell_quantity = new QSpinBox(FlashingOrderClass);
        spinBox_sell_quantity->setObjectName(QStringLiteral("spinBox_sell_quantity"));
        spinBox_sell_quantity->setGeometry(QRect(185, 190, 101, 31));
        label_3 = new QLabel(FlashingOrderClass);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(70, 130, 111, 31));
        label_3->setFont(font);
        label_4 = new QLabel(FlashingOrderClass);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(70, 190, 101, 31));
        label_4->setFont(font);
        pbtn_fresh = new QPushButton(FlashingOrderClass);
        pbtn_fresh->setObjectName(QStringLiteral("pbtn_fresh"));
        pbtn_fresh->setGeometry(QRect(130, 310, 141, 23));
        pbtn_save = new QPushButton(FlashingOrderClass);
        pbtn_save->setObjectName(QStringLiteral("pbtn_save"));
        pbtn_save->setGeometry(QRect(300, 310, 141, 23));
        combox_price_level = new QComboBox(FlashingOrderClass);
        combox_price_level->setObjectName(QStringLiteral("combox_price_level"));
        combox_price_level->setGeometry(QRect(185, 240, 291, 31));
        label_price_level_2 = new QLabel(FlashingOrderClass);
        label_price_level_2->setObjectName(QStringLiteral("label_price_level_2"));
        label_price_level_2->setGeometry(QRect(80, 240, 91, 21));
        label_price_level_2->setFont(font);
        label_price_level_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        retranslateUi(FlashingOrderClass);

        QMetaObject::connectSlotsByName(FlashingOrderClass);
    } // setupUi

    void retranslateUi(QWidget *FlashingOrderClass)
    {
        FlashingOrderClass->setWindowTitle(QApplication::translate("FlashingOrderClass", "\351\227\252\347\224\265\344\272\244\346\230\223\345\212\251\346\211\213", 0));
        label->setText(QApplication::translate("FlashingOrderClass", "\344\271\260\345\205\245\345\277\253\346\215\267\351\224\256 : CTRL +  =", 0));
        label_2->setText(QApplication::translate("FlashingOrderClass", "\345\215\226\345\207\272\345\277\253\346\215\267\351\224\256:  CTRL + \344\270\200", 0));
        label_3->setText(QApplication::translate("FlashingOrderClass", "\346\257\217\346\254\241\344\271\260\345\205\245\346\225\260\351\207\217:", 0));
        label_4->setText(QApplication::translate("FlashingOrderClass", "\346\257\217\346\254\241\345\215\226\345\207\272\346\225\260\351\207\217:", 0));
        pbtn_fresh->setText(QApplication::translate("FlashingOrderClass", "\345\210\267\346\226\260", 0));
        pbtn_save->setText(QApplication::translate("FlashingOrderClass", "\344\277\235\345\255\230", 0));
        label_price_level_2->setText(QApplication::translate("FlashingOrderClass", "\344\272\244\346\230\223\344\273\267\346\240\274:", 0));
    } // retranslateUi

};

namespace Ui {
    class FlashingOrderClass: public Ui_FlashingOrderClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLASHINGORDER_H
