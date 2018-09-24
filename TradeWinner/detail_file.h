#ifndef DETAIL_FILE_H_DSFSD
#define DETAIL_FILE_H_DSFSD

#include <fstream>

class DetailFile
{
public:

    DetailFile(const std::string &file_full_path, bool is_auto_flash = true);
    ~DetailFile();
    bool Init();

    void Write(const std::string &content);
    std::string Readline();
    void ResetRead();

private:

    std::fstream file_;
    std::string file_full_path_;
    bool is_auto_flash_;
    int index_;

    std::ifstream  read_file_;
    int read_index_;
};

#endif // DETAIL_FILE_H_DSFSD