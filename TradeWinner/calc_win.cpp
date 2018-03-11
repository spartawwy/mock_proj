
#include "calc_win.h"
#include "MySpinBox.h"

CalcWin::CalcWin()
{
    ui.setupUi(this);

    auto obj_name = ui.spinBox_quantity->objectName();
    const QRect geome_val = ui.spinBox_quantity->geometry();
    delete ui.spinBox_quantity;
    ui.spinBox_quantity = new MySpinBox(this);
    ui.spinBox_quantity->setSingleStep(100);
    ui.spinBox_quantity->setObjectName(obj_name);
    ui.spinBox_quantity->setGeometry(geome_val);
    ui.spinBox_quantity->setMaximum(1000000000);

    bool ret = connect(ui.pbtn_calc, SIGNAL(clicked(bool)), this, SLOT(DoCalc(bool))); 
}

void CalcWin::DoCalc(bool)
{
    const double fee_rate = 0.0003;
    const double min_commsstion = 5.0;
    const double tax_rate = 0.001;
    auto price = ui.dbspbox_price->value();
    auto percent = ui.updown_percent->value();
    auto qty = ui.spinBox_quantity->value();
    bool is_up = ui.radio_up->isChecked();
     
    if( price < 0.01 || qty == 0 )
    {
        ui.lab_result->setText("0.0");
        return;
    }
    
    double buy_raw = fee_rate * price * qty;
    double buy_commission = buy_raw < min_commsstion ? min_commsstion : fee_rate * buy_raw;
    double sell_totol_money = 0.0;
    double sell_commission = 0.0;
    double sell_tax = 0.0;

    if( is_up )
    { 
        sell_totol_money = price * qty * ( (100 + percent) / 100 );
        sell_tax = price * qty * ( (100 + percent) / 100 ) * tax_rate;
        
    }else
    {
        if( percent > 100.0 )
            return;
        sell_totol_money = price * qty * ( (100 - percent) / 100 );
        sell_tax = price * qty * ( (100 - percent) / 100 ) * tax_rate;
    }
    sell_commission = fee_rate * sell_totol_money < min_commsstion ? min_commsstion : fee_rate * sell_totol_money;

    char buf[64] = {'\0'};
    sprintf(buf, "%.2f", sell_totol_money - price * qty - buy_commission - sell_commission - sell_tax );
    ui.lab_result->setText(buf);
}
