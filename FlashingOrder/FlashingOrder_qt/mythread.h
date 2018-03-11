#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QString>

class FlashingOrder;
class MyThread : public QThread
{
public:
	MyThread(FlashingOrder *app);
	~MyThread(){};

	void run() override;

	void stock_name(QString &name) { stock_name_ = name; }

private:

	FlashingOrder *app_;
	QString stock_name_;
};
#endif // MYTHREAD_H  
