#include <iostream>
#include <sstream>

#include "detail_file.h"

static const unsigned int cst_size_limit = 10*1024*1024;

std::string DetailFile::FileEndStr(int index)
{
    return "." + std::to_string(index) + ".txt";
}

DetailFile::DetailFile(const std::string &dir, bool is_auto_flash)
    : dir_(dir)
    , file_name_() 
    , is_auto_flash_(is_auto_flash)
    , index_(0)
    , read_index_(0)
{
    
}

DetailFile::~DetailFile()
{
    if( file_.is_open() )
        file_.close();
    if( read_file_.is_open() )
        read_file_.close();
}

bool DetailFile::Init(const std::string &file_name)
{
    file_name_ = file_name;
    std::string file_full_path = dir_ + "/" + file_name_ + FileEndStr(index_);
    file_.open(file_full_path, std::fstream::out | std::fstream::in | std::ios::app/*, _SH_DENYRW*/);
     
    bool ret = file_.is_open();
    if( ret ) ++index_;
    return ret;
}

void DetailFile::ClearContent()
{ 
    int index = file_.is_open() ? (index_ - 1) : index_;
    std::string file_full_path = dir_ + "/" + file_name_ + FileEndStr(index);

    if( file_.is_open() )
        file_.close();
    file_.clear();

    file_.open(file_full_path, std::fstream::out | std::ios::trunc);
    file_ << "" << std::endl; 
    if( file_.is_open() )
        file_.close();
    file_.clear();
    
    file_.open(file_full_path, std::fstream::out | std::fstream::in | std::ios::app);
}

void DetailFile::Write(const std::string &content)
{
    if( file_.is_open() )
    {
        file_ << content << std::endl;
        if( is_auto_flash_ )
            file_.flush();
        if( file_.tellg().seekpos() > cst_size_limit )
        {
            file_.close();
            file_.clear();
            std::string file_full_path = dir_ + "/" + file_name_ + FileEndStr(index_);
            file_.open(file_full_path, std::fstream::out | std::ios::app);
            if( file_.is_open() )
                ++index_;
        }
    }
}

std::string DetailFile::Readline()
{ 
    char buf[1024] = {0}; 
    if( !read_file_.is_open() )
    { 
        std::string file_full_path = dir_ + "/" + file_name_ + FileEndStr(read_index_++);
        read_file_.open(file_full_path);
        if( !read_file_.is_open() )
            return "";
    }
     
    if( read_file_.getline(buf, sizeof(buf)) )
        return buf;
    else
    {
        read_file_.close();
        read_file_.clear();
        std::string file_full_path = dir_ + "/" + file_name_ + FileEndStr(read_index_++);
        read_file_.open(file_full_path);
        if( !read_file_.is_open() )
            return "";
        if( read_file_.getline(buf, sizeof(buf)) )
            return buf;
        else 
            return "";
    }
}

void DetailFile::ResetRead()
{
    read_file_.close();
    read_file_.clear();
    read_index_ = 0;
}