#ifndef STRING_FUNCTION_H_SDF3SDFSD_
#define STRING_FUNCTION_H_SDF3SDFSD_
 

#include <algorithm>
#include <regex>

unsigned int KMP(const char* text, size_t text_length, const char* pattern, size_t pattern_length, unsigned int* matches)  ;

std::vector<std::string> split(const std::string& source, const std::string& seperator, 
		const std::string& quotation = "\"")
{
    if( source.empty() ) 
        return std::vector<std::string>();

	auto p_source = source.c_str();

	unsigned int match_result[1024];
	memset(match_result, -1, sizeof(match_result));
	 
	auto num = KMP(source.c_str(), source.length(), seperator.c_str(), seperator.length(), match_result);

	std::vector<std::string> final_results;

	const char *p1 = nullptr;
	const char *p2 = nullptr;
	
	if( num == 0 )
	{
		final_results.push_back(source);
		return final_results;
	}else if( num == 1)
	{
		if( match_result[0] == 0 ) 
		{
			final_results.emplace_back("");
			final_results.emplace_back(p_source + seperator.length(), p_source + source.length());

		}else if( match_result[0] + seperator.length() != source.length() ) 
		{
			final_results.emplace_back(p_source, p_source + match_result[0]);
			final_results.emplace_back(p_source + match_result[0] + seperator.length(), p_source + source.length()/* - 1*/);

		}else
		{
			final_results.emplace_back(p_source, p_source + match_result[0]);
			final_results.emplace_back("");
		}
		
		return final_results;
	}

	auto i = 0;
	for(; i < num - 1; ++i)
	{
		if( i == 0 )
		{
			if( match_result[0] == 0 )
				final_results.emplace_back("");
			else
				final_results.emplace_back(p_source, p_source + match_result[0]);
			p1 = p_source + match_result[0] + seperator.length();
			p2 = p_source + match_result[1];

		}else 
		{
			p1 = p_source + match_result[i] + seperator.length();
			p2 = p_source + match_result[i+1];
		}

		final_results.emplace_back(p1, p2);
	}

	p2 = p_source + match_result[num-1];
	 
	if( p2 == p_source + source.length() - 1 )
	{
		final_results.emplace_back(p2, p2);

	}else if( p2 + seperator.length() != p_source + source.length() ) // not point to end
	{
		final_results.emplace_back(p2 + seperator.length(), p_source + source.length());
	}
    
    if( seperator == " " )
    {
        for( auto iter = final_results.begin(); iter != final_results.end(); )
        {
            if( *iter == " " || *iter == "" )
            {
                iter = final_results.erase(iter);
            }else
            {
                ++iter;
            }
        }    
    }
	return final_results;
}

inline void BuildNext(const char* pattern, size_t length, unsigned int* next)  
{  
    unsigned int i, t;  
  
    i = 1;  
    t = 0;  
    next[1] = 0;  
  
    while(i < length + 1)  
    {  
        while(t > 0 && pattern[i - 1] != pattern[t - 1])  
        {  
            t = next[t];  
        }  
  
        ++t;  
        ++i;  
        if( i >= length + 1 ) 
            break;
        if(pattern[i - 1] == pattern[t - 1])  
        {  
            next[i] = next[t];  
        }  
        else  
        {  
            next[i] = t;  
        }  
    }  
  
    //pattern末尾的结束符控制，用于寻找目标字符串中的所有匹配结果用  
	// control of pattern's end, used to search all meets results in target string
    while(t > 0 && pattern[i - 1] != pattern[t - 1])  
    {  
        t = next[t];  
    }  
  
    ++t;  
    ++i;  
  
    if( i < length )
        next[i] = t;   
}

unsigned int KMP(const char* text, size_t text_length, const char* pattern, size_t pattern_length, unsigned int* matches)  
{  
    unsigned int i, j, n;  
	unsigned int next[1024] = {0};  
  
    BuildNext(pattern, pattern_length, next);  
  
    i = 0;  
    j = 1;  
    n = 0;  
  
    while(pattern_length + 1 - j <= text_length - i)  
    {  
        if(text[i] == pattern[j - 1])  
        {  
            ++i;  
            ++j;  
  
            //发现匹配结果，将匹配子串的位置，加入结果  
            if(j == pattern_length + 1)  
            {  
                matches[n++] = i - pattern_length;  
                j = next[j];  
            }  
        }  
        else  
        {  
            j = next[j];  
  
            if(j == 0)  
            {  
                ++i;  
                ++j;  
            }  
        }  
    }  
  
    //返回发现的匹配数  
    return n;  
}  

std::string&   replace_all(std::string&   str, const   std::string&   old_value, const   std::string&   new_value)   
{   
    while(true)   
    {   
        std::string::size_type   pos(0);   
        if(   (pos=str.find(old_value)) != std::string::npos   )   
            str.replace(pos,old_value.length(),new_value);   
        else   break;   
    }   
    return   str;   
}   

std::string&   replace_all_distinct(std::string&   str, const   std::string&   old_value, const  std::string&   new_value)   
{   
    for(std::string::size_type   pos(0);   pos != std::string::npos;   pos+=new_value.length())   
    {   
        if(   (pos=str.find(old_value,pos)) != std::string::npos   )   
            str.replace(pos,old_value.length(),new_value);   
        else   break;   
    }   
    return   str;   
}   

#endif