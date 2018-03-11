#include "broker_cfg_win.h"

#include <qvariant.h>
#include <QMessageBox>
#include "TLib/core/tsystem_utility_functions.h"

#include "winner_app.h"
#include "winner_win.h"

BrokerCfgWin::BrokerCfgWin(WinnerApp* app)
    : app_(app)
{
    ui_.setupUi(this);
    ui_.le_pwd->setEchoMode(QLineEdit::Password);
    auto ret = connect(ui_.cb_broker, SIGNAL(currentIndexChanged(int)), this, SLOT(DocurrentIndexChanged(int)) );
    ret = connect(ui_.okButton, SIGNAL(clicked()), this, SLOT(DoOkBtnClicked()));
}

bool BrokerCfgWin::Init()
{
    auto select_brokertext_by_brokerid = [this](int id) ->QString
    {
        for( int i = 0; i < this->ui_.cb_broker->count(); ++i )
        {
            auto val = this->ui_.cb_broker->itemData(i);
            if( val.toInt() == id )
            {
                return this->ui_.cb_broker->itemText(i);
            }
        }
        return "";
    };

    std::vector<T_BrokerInfo>  broker_info_vector = app_->db_moudle().GetAllBrokerInfo();
    
    QVariant qv(1);
    std::for_each( std::begin(broker_info_vector), std::end(broker_info_vector), [this](T_BrokerInfo& entry)
    { 
        ui_.cb_broker->addItem(QString::fromLocal8Bit(entry.remark.c_str()), QVariant(entry.id));
    });

    
    // todo:   
    ui_.cb_department->addItem(QString::fromLocal8Bit("普通交易"), QVariant(1));
    ui_.cb_department->addItem(QString::fromLocal8Bit("信用交易"), QVariant(3));

    auto p_broker_info = this->app_->user_broker_info();
    if( p_broker_info )
    {
        //ui_.cb_broker->setCurrentIndex(static_cast<int>(p_broker_info->type));
        ui_.cb_broker->setCurrentText(select_brokertext_by_brokerid(p_broker_info->id));
        /*T_UserAccountInfo * p_account = app_->db_moudle().FindUserAccountInfo(app_->user_info().id);
        assert(p_account);
        ui_.le_account->setText(p_account->account_no_in_broker_.c_str()); */
    }

    auto p_account = this->app_->user_account_info();
    if( p_account )
    {
        ui_.le_account->setText(p_account->account_no_in_broker_.c_str());
        ui_.le_pwd->setText(p_account->trade_pwd_.c_str());
    }
    /*
    ui_.le_account->setText("32506627");
    ui_.le_pwd->setText("626261");*/
    return true;
}

void  BrokerCfgWin::DocurrentIndexChanged(int index)
{
    // todo:
}

void BrokerCfgWin::DoOkBtnClicked()
{
    std::string acc_no_input = ui_.le_account->text().trimmed().toLocal8Bit().data();
    auto p_broker_info = app_->db_moudle().FindUserBrokerByBroker(ui_.cb_broker->currentData().toInt());
    assert(p_broker_info);
    if( !app_->trade_agent().Setup(p_broker_info->type, acc_no_input) )
		return;
    //int department_id = ui_.cb_department->currentData().toInt();
    auto ret = app_->LoginBroker(ui_.cb_broker->currentData().toInt()
            , ui_.cb_department->currentData().toInt()
            , ui_.le_account->text().toLocal8Bit().data()
            , ui_.le_pwd->text().toLocal8Bit().data());
 
    if( !ret )
    {
        // todo : information
        //app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("登录失败!"));
        QMessageBox::information(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("登录失败! 请确认密码,并检查网络!"));
        return;
    }

    // save accountInfo------------ 
    T_AccountInformation account_info;
    account_info.id = app_->user_info().account_id;
    account_info.trade_acc_no = account_info.account_no = acc_no_input;
    const int broker_id_input = ui_.cb_broker->currentData().toInt();
    account_info.broker_id = broker_id_input;
    account_info.trade_pwd = ui_.le_pwd->text().toLocal8Bit().data();
    //account_info.comm_pwd;// normal have not commpwd
    account_info.department_id = utility::FormatStr("%d", ui_.cb_department->currentData().toInt());
    if( app_->user_info().account_id <= 0 )
    {
        app_->db_moudle().AddAccountInfo(account_info); // will allocate account_id and fill account_info.id
        app_->user_info().account_id = account_info.id;
        app_->db_moudle().SaveUserinformation(app_->user_info());

    }else // has login broker
    {
        assert( !app_->user_info().account_no.empty() );

        int old_broker_id = app_->db_moudle().FindBorkerIdByAccountID(app_->user_info().account_id);
        if( old_broker_id != broker_id_input )
        {
            auto p_info = app_->db_moudle().FindAccountInfoByAccNoAndBrokerId(acc_no_input, broker_id_input);
            if( !p_info ) 
            {
                app_->db_moudle().AddAccountInfo(account_info);// will allocate account_id and fill account_info.id
                app_->user_info().account_id = account_info.id;
            }else
            {
                app_->user_info().account_id = account_info.id = p_info->account_id;
            }
            app_->user_info().account_no = acc_no_input;
        }
        else
            app_->db_moudle().UpdateAccountInfo(account_info);
    }

    // save userInformation account_id 
    app_->db_moudle().SaveUserinformation(app_->user_info());
     
    // set it  
    app_->user_account_info()->broker_id_ = account_info.broker_id;
    app_->user_account_info()->account_no_in_broker_ = account_info.account_no;
    app_->user_account_info()->trade_pwd_ = account_info.trade_pwd;
    app_->user_account_info()->comm_pwd_ = account_info.comm_pwd;
    app_->user_account_info()->department_id_ = account_info.department_id;
     
    app_->user_broker_info(p_broker_info);

    done(QDialog::Accepted);
}