/********************************************************************************
** Form generated from reading UI file 'graphic_componet.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHIC_COMPONET_H
#define UI_GRAPHIC_COMPONET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphicComponetClass
{
public:

    void setupUi(QWidget *GraphicComponetClass)
    {
        if (GraphicComponetClass->objectName().isEmpty())
            GraphicComponetClass->setObjectName(QStringLiteral("GraphicComponetClass"));
        GraphicComponetClass->resize(600, 400);

        retranslateUi(GraphicComponetClass);

        QMetaObject::connectSlotsByName(GraphicComponetClass);
    } // setupUi

    void retranslateUi(QWidget *GraphicComponetClass)
    {
        GraphicComponetClass->setWindowTitle(QApplication::translate("GraphicComponetClass", "GraphicComponet", 0));
    } // retranslateUi

};

namespace Ui {
    class GraphicComponetClass: public Ui_GraphicComponetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHIC_COMPONET_H
