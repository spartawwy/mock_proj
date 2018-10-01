#ifndef DETAIL_FILE_H_DSFSD
#define DETAIL_FILE_H_DSFSD

#include <fstream>

class DetailFile
{
public:

    static std::string FileEndStr(int index);

    DetailFile(const std::string &dir, bool is_auto_flash = true);
    ~DetailFile();
    bool Init(const std::string &file_name);

    void Write(const std::string &content);
    std::string Readline();
    void ResetRead();

private:
    
    std::fstream file_;
    std::string dir_;
    std::string file_name_;
    bool is_auto_flash_;
    int index_;

    std::ifstream  read_file_;
    int read_index_;
};

#endif // DETAIL_FILE_H_DSFSD