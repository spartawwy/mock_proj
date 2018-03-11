#include "login_win.h"

#include <qmessagebox.h>
#include "winner_app.h"
#include "rwini.h"

 static const std::string cst_autofill_str = "autofill";

LoginWin::LoginWin(WinnerApp* app)
    : app_(app)
	, ini_obj_(nullptr)
{
    ui_.setupUi(this);

    auto ret = connect(ui_.okButton, SIGNAL(clicked()), this, SLOT(DoOkBtnClicked()));
    ui_.le_pwd->setEchoMode(QLineEdit::Password);
    //setAttribute(Qt::WA_DeleteOnClose);
	ini_obj_ = std::make_shared<Crwini>((app_->applicationDirPath().append("\\config.ini").toLocal8Bit().data()));
	if( !ini_obj_ )
		app_->local_logger().LogLocal("error: open config.ini fail!");
	if( ini_obj_ && ini_obj_->ReadInt("user", "autofill") == 1 )
	{ 
		ui_.cb_remember_userpwd->setCheckState(Qt::Checked);
		auto name = ini_obj_->ReadString("user", "name");
		ui_.le_name->setText(name.c_str());
		ui_.le_pwd->setText(ini_obj_->ReadString("user", "pwd").c_str());
	}
}

void LoginWin::closeEvent(QCloseEvent* e)
{
    
}

void LoginWin::Init()
{
    //setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    ////setWindowModality(Qt::ApplicationModal);//设置阻塞类型
    //setAttribute(Qt::WA_ShowModal, true);
}

void LoginWin::DoOkBtnClicked()
{ 
    // todo: read encode use pearid date in sn file
    T_UserInformation user_info;
    auto ret = app_->db_moudle().CheckLogin(ui_.le_name->text().trimmed().toLocal8Bit().data(), ui_.le_pwd->text().trimmed().toLocal8Bit().data()
        , &user_info);

    if( ret < 0 )
    {
        QMessageBox::information(nullptr, "info", QString::fromLocal8Bit("用户名或密码错误!"));

    }else
    {
		if( ini_obj_  )
		{
			if( ui_.cb_remember_userpwd->isChecked() )
			{
				if( ini_obj_->ReadInt("user", cst_autofill_str) != 1 )
					ini_obj_->WriteInt("user", cst_autofill_str, 1);
				ini_obj_->WriteString("user", "name", ui_.le_name->text().trimmed().toLocal8Bit().data());
				ini_obj_->WriteString("user", "pwd", ui_.le_pwd->text().toLocal8Bit().data());
			}else
			{
				ini_obj_->WriteInt("user", cst_autofill_str, 0);
			}
		}
        app_->user_info(user_info);
        done(QDialog::Accepted);
    }
}
