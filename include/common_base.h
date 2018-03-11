#ifndef COMMON_BASE_SDF878DF_H_
#define COMMON_BASE_SDF878DF_H_

#include <cstring>
#include <cassert>
// 0资金  1股份   2当日委托  3当日成交     4可撤单   5股东代码  6融资余额   7融券余额  8可融证券</param>
enum class TypeQueryCategory : char
{
    CAPITAL = 0,
    STOCK = 1,
    TODAY_DELEGATE = 2,
    TODAY_FILL = 3,
    CAN_RECALL_ORDER,
    SHARED_HOLDER_CODE = 5,
    RZYE,
    RQYE,
    KRZQ,
};

enum class TypeMarket : char
{
    SZ = 0,
    SH
};

struct T_AccountData
{
    char shared_holder_code[64]; 
    char name[64];
    TypeMarket type;
    char capital_code[64]; 
    char seat_code[64];
    char rzrq_tag[64];  //融资融券 标识
};


struct T_PositionData
{
	char code[32];
	char pinyin[32];
	int total;
	int avaliable;
	double cost;
	double value;
	double profit;
	double profit_percent;
    T_PositionData() : total(0), avaliable(0), cost(0.0), value(0.0), profit(0.0), profit_percent(0.0)
    { 
        code[0] = '\0'; pinyin[0] = '\0';
    }
	T_PositionData(const T_PositionData &lh) : total(lh.total),avaliable(lh.avaliable), cost(lh.cost), value(lh.value), profit(lh.profit), profit_percent(lh.profit_percent)
    {
        strcpy_s(code, lh.code); strcpy_s(pinyin, lh.pinyin); 
    }
    T_PositionData& operator = (const T_PositionData &lh)
    {
        if( this == &lh ) return *this;
        strcpy_s(code, lh.code); 
        strcpy_s(pinyin, lh.pinyin); 
        total = lh.total; 
        avaliable = lh.avaliable; 
        cost = lh.cost; 
        value = lh.value; 
        profit = lh.profit; 
        profit_percent = lh.profit_percent; 
        return *this;
    }
	/*T_PositionData(const T_PositionData &&lh) : total(lh.total),avaliable(lh.avaliable), cost(lh.cost), value(lh.value), profit(lh.profit), profit_percent(lh.profit_percent)
    { 
    }*/
};

struct T_Capital
{
    double remain;
    double available;
    double total;
    T_Capital(): remain(0.0), available(0.0), total(0.0){}
    T_Capital(const T_Capital&lh) : remain(lh.remain), available(lh.available), total(lh.total) {}
    T_Capital& operator = (const T_Capital&lh) 
    {
        if( this == &lh ) return *this;
        remain = lh.remain; available = lh.available; total = lh.total;
        return *this;
    }
};

class Buffer
{
public:
	// notice size + 1 == 1024 is suitable for avoid memory fragment
    explicit Buffer(unsigned int size=64) : size_(size), p_data_(nullptr) 
    {
        p_data_ = new char[size + 1];
        memset(p_data_, 0, size + 1); 
    }
    explicit Buffer(const char* p_str, unsigned int size) 
    {
        assert(p_str);
        p_data_ = new char[size + 1];
        size_ = size;
        memcpy(p_data_, p_str, size);
        p_data_[size] = '\0';
    } 

    ~Buffer(){ if( p_data_ ) delete[] p_data_; p_data_ = nullptr;};

    char * data() { return p_data_; }
    const char *c_data() const {return p_data_;}
    unsigned int size() const {return size_;}
    void reset() { memset(p_data_, 0, size_ + 1); }

private:

    Buffer(Buffer&);
    Buffer& operator = (Buffer&);

    unsigned int size_;
    char *p_data_;
};

#endif // COMMON_BASE_SDF878DF_H_