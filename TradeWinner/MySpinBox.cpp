#include "MySpinBox.h"

//void QAbstractSpinBox::focusOutEvent(QFocusEvent * event) [virtual protected]

MySpinBox::MySpinBox(QWidget * parent) : QSpinBox(parent)
{ 
}

void MySpinBox::focusOutEvent(QFocusEvent * event)
{
    if( this->value() % 100 != 0 )
    {
        if( this->value() < 100 )
            this->setValue(100);
        else
            this->setValue( (this->value() / 100) * 100 );
    }
    QSpinBox::focusOutEvent(event);
}