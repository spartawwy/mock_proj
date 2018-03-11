#include "winner_app.h"
#include <QtWidgets/QApplication>
#include <qtextcodec.h>

//#include <boost/lexical_cast.hpp>

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>

#include "winner_app.h"

using namespace TSystem;
 
#include "VMProtectSDK.h"

#pragma comment(lib, "VMProtectSDK32.lib")
void print_state(INT state)
{
    if (state& SERIAL_STATE_FLAG_CORRUPTED)
    {
	    MessageBox(NULL,"许可文件损坏", "错误", 0);
    }
    else if(state & SERIAL_STATE_FLAG_INVALID)
    {
	    MessageBox(NULL,"许可文件无效", "错误", 0);
    }
    else if(state & SERIAL_STATE_FLAG_BLACKLISTED)
    {
	    MessageBox(NULL,"许可文件被列入黑名单", "错误", 0);
    }
    else if(state & SERIAL_STATE_FLAG_DATE_EXPIRED)
    {
	    MessageBox(NULL,"许可文件过期", "错误", 0);
    }
    else if(state & SERIAL_STATE_FLAG_RUNNING_TIME_OVER)
    {
	    MessageBox(NULL,"许可文件时限到", "错误", 0);
    }
    else if(state & SERIAL_STATE_FLAG_BAD_HWID)
    {
	    MessageBox(NULL,"硬件ID不符","错误", 0);
    }
    else if(state & SERIAL_STATE_FLAG_MAX_BUILD_EXPIRED)
    {
	    MessageBox(NULL,"版本过期", "错误", 0);
    }
}

char * read_serial(const char *fname)
{
	FILE *f =fopen(fname, "rb");
	if (NULL== f) return NULL;
	fseek(f,0, SEEK_END);
	int s =ftell(f);
	fseek(f,0, SEEK_SET);
	char *buf= new char[s + 1];
	fread(buf,s, 1, f);
	buf[s] =0;
	fclose(f);
	return buf;
}

bool ValidSerial()
{
	char* serial = read_serial("tradeWinner.lic");
	int res = VMProtectSetSerialNumber(serial);
	delete[] serial;
	if (res)
	{
		print_state(res);
		return false;
	}
	return TRUE;
}


class TheFrame : public TSystem::ExecutableFrame
{
 
public:
    int main(int argc, char* argv[])
    {
    TSystem::utility::ProjectTag("WZF");
#ifdef PUBLISH
    if( !ValidSerial() )
        return -1;
#else
	QCoreApplication::addLibraryPath("D:\\Qt\\qt5.2.1_win32\\bin\\plugins");
#endif
    QCoreApplication::addLibraryPath(".");
	QCoreApplication::addLibraryPath("./plugins");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(APP_CODE_TEXT));
     
    //-------------test
    
    //---------------end

    WinnerApp  winner_app(argc, argv);
    int ret = 0;
 
    PrintAppInfo(winner_app);
   
    if( winner_app.Init() )
    { 
        PrintLaunchDone();
        ret = winner_app.exec(); 
    }else
        winner_app.Stop();
     
    winner_app.WaitShutdown();
   
    return ret;
    }
};

int main(int argc, char* argv[])
{
   TheFrame frame;
   return frame.LaunchServer(argc, argv);
}