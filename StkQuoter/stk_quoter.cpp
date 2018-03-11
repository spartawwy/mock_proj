// StkQuoter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "stk_quoter_api.h"

#include <iostream>  
//#include <fstream>  
#include "afxinet.h"  

#include <memory>
#include <string>
#include <assert.h>

#include "string_function.h"

extern "C" int STKQUOTER_IMEXPORT  StkQuoteGetQuote(char stocks[][16], const unsigned int size, T_StockPriceInfo price_info[]/*, unsigned int *ret_size*/)
{

    CInternetSession session((LPCTSTR)"by_dreamcatcher"); //"建立会话"  
    CStdioFile* sessionfile = NULL;   
 
    std::string sinaurl("http://hq.sinajs.cn/list=");
    bool is_first = true;
    for( int i = 0; i < size; ++i )
    {
        if( strlen(stocks[i]) != 6 )
            continue;
        if( is_first )
            is_first = false;
        else
            sinaurl += ",";

        if( stocks[i][0] == '6' || !strcmp(stocks[i], "000001") )
            sinaurl += "sh";
        else
            sinaurl += "sz";
        sinaurl += stocks[i];
    }

    try
    {  
        sessionfile = session.OpenURL((LPCTSTR)sinaurl.c_str());  
    }catch(CInternetException& ex)
    {  
        std::cout << "error" << " open request faile:"  
            <<"dwcontext:" << ex.m_dwContext << "\tdwErrorCode:" << ex.m_dwError << std::endl;  
        ex.Delete();  
    }

    const unsigned int buf_size = 1024*4;
    char *p_buffer = new char[buf_size];
    memset(p_buffer, 0, buf_size);

    unsigned int getlen = 0;
    unsigned int pos = 0;
    do
    {
        getlen = sessionfile->Read(p_buffer + pos, 300);  
        pos += getlen;

    }while (getlen > 0);

    //std::cout << p_buffer << std::endl;
    int index = 0;

    auto stock_raw_lines = split(p_buffer, ";");
    if( stock_raw_lines.size() < size )
       goto EXIT_PROC;
#if 1 
    
    std::for_each( std::begin(stock_raw_lines), std::end(stock_raw_lines), [&](std::string &line)
    {
        if( index >= size )
            return;

        auto blocks = split(line, ",");
        strcpy_s(price_info[index].code, stocks[index]);
        if( blocks.size() > 2 )
        {
            price_info[index].open_price = atof(blocks.at(1).c_str());
            price_info[index].pre_close_price = atof(blocks.at(2).c_str());
            price_info[index].cur_price = atof(blocks.at(3).c_str());
        }else
        {
            price_info[index].open_price = 0.0;
            price_info[index].pre_close_price = 0.0;
            price_info[index].cur_price = 0.0;
        }
        ++ index;
    });

#endif

EXIT_PROC:

    delete [] p_buffer;

    return index;
}

//ps: use delete[] to release datas; out : count--can't be null
extern "C" int STKQUOTER_IMEXPORT  StkHisData(char stocks[16], int start_date, int end_date, T_StockHisDataItem **datas)
{
	const unsigned int buf_size = 1024*10;
	static auto do_move = [buf_size](char *target, char *src, unsigned int &position)
	{
		/*if( buf_size < 1024*10 )
			position = position;*/
		position = strlen(src);
		auto old_len = strlen(target); 
		memmove(target, src, position);
		assert( buf_size >= position);
		memset(target + position, 0, buf_size - position);
	};

    int count = 0;
    CInternetSession session((LPCTSTR)"by_dreamcatcher"); //"建立会话"  
    CStdioFile* sessionfile = NULL;   
 
    std::string strurl("http://quotes.money.163.com/service/chddata.html?code=");
      
    if( strlen(stocks) != 6 )
        return 0; //continue;
 
    if( stocks[0] == '6' )
        strurl += "0";
    else
        strurl += "1";
    strurl += stocks;
 
    strurl += "&start=";
    strurl += std::to_string(start_date);
    strurl += "&end=";
    strurl += std::to_string(end_date);

    strurl += "&fields=TCLOSE;HIGH;LOW;TOPEN;LCLOSE;VOTURNOVER;VATURNOVER";

    try
    {  
        sessionfile = session.OpenURL((LPCTSTR)strurl.c_str());  
    }catch(CInternetException& ex)
    {  
        std::cout << "error" << " open request faile:"  
            <<"dwcontext:" << ex.m_dwContext << "\tdwErrorCode:" << ex.m_dwError << std::endl;  
        ex.Delete();  
    }
	

    *datas = new T_StockHisDataItem[1024];
    T_StockHisDataItem * p_data_array = *datas;
      
    
    char *p_buffer = new char[buf_size];
    memset(p_buffer, 0, buf_size);

    unsigned int getlen = 0;
    unsigned int pos = 0;
    bool is_head_line = true;
#if 1
    do
    {
        getlen = sessionfile->Read(p_buffer + pos, 40);  
        pos += getlen; 

        char *p_rem = strstr(p_buffer, "\r\n");
        if( !p_rem )
            continue;
		OutputDebugString(p_buffer);
        *p_rem = '\0';
 
        p_rem += 2;
         
        if( is_head_line )
        {
            is_head_line = false;
            
           /* pos = strlen(p_rem);
            memmove(p_buffer, p_rem, pos);
            p_buffer[pos] = '\0';*/
            do_move(p_buffer, p_rem, pos);
            continue;
        }
        auto quotes = split(p_buffer, ",");
        if( quotes.size() < 10 )
        {
            do_move(p_buffer, p_rem, pos);
            continue;
        }
        auto data_str = quotes.at(0);
        auto year_str = data_str.substr(0, 4);
        auto mon_str = data_str.substr(5, 2);
        auto day_str = data_str.substr(8, 2);

        p_data_array[count].date = atoi( year_str.c_str() ) * 10000 + atoi( mon_str.c_str() ) * 100 + atoi( day_str.c_str() );
        p_data_array[count].close_price = atof(quotes.at(3).c_str());
        p_data_array[count].high_price = atof(quotes.at(4).c_str());
        p_data_array[count].low_price = atof(quotes.at(5).c_str());
        p_data_array[count].open_price = atof(quotes.at(6).c_str());
        p_data_array[count].pre_close_price = atof(quotes.at(7).c_str());
        p_data_array[count].vol = atof(quotes.at(8).c_str());
        p_data_array[count].capital = atof(quotes.at(9).c_str());
        
        ++ count;

        do_move(p_buffer, p_rem, pos);

    }while (getlen > 0);
#else
	do
	{
		getlen = sessionfile->Read(p_buffer + pos, 40);  
		pos += getlen; 
	 }while (getlen > 0);
	OutputDebugString(p_buffer);
#endif
    //std::cout << p_buffer << std::endl;
    std::cout << p_buffer << std::endl;
      

EXIT_PROC:

    delete [] p_buffer;

    return count;
}

extern "C" void STKQUOTER_IMEXPORT StkRelHisData(T_StockHisDataItem *p_items)
{
    if( p_items )
        delete[] p_items;
}