/********************************************************************************
** Form generated from reading UI file 'msg.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MSG_H
#define UI_MSG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MsgForm
{
public:
    QLabel *label_content;

    void setupUi(QWidget *MsgForm)
    {
        if (MsgForm->objectName().isEmpty())
            MsgForm->setObjectName(QStringLiteral("MsgForm"));
        MsgForm->resize(400, 300);
        label_content = new QLabel(MsgForm);
        label_content->setObjectName(QStringLiteral("label_content"));
        label_content->setGeometry(QRect(10, 70, 381, 111));
        QFont font;
        font.setPointSize(12);
        label_content->setFont(font);
        label_content->setTextFormat(Qt::PlainText);
        label_content->setAlignment(Qt::AlignCenter);
        label_content->setWordWrap(true);

        retranslateUi(MsgForm);

        QMetaObject::connectSlotsByName(MsgForm);
    } // setupUi

    void retranslateUi(QWidget *MsgForm)
    {
        MsgForm->setWindowTitle(QApplication::translate("MsgForm", "\346\217\220\347\244\272", 0));
        label_content->setText(QApplication::translate("MsgForm", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class MsgForm: public Ui_MsgForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MSG_H
