/********************************************************************************
** Form generated from reading UI file 'broker_cfg.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BROKER_CFG_H
#define UI_BROKER_CFG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_broker_cfg_dlg
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *label_12;
    QLineEdit *le_account;
    QLabel *label_13;
    QLineEdit *le_pwd;
    QComboBox *cb_broker;
    QLabel *label_14;
    QComboBox *cb_department;
    QLabel *label_15;
    QCheckBox *cbox_remenberpwd;

    void setupUi(QDialog *broker_cfg_dlg)
    {
        if (broker_cfg_dlg->objectName().isEmpty())
            broker_cfg_dlg->setObjectName(QStringLiteral("broker_cfg_dlg"));
        broker_cfg_dlg->resize(536, 394);
        layoutWidget = new QWidget(broker_cfg_dlg);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(60, 320, 301, 33));
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

        label_12 = new QLabel(broker_cfg_dlg);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(120, 160, 71, 21));
        QFont font;
        font.setPointSize(10);
        label_12->setFont(font);
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        le_account = new QLineEdit(broker_cfg_dlg);
        le_account->setObjectName(QStringLiteral("le_account"));
        le_account->setGeometry(QRect(200, 160, 161, 31));
        label_13 = new QLabel(broker_cfg_dlg);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(120, 210, 71, 21));
        label_13->setFont(font);
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        le_pwd = new QLineEdit(broker_cfg_dlg);
        le_pwd->setObjectName(QStringLiteral("le_pwd"));
        le_pwd->setGeometry(QRect(200, 210, 161, 31));
        cb_broker = new QComboBox(broker_cfg_dlg);
        cb_broker->setObjectName(QStringLiteral("cb_broker"));
        cb_broker->setGeometry(QRect(200, 60, 161, 31));
        label_14 = new QLabel(broker_cfg_dlg);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(120, 70, 71, 21));
        label_14->setFont(font);
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cb_department = new QComboBox(broker_cfg_dlg);
        cb_department->setObjectName(QStringLiteral("cb_department"));
        cb_department->setGeometry(QRect(200, 110, 161, 31));
        label_15 = new QLabel(broker_cfg_dlg);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(120, 120, 71, 21));
        label_15->setFont(font);
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cbox_remenberpwd = new QCheckBox(broker_cfg_dlg);
        cbox_remenberpwd->setObjectName(QStringLiteral("cbox_remenberpwd"));
        cbox_remenberpwd->setGeometry(QRect(300, 270, 131, 31));

        retranslateUi(broker_cfg_dlg);
        QObject::connect(cancelButton, SIGNAL(clicked()), broker_cfg_dlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(broker_cfg_dlg);
    } // setupUi

    void retranslateUi(QDialog *broker_cfg_dlg)
    {
        broker_cfg_dlg->setWindowTitle(QApplication::translate("broker_cfg_dlg", "\350\257\201\345\210\270\347\231\273\345\275\225", 0));
        okButton->setText(QApplication::translate("broker_cfg_dlg", "\347\241\256\345\256\232", 0));
        cancelButton->setText(QApplication::translate("broker_cfg_dlg", "\345\217\226\346\266\210", 0));
        label_12->setText(QApplication::translate("broker_cfg_dlg", "\350\264\246\345\217\267:", 0));
        label_13->setText(QApplication::translate("broker_cfg_dlg", "\345\257\206\347\240\201:", 0));
        label_14->setText(QApplication::translate("broker_cfg_dlg", "\345\210\270\345\225\206:", 0));
        label_15->setText(QApplication::translate("broker_cfg_dlg", "\350\220\245\344\270\232\351\203\250:", 0));
        cbox_remenberpwd->setText(QApplication::translate("broker_cfg_dlg", "\350\256\260\344\275\217\345\257\206\347\240\201", 0));
    } // retranslateUi

};

namespace Ui {
    class broker_cfg_dlg: public Ui_broker_cfg_dlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROKER_CFG_H
