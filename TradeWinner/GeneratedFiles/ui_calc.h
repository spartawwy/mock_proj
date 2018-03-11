/********************************************************************************
** Form generated from reading UI file 'calc.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALC_H
#define UI_CALC_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalcForm
{
public:
    QDoubleSpinBox *dbspbox_price;
    QLabel *label_raise_percent;
    QDoubleSpinBox *updown_percent;
    QLabel *label_raise_percent_bfh;
    QLabel *label_calcwin_start_price;
    QSpinBox *spinBox_quantity;
    QLabel *lab_quantity;
    QLabel *lab_fee_intro;
    QLabel *lab_result;
    QPushButton *pbtn_calc;
    QGroupBox *groupBox;
    QRadioButton *radio_up;
    QRadioButton *radio_down;
    QLabel *lab_result_tag;

    void setupUi(QWidget *CalcForm)
    {
        if (CalcForm->objectName().isEmpty())
            CalcForm->setObjectName(QStringLiteral("CalcForm"));
        CalcForm->resize(437, 335);
        dbspbox_price = new QDoubleSpinBox(CalcForm);
        dbspbox_price->setObjectName(QStringLiteral("dbspbox_price"));
        dbspbox_price->setGeometry(QRect(130, 40, 101, 31));
        dbspbox_price->setDecimals(2);
        dbspbox_price->setMaximum(999.99);
        label_raise_percent = new QLabel(CalcForm);
        label_raise_percent->setObjectName(QStringLiteral("label_raise_percent"));
        label_raise_percent->setGeometry(QRect(50, 110, 71, 21));
        QFont font;
        font.setPointSize(10);
        label_raise_percent->setFont(font);
        label_raise_percent->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        updown_percent = new QDoubleSpinBox(CalcForm);
        updown_percent->setObjectName(QStringLiteral("updown_percent"));
        updown_percent->setGeometry(QRect(130, 100, 81, 31));
        updown_percent->setDecimals(1);
        updown_percent->setMaximum(1000);
        label_raise_percent_bfh = new QLabel(CalcForm);
        label_raise_percent_bfh->setObjectName(QStringLiteral("label_raise_percent_bfh"));
        label_raise_percent_bfh->setGeometry(QRect(210, 100, 31, 21));
        QFont font1;
        font1.setPointSize(12);
        label_raise_percent_bfh->setFont(font1);
        label_raise_percent_bfh->setAlignment(Qt::AlignCenter);
        label_calcwin_start_price = new QLabel(CalcForm);
        label_calcwin_start_price->setObjectName(QStringLiteral("label_calcwin_start_price"));
        label_calcwin_start_price->setGeometry(QRect(50, 50, 71, 21));
        label_calcwin_start_price->setFont(font);
        label_calcwin_start_price->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        spinBox_quantity = new QSpinBox(CalcForm);
        spinBox_quantity->setObjectName(QStringLiteral("spinBox_quantity"));
        spinBox_quantity->setGeometry(QRect(130, 161, 101, 31));
        spinBox_quantity->setMaximum(999999999);
        spinBox_quantity->setValue(100);
        lab_quantity = new QLabel(CalcForm);
        lab_quantity->setObjectName(QStringLiteral("lab_quantity"));
        lab_quantity->setGeometry(QRect(30, 170, 91, 21));
        lab_quantity->setFont(font);
        lab_quantity->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lab_fee_intro = new QLabel(CalcForm);
        lab_fee_intro->setObjectName(QStringLiteral("lab_fee_intro"));
        lab_fee_intro->setGeometry(QRect(110, 290, 221, 31));
        lab_fee_intro->setFont(font);
        lab_fee_intro->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lab_result = new QLabel(CalcForm);
        lab_result->setObjectName(QStringLiteral("lab_result"));
        lab_result->setGeometry(QRect(150, 230, 121, 31));
        lab_result->setFont(font);
        lab_result->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        pbtn_calc = new QPushButton(CalcForm);
        pbtn_calc->setObjectName(QStringLiteral("pbtn_calc"));
        pbtn_calc->setGeometry(QRect(260, 230, 75, 23));
        groupBox = new QGroupBox(CalcForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(240, 100, 91, 41));
        radio_up = new QRadioButton(groupBox);
        radio_up->setObjectName(QStringLiteral("radio_up"));
        radio_up->setGeometry(QRect(10, 0, 82, 18));
        radio_up->setChecked(true);
        radio_down = new QRadioButton(groupBox);
        radio_down->setObjectName(QStringLiteral("radio_down"));
        radio_down->setGeometry(QRect(10, 20, 82, 18));
        lab_result_tag = new QLabel(CalcForm);
        lab_result_tag->setObjectName(QStringLiteral("lab_result_tag"));
        lab_result_tag->setGeometry(QRect(90, 230, 41, 31));
        lab_result_tag->setFont(font);
        lab_result_tag->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        retranslateUi(CalcForm);

        QMetaObject::connectSlotsByName(CalcForm);
    } // setupUi

    void retranslateUi(QWidget *CalcForm)
    {
        CalcForm->setWindowTitle(QApplication::translate("CalcForm", "\350\256\241\347\256\227\345\231\250", 0));
        label_raise_percent->setText(QApplication::translate("CalcForm", "\344\273\267\345\267\256:", 0));
        label_raise_percent_bfh->setText(QApplication::translate("CalcForm", "%", 0));
        label_calcwin_start_price->setText(QApplication::translate("CalcForm", "\350\265\267\345\247\213\344\273\267\346\240\274:", 0));
        lab_quantity->setText(QApplication::translate("CalcForm", "\344\271\260\345\215\226\346\225\260\351\207\217:", 0));
        lab_fee_intro->setText(QApplication::translate("CalcForm", "\346\263\250:\344\275\243\351\207\221 3/10000,\346\234\200\344\275\2165\345\205\203\n"
"    \345\215\260\350\212\261\347\250\216 1/1000", 0));
        lab_result->setText(QApplication::translate("CalcForm", "fee ret", 0));
        pbtn_calc->setText(QApplication::translate("CalcForm", "\350\256\241\347\256\227", 0));
        groupBox->setTitle(QString());
        radio_up->setText(QApplication::translate("CalcForm", "\344\270\212\346\266\250", 0));
        radio_down->setText(QApplication::translate("CalcForm", "\344\270\213\350\267\214", 0));
        lab_result_tag->setText(QApplication::translate("CalcForm", "\347\273\223\346\236\234:", 0));
    } // retranslateUi

};

namespace Ui {
    class CalcForm: public Ui_CalcForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALC_H
