
#ifndef TICKER_SDF23DSF_H_
#define TICKER_SDF23DSF_H_

#include <assert.h>

#include <stdint.h>

#include <Windows.h>

#include "tdxhqapi.h"


class Buffer
{
public:

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

//using namespace TSystem;


class  QuotesData
{
public:
    int64_t time_stamp;
	double cur_price;
    double active_degree;
    double price_b_1;
    double price_s_1;
    double price_b_2;
    double price_s_2;
    double price_b_3;
    double price_s_3;
    double price_b_4;
    double price_s_4;
    double price_b_5;
    double price_s_5;
    QuotesData() 
        : time_stamp(0)
        , cur_price(0.0)  
        , active_degree(0.0)
        , price_b_1(0.0)
        , price_s_1(0.0)
        , price_b_2(0.0)
        , price_s_2(0.0)
        , price_b_3(0.0)
        , price_s_3(0.0)
        , price_b_4(0.0)
        , price_s_4(0.0)
        , price_b_5(0.0)
        , price_s_5(0.0)
    {
    }
};

class StockTicker
{
public:

     StockTicker();

     ~StockTicker();

    bool Init();
    void test();

};

#endif //TICKER_SDF23DSF_H_