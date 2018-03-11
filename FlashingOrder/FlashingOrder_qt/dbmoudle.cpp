#include "dbmoudle.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <QString>
#include <qdebug.h>

#include "flashingorder.h"

DBMoudle::DBMoudle(FlashingOrder *app)
    : p_database_(nullptr)
	, app_(app)
{
    p_database_ = std::make_shared<QSqlDatabase>(); //database;
   /* if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {*/
        *p_database_ = QSqlDatabase::addDatabase("QSQLITE");
        p_database_->setDatabaseName("pzwj.kd");
        //database.setUserName("XingYeZhiXia");
        //database.setPassword("123456");
    //}
}

#include <qvariant.h>

bool DBMoudle::Init(int broker_id, int account_id)
{
	if( !LoadBrokerByBrokerId(broker_id) || !LoadAccountInfoByAccountId(account_id) )
		return false;

    if( !p_database_->open() )
    {
        qDebug() << "Error: Failed to connect database." << p_database_->lastError();
        return false;
    }
	
    QSqlQuery sql_query;
    QString sql_str = "SELECT name, code FROM stock order by name ";
    sql_query.prepare(sql_str);
    if(!sql_query.exec())
    {
        qDebug() << "Error: Fail to SELECT name, code FROM stock." << sql_query.lastError();
		return false;
    }
    else
    {
		bool has_record = false;
        while(sql_query.next())
        { 
			has_record = true;
			std::string name = sql_query.value(0).toString().toLocal8Bit().data();
			std::string code = sql_query.value(1).toString().toLocal8Bit().data();
            qDebug() << QString("name:%1 code:%2 ").arg(sql_query.value(0).toString()).arg(sql_query.value(1).toString()) << "\n";
			app_->stock_name2code_.insert(std::make_pair(std::move(name), std::move(code)));
        }
		return has_record;
    }

}

bool DBMoudle::LoadBrokerByBrokerId(int broker_id)
{
	if( !p_database_->open() )
	{
		qDebug() << "Error: Failed to connect database." << p_database_->lastError();
		return false;
	}

	QSqlQuery sql_query;
	QString sql_str = QString("SELECT ip, port, type, remark, com_ver FROM BrokerInfo WHERE id=%1").arg(broker_id);
	sql_query.prepare(sql_str);
	if(!sql_query.exec())
	{
		qDebug() << "Error: Fail to SELECT ip, port, type, remark, com_ver FROM BrokerInfo." << sql_query.lastError();
		return false;
	}
	else
	{
		bool has_record = false;
		while(sql_query.next())
		{ 
			has_record = true;
			/* 
			qDebug() << QString("name:%1 code:%2 ").arg(sql_query.value(0).toString()).arg(sql_query.value(1).toString()) << "\n";*/
			app_->broker_info_.id = broker_id;
			app_->broker_info_.ip = sql_query.value(0).toString().toLocal8Bit().data();
			app_->broker_info_.port = sql_query.value(1).toInt();
			app_->broker_info_.type = (TypeBroker)sql_query.value(2).toInt();
			app_->broker_info_.remark = sql_query.value(3).toString().toLocal8Bit().data();
			app_->broker_info_.com_ver = sql_query.value(4).toString().toLocal8Bit().data();
		}
		return has_record;
	}
}

bool DBMoudle::LoadAccountInfoByAccountId(int account_id)
{
	if( !p_database_->open() )
	{
		qDebug() << "Error: Failed to connect database." << p_database_->lastError();
		return false;
	}

	QSqlQuery sql_query;
	QString sql_str = QString("SELECT account_no, trade_account_no, trade_pwd, comm_pwd, broker_id, department_id, remark FROM AccountInfo WHERE id=%1").arg(account_id);
	sql_query.prepare(sql_str);
	if(!sql_query.exec())
	{
		qDebug() << "Error: Fail to SELECT * FROM AccountInfo." << sql_query.lastError();
		return false;
	}
	else
	{
		if(sql_query.first())
		{ 
			/* 
			qDebug() << QString("name:%1 code:%2 ").arg(sql_query.value(0).toString()).arg(sql_query.value(1).toString()) << "\n";*/
			app_->account_info_.account_id = account_id; 
			app_->account_info_.account_no_in_broker_ = sql_query.value(0).toString().toLocal8Bit().data();
			app_->account_info_.trade_no_in_broker_ = sql_query.value(1).toString().toLocal8Bit().data();
			app_->account_info_.trade_pwd_ = sql_query.value(2).toString().toLocal8Bit().data();
			app_->account_info_.comm_pwd_ = sql_query.value(3).toString().toLocal8Bit().data();
			app_->account_info_.broker_id_ = sql_query.value(4).toInt();
			app_->account_info_.department_id_ = sql_query.value(5).toInt();
			return true;
		}
		return false;
	}
}