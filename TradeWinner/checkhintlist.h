#ifndef CHECKHINTLIST_H
#define CHECKHINTLIST_H

#include <QtWidgets/QWidget>
#include "ui_checkhintlist.h"

class HintList;

class CheckHintList : public QWidget
{
    Q_OBJECT

public:
    CheckHintList(QWidget *parent = 0);
    ~CheckHintList();

    virtual void keyPressEvent(QKeyEvent *event) override;

public slots:

    void FlushFromStationListWidget(QString str);
    void onClickedListWidget(QModelIndex index);
    void changeFromStationText(QString text);

    
private:
    Ui::CheckHintListClass ui;

    HintList  *m_list_hint;
};

#endif // CHECKHINTLIST_H
