#include "records_win.h"

#include <qstandarditemmodel>

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

const int cst_fills_col_count = 7;
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
 
RecordsWin::RecordsWin(WinnerApp *app) : app_(app)
{
	ui.setupUi(this);

	QStandardItemModel * model = new QStandardItemModel(0, cst_fills_col_count, this);
    model->setHorizontalHeaderItem(cst_fills_col_index_id, new QStandardItem(QString::fromLocal8Bit("记录号")));
    model->horizontalHeaderItem(cst_fills_col_index_id)->setTextAlignment(Qt::AlignCenter);
    //model->set
	model->setHorizontalHeaderItem(cst_fills_col_index_date, new QStandardItem(QString::fromLocal8Bit("日期")));
    model->horizontalHeaderItem(cst_fills_col_index_date)->setTextAlignment(Qt::AlignCenter);
    
	model->setHorizontalHeaderItem(cst_fills_col_index_time, new QStandardItem(QString::fromLocal8Bit("时间")));
    model->horizontalHeaderItem(cst_fills_col_index_time)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_stock, new QStandardItem(QString::fromLocal8Bit("证券代码")));
    model->horizontalHeaderItem(cst_fills_col_index_stock)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_pinyin, new QStandardItem(QString::fromLocal8Bit("证券名称")));
    model->horizontalHeaderItem(cst_fills_col_index_pinyin)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_buysell, new QStandardItem(QString::fromLocal8Bit("买卖")));
    model->horizontalHeaderItem(cst_fills_col_index_buysell)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_quantity, new QStandardItem(QString::fromLocal8Bit("数量")));
    model->horizontalHeaderItem(cst_fills_col_index_quantity)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_price, new QStandardItem(QString::fromLocal8Bit("价格")));
    model->horizontalHeaderItem(cst_fills_col_index_price)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_amount, new QStandardItem(QString::fromLocal8Bit("金额")));
    model->horizontalHeaderItem(cst_fills_col_index_amount)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_fills_col_index_fee, new QStandardItem(QString::fromLocal8Bit("手续费")));
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

}

void RecordsWin::ShowUI(const QString &title_str, const QString &str)
{

}

void RecordsWin::UpdateTblviewFills()
{
	std::vector<std::shared_ptr<T_FillItem> > records = app_->db_moudle().LoadAllFillRecord();

	static auto str_buy = QString::fromLocal8Bit("买");
	static auto str_sell = QString::fromLocal8Bit("卖");

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
}