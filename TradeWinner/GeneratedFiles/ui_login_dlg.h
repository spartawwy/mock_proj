/********************************************************************************
** Form generated from reading UI file 'login_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_DLG_H
#define UI_LOGIN_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_login_dlg
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *label_2;
    QLineEdit *le_name;
    QLabel *label;
    QLineEdit *le_pwd;
    QLabel *label_3;
    QCheckBox *cb_remember_userpwd;

    void setupUi(QDialog *login_dlg)
    {
        if (login_dlg->objectName().isEmpty())
            login_dlg->setObjectName(QStringLiteral("login_dlg"));
        login_dlg->resize(543, 436);
        layoutWidget = new QWidget(login_dlg);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(50, 270, 351, 33));
        hboxLayout = new QHBoxLayout(layoutWidget);
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(layoutWidget);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(layoutWidget);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);

        label_2 = new QLabel(login_dlg);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(120, 190, 101, 31));
        QFont font;
        font.setPointSize(12);
        label_2->setFont(font);
        le_name = new QLineEdit(login_dlg);
        le_name->setObjectName(QStringLiteral("le_name"));
        le_name->setGeometry(QRect(230, 160, 200, 20));
        label = new QLabel(login_dlg);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(110, 160, 101, 21));
        label->setFont(font);
        le_pwd = new QLineEdit(login_dlg);
        le_pwd->setObjectName(QStringLiteral("le_pwd"));
        le_pwd->setGeometry(QRect(230, 200, 200, 20));
        label_3 = new QLabel(login_dlg);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(120, 10, 331, 61));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\215\216\346\226\207\346\245\267\344\275\223"));
        font1.setPointSize(16);
        font1.setStyleStrategy(QFont::PreferAntialias);
        label_3->setFont(font1);
        cb_remember_userpwd = new QCheckBox(login_dlg);
        cb_remember_userpwd->setObjectName(QStringLiteral("cb_remember_userpwd"));
        cb_remember_userpwd->setGeometry(QRect(230, 240, 71, 16));

        retranslateUi(login_dlg);
        QObject::connect(cancelButton, SIGNAL(clicked()), login_dlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(login_dlg);
    } // setupUi

    void retranslateUi(QDialog *login_dlg)
    {
        login_dlg->setWindowTitle(QApplication::translate("login_dlg", "\347\231\273\345\275\225TradeWinner 0.1", 0));
        okButton->setText(QApplication::translate("login_dlg", "\347\241\256\345\256\232", 0));
        cancelButton->setText(QApplication::translate("login_dlg", "\345\217\226\346\266\210", 0));
        label_2->setText(QApplication::translate("login_dlg", " \345\257\206\347\240\201:", 0));
        label->setText(QApplication::translate("login_dlg", "\347\224\250\346\210\267\345\220\215:", 0));
        label_3->setText(QApplication::translate("login_dlg", "TradeWinner\350\202\241\347\245\250\350\207\252\345\212\250\345\214\226\344\272\244\346\230\223\347\263\273\347\273\237", 0));
        cb_remember_userpwd->setText(QApplication::translate("login_dlg", "\350\207\252\345\212\250\344\277\235\345\255\230", 0));
    } // retranslateUi

};

namespace Ui {
    class login_dlg: public Ui_login_dlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_DLG_H
