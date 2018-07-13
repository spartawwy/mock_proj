#ifndef COOKIE_H_DSF3DSFS_
#define COOKIE_H_DSF3DSFS_
#include <windows.h>
 
#include <string>
#include <cassert>

class Cookie
{
public:

    enum class TRetCookie : char
    {
        OK = 0,
        ERROR_FILE_OPEN,
        ERROR_OTHER,
    };
    struct  T_DataAccess
    {
        int max_task_id;
        long long max_fill_id;
		int latest_date_tag;
        //std::string name;
    };

    Cookie();
    ~Cookie();
     
    TRetCookie Init();
    
	void latest_date_tag(int val) { assert(data_); data_->latest_date_tag = val; }
	int latest_date_tag() { assert(data_); return data_->latest_date_tag; }
   /* void max_task_id(int id) { assert(data_); data_->max_task_id = id; }
    int max_task_id() { assert(data_); return data_->max_task_id; }*/
    T_DataAccess *data_;

private:

     //T_DataAccess * data() { return data_; }

     char * mmfm_base_address_;
     HANDLE mmfm_;
     HANDLE mmHandle_;

     DWORD mmf_size_;
     size_t view_size_;
};

#endif