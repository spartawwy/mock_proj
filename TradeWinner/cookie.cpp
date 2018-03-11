#include "cookie.h"

#include <direct.h>

//#include <Windows.h>

#define BAD_POS 0xFFFFFFFF // returned by SetFilePointer and GetFileSize
#define SUCCESS 0
using namespace std;


Cookie::Cookie() : mmfm_base_address_(nullptr), mmfm_(INVALID_HANDLE_VALUE), mmHandle_(INVALID_HANDLE_VALUE)
    , mmf_size_(32*1024), view_size_(16*1024), data_(nullptr)
{

}

Cookie::~Cookie()
{
    UnmapViewOfFile(mmfm_base_address_);
     
    if( mmfm_ != INVALID_HANDLE_VALUE )
        CloseHandle(mmfm_);
    // close file handle
    if( mmHandle_ != INVALID_HANDLE_VALUE )
        CloseHandle(mmHandle_);
}

Cookie::TRetCookie Cookie::Init()
{ 
    DWORD error_code;
    TRetCookie  ret = TRetCookie::OK;

    //const char* shared_name = "cookietrdw";
    const char* shared_name = "cookietrdw10";
     
    _mkdir("c:\\trade_localf");

    //mmf_flags flags = FILE_FLAG_SEQUENTIAL_SCAN;//|FILE_FLAG_WRITE_THROUGH|FILE_FLAG_NO_BUFFERING;

    mmfm_ = INVALID_HANDLE_VALUE;
    // create file
    mmHandle_ = CreateFile("c:\\trade_localf\\my.cookie10",
             GENERIC_READ | GENERIC_WRITE,  //access_mode
             FILE_SHARE_READ | FILE_SHARE_WRITE, //share_mode
             NULL,
             OPEN_ALWAYS,
             FILE_FLAG_SEQUENTIAL_SCAN,
             NULL);
 
    if (mmHandle_ == INVALID_HANDLE_VALUE) 
    {
       error_code = GetLastError();
       ret = TRetCookie::ERROR_OTHER;
        //  throw exception
       goto EXIT_PRO;
        
    } 
     
    DWORD high_size;
    DWORD file_size = GetFileSize(mmHandle_, &high_size);
    if (file_size == BAD_POS && (error_code = GetLastError()) != SUCCESS)
    {
        ret = TRetCookie::ERROR_OTHER;
        goto EXIT_PRO;
    }
  
    DWORD size_high = 0;
    //创建文件映射，如果要创建内存页面文件的映射，第一个参数设置为INVALID_HANDLE_VALUE
    mmfm_ = CreateFileMapping(mmHandle_,
                                    NULL,
                                    PAGE_READWRITE,
                                    size_high,
                                    mmf_size_,
                                    shared_name);
 
    error_code = GetLastError();
    if(SUCCESS != error_code || mmfm_ == NULL )   
    {             
        ret = TRetCookie::ERROR_FILE_OPEN; 
        goto EXIT_PRO;
    } 
                  
  /*  char write_chars[] = "hello chars"; 
     const size_t write_chars_size = sizeof(write_chars);*/
    //WriteFile(mmHandle,write_chars,write_chars_size,&written,NULL);
    //size_t view_size = 1024*256; 
  
    mmfm_base_address_ = (char*)MapViewOfFile(mmfm_, FILE_MAP_ALL_ACCESS, 0/*fileofsethigh*/, 0/*fileofsetlow*/, view_size_);
    if(mmfm_base_address_ == NULL)
    {
        error_code = GetLastError();
        if(error_code != SUCCESS)
        {
            // cout << "error code " << error_code << endl;
        }
        ret = TRetCookie::ERROR_FILE_OPEN;
        goto EXIT_PRO;

    } 

    data_ = reinterpret_cast<T_DataAccess*>(mmfm_base_address_);
         
    return TRetCookie::OK;
 
EXIT_PRO:
     
    UnmapViewOfFile(mmfm_base_address_);
    CloseHandle(mmfm_);
    mmfm_ = INVALID_HANDLE_VALUE;
    CloseHandle(mmHandle_);
    mmHandle_ = INVALID_HANDLE_VALUE;
    return ret;
}
