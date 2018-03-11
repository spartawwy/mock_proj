#ifndef MY_SPIN_BOX_SDFS3SDF_H_
#define MY_SPIN_BOX_SDFS3SDF_H_

#include <QSpinBox>

class MySpinBox : public QSpinBox
{
public:

    MySpinBox(QWidget * parent = 0);

    virtual void focusOutEvent(QFocusEvent * event) override;

};
#endif 