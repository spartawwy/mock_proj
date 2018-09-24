#include <iostream>
#include <sstream>

#include "detail_file.h"

static const unsigned int cst_size_limit = 10*1024*1024;

DetailFile::DetailFile(const std::string &file_full_path, bool is_auto_flash)
    : file_full_path_(file_full_path) 
    , is_auto_flash_(is_auto_flash)
    , index_(0)
    , read_index_(0)
{
    std::string file_path = file_full_path_ + "." + std::to_string(index_++);
    file_.open(file_path, std::fstream::out | std::ios::app);
}

DetailFile::~DetailFile()
{
    if( file_.is_open() )
        file_.close();
    if( read_file_.is_open() )
        read_file_.close();
}

bool DetailFile::Init()
{
    return file_.is_open();
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
            std::string file_path = file_full_path_ + "." + std::to_string(index_++);
            file_.open(file_path, std::fstream::out | std::ios::app);
        }
    }
}

std::string DetailFile::Readline()
{ 
    char buf[1024] = {0};
    /* std::string tmp;
    tmp.max_size();*/
    if( !read_file_.is_open() )
    {
        std::string file_path = file_full_path_ + "." + std::to_string(read_index_++);
        read_file_.open(file_path);
        if( !read_file_.is_open() )
            return "";
    }
     
    if( read_file_.getline(buf, sizeof(buf)) )
        return buf;
    else
    {
        read_file_.close();
        read_file_.clear();
        std::string file_path = file_full_path_ + "." + std::to_string(read_index_++);
        read_file_.open(file_path);
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