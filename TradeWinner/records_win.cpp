#include "records_win.h"

#include <qstandarditemmodel>

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"
#include "stock_ticker.h"

const int cst_fills_col_count = 10;
const int cst_fills_col_index_id = 0;
const int cst_fills_col_index_date = 1;
const int cst_fills_col_index_time = 2;
const int cst_fills_col_index_stock = 3;
const int cst_fills_col_index_pinyin = 4;
const int cst_fills_col_index_buysell = 5;
const int cst_fills_col_index_quantity = 6;
const int cst_fills_col_index_price = 7;
const int cst_fills_col_index_amount = 8;
const int cst_fills_col_index_fee = 9;
 
const int cst_pos_col_count					  = 9;
const int cst_pos_col_index_stock             =  0; 
const int cst_pos_col_index_pinyin            =  1;
const int cst_pos_col_index_pos               =  2;
const int cst_pos_col_index_ava               =  3;
const int cst_pos_col_index_cost_price        =  4;
const int cst_pos_col_index_curprice          =  5;
const int cst_pos_col_index_market_value      =  6;
const int cst_pos_col_index_proflost          =  7;
const int cst_pos_col_index_proflost_percent  =  8;

RecordsWin::RecordsWin(WinnerApp *app) : app_(app)
{
	ui.setupUi(this);

	//QTableView table records -----------------
	QStandardItemModel * model = new QStandardItemModel(0, cst_fills_col_count, this);
    model->setHorizontalHeaderItem(cst_fills_col_index_id, new QStandardItem(QString::fromLocal8Bit("��¼��")));
    model->horizontalHeaderItem(cst_fills_col_index_id)->setTextAlignment(Qt::AlignCenter);
    //model->set
	model->setHorizontalHeaderItem(cst_fills_col_index_date, new QStandardItem(QString::fromLocal8Bit("����")));
    model->horizontalHeaderItem(cst_fills_col_index_date)->setTextAlignment(Qt::AlignCenter);
    
	model->setHorizontalHeaderItem(cst_fills_col_index_time, new QStandardItem(QString::fromLocal8Bit("ʱ��")));
    model->horizontalHeaderItem(cst_fills_col_index_time)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_stock, new QStandardItem(QString::fromLocal8Bit("֤ȯ����")));
    model->horizontalHeaderItem(cst_fills_col_index_stock)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_pinyin, new QStandardItem(QString::fromLocal8Bit("֤ȯ����")));
    model->horizontalHeaderItem(cst_fills_col_index_pinyin)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_buysell, new QStandardItem(QString::fromLocal8Bit("����")));
    model->horizontalHeaderItem(cst_fills_col_index_buysell)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_quantity, new QStandardItem(QString::fromLocal8Bit("����")));
    model->horizontalHeaderItem(cst_fills_col_index_quantity)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_price, new QStandardItem(QString::fromLocal8Bit("�۸�")));
    model->horizontalHeaderItem(cst_fills_col_index_price)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_amount, new QStandardItem(QString::fromLocal8Bit("���")));
    model->horizontalHeaderItem(cst_fills_col_index_amount)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_fee, new QStandardItem(QString::fromLocal8Bit("������")));
    model->horizontalHeaderItem(cst_fills_col_index_fee)->setTextAlignment(Qt::AlignCenter);

	ui.tbview_fills->setModel(model);

	ui.tbview_fills->setColumnWidth(cst_fills_col_index_id, 60);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_date, 80);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_time, 80);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_stock, 60);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_pinyin, 60);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_buysell, 40);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_quantity, 80);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_price, 60);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_amount, 60);
	ui.tbview_fills->setColumnWidth(cst_fills_col_index_fee, 60);

	ui.tbview_fills->setEditTriggers(QAbstractItemView::NoEditTriggers);
	 
	// table records table position----------------------
	QStandardItemModel * model_pos = new QStandardItemModel(0, cst_pos_col_count, this); 
     
	model_pos->setHorizontalHeaderItem(cst_pos_col_index_stock, new QStandardItem(QString::fromLocal8Bit("֤ȯ����")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_stock)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_pinyin, new QStandardItem(QString::fromLocal8Bit("֤ȯ����")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_pinyin)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_pos, new QStandardItem(QString::fromLocal8Bit("֤ȯ����")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_pos)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_ava, new QStandardItem(QString::fromLocal8Bit("��������")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_ava)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_cost_price, new QStandardItem(QString::fromLocal8Bit("�ο��ɱ���")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_cost_price )->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_curprice, new QStandardItem(QString::fromLocal8Bit("��ǰ��")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_curprice)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_market_value, new QStandardItem(QString::fromLocal8Bit("������ֵ")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_market_value)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_proflost, new QStandardItem(QString::fromLocal8Bit("ӯ���ο�")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_proflost)->setTextAlignment(Qt::AlignCenter);

	model_pos->setHorizontalHeaderItem(cst_pos_col_index_proflost_percent, new QStandardItem(QString::fromLocal8Bit("ӯ������(%)")));
    model_pos->horizontalHeaderItem(cst_pos_col_index_proflost_percent)->setTextAlignment(Qt::AlignCenter);
	
	ui.tbview_position->setModel(model_pos);
	ui.tbview_position->setEditTriggers(QAbstractItemView::NoEditTriggers);

	ui.tbview_position->setColumnWidth(cst_pos_col_index_stock, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_pinyin, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_pos, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_ava, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_cost_price, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_curprice, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_market_value, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_proflost, 60);
	ui.tbview_position->setColumnWidth(cst_pos_col_index_proflost_percent, 90); 
}

