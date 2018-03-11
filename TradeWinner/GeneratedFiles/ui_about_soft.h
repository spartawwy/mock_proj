/********************************************************************************
** Form generated from reading UI file 'about_soft.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_SOFT_H
#define UI_ABOUT_SOFT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutsoftUi
{
public:
    QLabel *label;

    void setupUi(QWidget *AboutsoftUi)
    {
        if (AboutsoftUi->objectName().isEmpty())
            AboutsoftUi->setObjectName(QStringLiteral("AboutsoftUi"));
        AboutsoftUi->resize(399, 237);
        label = new QLabel(AboutsoftUi);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 30, 361, 171));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        retranslateUi(AboutsoftUi);

        QMetaObject::connectSlotsByName(AboutsoftUi);
    } // setupUi

    void retranslateUi(QWidget *AboutsoftUi)
    {
        AboutsoftUi->setWindowTitle(QApplication::translate("AboutsoftUi", "\345\205\263\344\272\216\346\234\254\350\275\257\344\273\266", 0));
        label->setText(QApplication::translate("AboutsoftUi", "TradeWinner mock \350\202\241\347\245\250\346\250\241\346\213\237\344\272\244\346\230\223\350\275\257\344\273\266. \347\211\210\346\235\203(2018-2030)\345\275\222 \346\261\252\346\226\207\345\216\237\346\211\200\346\234\211\n"
"Email:249564063@qq.com", 0));
    } // retranslateUi

};

namespace Ui {
    class AboutsoftUi: public Ui_AboutsoftUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_SOFT_H
