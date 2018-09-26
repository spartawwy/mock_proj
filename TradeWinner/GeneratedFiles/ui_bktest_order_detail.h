/********************************************************************************
** Form generated from reading UI file 'bktest_order_detail.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BKTEST_ORDER_DETAIL_H
#define UI_BKTEST_ORDER_DETAIL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BktestOrderDetailForm
{
public:
    QPlainTextEdit *pte_bktest_detail;

    void setupUi(QWidget *BktestOrderDetailForm)
    {
        if (BktestOrderDetailForm->objectName().isEmpty())
            BktestOrderDetailForm->setObjectName(QStringLiteral("BktestOrderDetailForm"));
        BktestOrderDetailForm->resize(714, 581);
        pte_bktest_detail = new QPlainTextEdit(BktestOrderDetailForm);
        pte_bktest_detail->setObjectName(QStringLiteral("pte_bktest_detail"));
        pte_bktest_detail->setEnabled(true);
        pte_bktest_detail->setGeometry(QRect(10, 10, 691, 541));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(11);
        font.setBold(false);
        font.setWeight(50);
        pte_bktest_detail->setFont(font);
        pte_bktest_detail->setReadOnly(true);

        retranslateUi(BktestOrderDetailForm);

        QMetaObject::connectSlotsByName(BktestOrderDetailForm);
    } // setupUi

    void retranslateUi(QWidget *BktestOrderDetailForm)
    {
        BktestOrderDetailForm->setWindowTitle(QApplication::translate("BktestOrderDetailForm", "\345\233\236\346\265\213\346\230\216\347\273\206", 0));
    } // retranslateUi

};

namespace Ui {
    class BktestOrderDetailForm: public Ui_BktestOrderDetailForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BKTEST_ORDER_DETAIL_H