void RecordsWin::ShowUI(const QString &title_str, const QString &str)
{

}

void RecordsWin::UpdateTblviewFills()
{
	std::list<std::shared_ptr<T_FillItem> > records = app_->db_moudle().LoadAllFillRecord(app_->user_info().id);

	static auto str_buy = QString::fromLocal8Bit("��");
	static auto str_sell = QString::fromLocal8Bit("��");

	QStandardItemModel * model = (QStandardItemModel*)(this->ui.tbview_fills->model());
	model->removeRows(0, model->rowCount());
	std::for_each( std::begin(records), std::end(records), [model, this](std::shared_ptr<T_FillItem>& entry)
	{ 
		auto align_way = Qt::AlignCenter;
		model->insertRow(model->rowCount());
		int row_index = model->rowCount() - 1;
		  
		//auto item = new QStandardItem( utility::FormatStr("%d", entry->id).c_str());
		auto item = new QStandardItem(QString("%1").arg(entry->id));
		model->setItem(row_index, cst_fills_col_index_id, item);

		item = new QStandardItem(QString("%1").arg(entry->date));
		model->setItem(row_index, cst_fills_col_index_date, item);
		 
		item = new QStandardItem(QString("%1").arg(entry->time_stamp));
		model->setItem(row_index, cst_fills_col_index_time, item);

		item = new QStandardItem(QString("%1").arg(entry->stock.c_str()));
		model->setItem(row_index, cst_fills_col_index_stock, item);

		item = new QStandardItem( QString("%1").arg(QString::fromLocal8Bit(entry->pinyin.c_str())) );
		model->setItem(row_index, cst_fills_col_index_pinyin, item);
		
		item = new QStandardItem(QString("%1").arg(entry->is_buy ? str_buy : str_sell));
		model->setItem(row_index, cst_fills_col_index_buysell, item);

		item = new QStandardItem(QString("%1").arg(entry->quantity));
		model->setItem(row_index, cst_fills_col_index_quantity, item);

		item = new QStandardItem(QString("%1").arg(entry->price));
		model->setItem(row_index, cst_fills_col_index_price, item);

		item = new QStandardItem(QString("%1").arg(entry->amount));
		model->setItem(row_index, cst_fills_col_index_amount, item);

		item = new QStandardItem(QString("%1").arg(entry->fee));
		model->setItem(row_index, cst_fills_col_index_fee, item);
	});

#if 0 
    // position view --------------------
	auto records_for_calprofit = app_->db_moudle().LoadFillRecordsForCalProfit(app_->user_info().id);
	char *stock[64] = {0};
	int i = 0;
	std::for_each( std::begin(records_for_calprofit), std::end(records_for_calprofit), [&i, &stock](T_CodeMapFills::reference entry)
	{
		stock[i++] = const_cast<char*>(entry.first.c_str());
	});
	TCodeMapQuotesData  stock_quotes;
	app_->stock_ticker().GetQuoteDatas(stock, i, stock_quotes);

	auto positions = app_->QueryPosition();

    model = (QStandardItemModel*)(this->ui.tbview_position->model());
    model->removeRows(0, model->rowCount());
     
    std::for_each( std::begin(records_for_calprofit), std::end(records_for_calprofit), [model, &positions, &stock_quotes, this](T_CodeMapFills::reference entry)
    {
		auto iter = stock_quotes.find(entry.first);
		if( iter == stock_quotes.end() )
		{
			// log_error:
			return; 
		}
		double cur_price = iter->second->cur_price;
		auto pos_iter  = positions.find(entry.first);
        if( pos_iter == positions.end() )
		{ // log_error:
			return; 
		}

		model->insertRow(model->rowCount());
		int row_index = model->rowCount() - 1;

        double input_amount = 0.0;
        double mid_get_amount = 0.0;
        std::for_each( std::begin(entry.second), std::end(entry.second), [model, &input_amount, &mid_get_amount, &entry, this](std::shared_ptr<T_FillItem>& in)
        {
            if( in->is_buy )
            {
                input_amount += in->amount + in->fee;
            }else
            {
                mid_get_amount += in->amount - in->fee;
            }
        });
		double market_value = cur_price * pos_iter->second.total;
        double profit = market_value + mid_get_amount - input_amount;
		double profit_percent = (profit * 100 / input_amount);

		// ���㹫ʽ���ɱ���=��������-ӯ����/�ֹɹ���
		double cost_price = 0.0;
		if( pos_iter->second.total > 0 )
			cost_price = (input_amount - profit) / pos_iter->second.total;
		  
		auto item = new QStandardItem(QString("%1").arg(entry.first.c_str()));
		model->setItem(row_index, cst_pos_col_index_stock, item);

		item = new QStandardItem( QString("%1").arg(QString::fromLocal8Bit(pos_iter->second.pinyin)) );
		model->setItem(row_index, cst_pos_col_index_pinyin, item);

		item = new QStandardItem(QString("%1").arg(pos_iter->second.total));
		model->setItem(row_index, cst_pos_col_index_pos, item); 

		item = new QStandardItem(QString("%1").arg(pos_iter->second.avaliable));
		model->setItem(row_index, cst_pos_col_index_ava, item);
		  
		item = new QStandardItem( QString("%1").arg(cost_price));
		model->setItem(row_index, cst_pos_col_index_cost_price, item);
 
		item = new QStandardItem( QString("%1").arg(cur_price));
		model->setItem(row_index, cst_pos_col_index_curprice, item);

		item = new QStandardItem( QString("%1").arg(market_value) );
		model->setItem(row_index, cst_pos_col_index_market_value, item);

		item = new QStandardItem( QString("%1").arg(profit) );
		model->setItem(row_index, cst_pos_col_index_proflost, item);

		item = new QStandardItem( QString("%1").arg(profit_percent) );
		model->setItem(row_index, cst_pos_col_index_proflost_percent, item);
		    
    });
#else

	T_CodeMapProfit profits = app_->CalcProfit();

	for(
#endif


}