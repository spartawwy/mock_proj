#include "checkhintlist.h"

#include <qdebug.h>

#include <QCompleter>
#include "HintList.h"

CheckHintList::CheckHintList(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
#if 1
    m_list_hint= new HintList(this);
    m_list_hint->hide();
    
    bool ret = connect(ui.lineEdit_fromStation, SIGNAL(textChanged(QString)),this,SLOT(FlushFromStationListWidget(QString)));
    ret = connect(m_list_hint,SIGNAL(clicked(QModelIndex)),this,SLOT(onClickedListWidget(QModelIndex)));
    ret = connect(m_list_hint,SIGNAL(choiceStr(QString)),this,SLOT(changeFromStationText(QString)));
#endif
#if 0 
    QStringList strings;  
    strings << "Biao" << "Biao Huang" << "Mac" << "MacBook" << "MacBook Pro" << "Mac Pro";  
    auto completer = new QCompleter(strings, this);  
    completer->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    ui.lineEdit_fromStation->setCompleter(completer);
#endif
}

CheckHintList::~CheckHintList()
{

}

void CheckHintList::keyPressEvent(QKeyEvent *event)
{
    if( ui.lineEdit_fromStation->hasFocus() )
    {
        qDebug() << "keyPressEvent"; 
        m_list_hint->setFocus();
    }
}

void CheckHintList::FlushFromStationListWidget(QString str)
{
    m_list_hint->clear();

    m_list_hint->addItem("dsa");
    m_list_hint->addItem("eea");
    m_list_hint->addItem("ddda");

    int x = ui.lineEdit_fromStation->x() + 4;
    int y = ui.lineEdit_fromStation->y() + 44;
    int w = ui.lineEdit_fromStation->width();
    int h = m_list_hint->count() * 20 ;

    m_list_hint->setGeometry(x,y,w,h);
    m_list_hint->setCurrentRow(0);
    m_list_hint->ShowUI();
} 

void CheckHintList::onClickedListWidget(QModelIndex index)
{
    ui.lineEdit_fromStation->setText(m_list_hint->item(index.row())->text());
    m_list_hint->hide();
}

void CheckHintList::changeFromStationText(QString text)
{
    ui.lineEdit_fromStation->setText(text);
}