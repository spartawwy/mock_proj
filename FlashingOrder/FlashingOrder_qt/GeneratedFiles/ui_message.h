/********************************************************************************
** Form generated from reading UI file 'message.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGE_H
#define UI_MESSAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_messageForm
{
public:
    QLabel *label_content;

    void setupUi(QWidget *messageForm)
    {
        if (messageForm->objectName().isEmpty())
            messageForm->setObjectName(QStringLiteral("messageForm"));
        messageForm->resize(400, 300);
        label_content = new QLabel(messageForm);
        label_content->setObjectName(QStringLiteral("label_content"));
        label_content->setGeometry(QRect(10, 70, 381, 111));
        QFont font;
        font.setPointSize(12);
        label_content->setFont(font);
        label_content->setTextFormat(Qt::PlainText);
        label_content->setAlignment(Qt::AlignCenter);
        label_content->setWordWrap(true);

        retranslateUi(messageForm);

        QMetaObject::connectSlotsByName(messageForm);
    } // setupUi

    void retranslateUi(QWidget *messageForm)
    {
        messageForm->setWindowTitle(QApplication::translate("messageForm", "\346\217\220\347\244\272", 0));
        label_content->setText(QApplication::translate("messageForm", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class messageForm: public Ui_messageForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGE_H
