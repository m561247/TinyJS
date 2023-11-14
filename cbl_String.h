﻿#ifndef CBLSTRINGH
#define CBLSTRINGH
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
//#include <dirent.h>
#include <assert.h>
#ifdef linux
#define SOCKET_ERROR (-1)
#define O_BINARY (0)
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <netdb.h>
#include <string>
#else
#include <sys/stat.h>
#include <process.h>
//#include <dir.h>
#include <direct.h>
#include <io.h>
#endif
#define DELIMITER "/"
#ifndef IGNNRE_PARAMETER
#define IGNORE_PARAMETER(n) ((void)n)
#endif
#define MAXSAC 100
#define HTTP_STR_BUF_SIZE (1024*2)

class wString
{
private:
	unsigned int len;	//実際の長さ
	unsigned int capa;	//保存領域の長さ
	char* String;

	void replace_character_len(const char* sentence, int slen, const char* p, int klen, const char* rep);
	void replace_str(const char* sentence, unsigned int slen, const char* p, unsigned int klen, const char* rep);
	void set_binary(const void* mem, int binaryLength);
#ifndef va_copy
	int tsprintf_string(char* str);
	//int tsprintf_string(wString& str);
	int tsprintf_char(int ch);
	int tsprintf_decimal(signed long val, int zerof, int width);
	int tsprintf_decimalu(unsigned long val, int zerof, int width);
	int tsprintf_octadecimal(unsigned long val, int zerof, int width);
	int tsprintf_hexadecimal(unsigned long val, int capital, int zerof, int width);
	int vtsprintf(const char* fmt, va_list arg);
#endif
public:
	const static int npos;
	wString(void);
	explicit wString(int mylen);
	wString(const char* str);
	wString(const wString& str);
	~wString(void);
	//    static void     copy(wString* src,const wString* dst);
	//   static void     copy(wString* src,const wString& dst);
		//Initialze SAC
	//    static void     wStringInit(void);
	//    static void     wStringEnd(void);
	//    static wString* NextSac(void);
	void   resize(const int newSize);
	//OPERATION OVERLOAD
	wString  operator+(const wString& str) const;
	wString  operator+(const char* str) const;
	friend wString operator+(const char* str1, const wString& str2);
	void     operator=(const wString& str);
	void     operator=(const char* str);
	void     operator=(const int num);
	void     operator=(const double num);
	bool     operator==(const wString& str) const;
	bool     operator==(const char* str) const;
	bool     operator!=(const wString& str) const;
	bool     operator!=(const char* str) const;
	bool     operator>=(const wString& str) const;
	bool     operator>=(const char* str) const;
	bool     operator<=(const wString& str) const;
	bool     operator<=(const char* str) const;
	bool     operator> (const wString& str) const;
	bool     operator> (const char* str) const;
	bool     operator< (const wString& str) const;
	bool     operator< (const char* str) const;
	void     operator+=(const char ch);
	void     operator+=(const wString& str);
	void     operator+=(const char* str);
	char     operator[](int index) const;
	//STRING FUNCTION
	bool            starts_with(const char* str);
	bool            ends_with(const char* str);
	char            at(unsigned int index) const;
	wString        SubString(int start, int mylen)   const;
	wString        substr(int start, int mylen = -1) const;
	//    wString        substr(int index) const;
	int             compare(const wString& str) const;
	int             compare(const char* str) const;
	void            clear(void);
	//    int             Pos(const char* pattern);
	int             Pos(const wString& pattern, int pos = 0) const;
	int             Pos(const char* pattern, int pos = 0) const;
	//    int             Pos(const wString& pattern,int pos);
	size_t          copy(char* str, size_t slen, size_t index) const;
	wString& replace(unsigned int index, unsigned int len, const wString& repstr);

	unsigned int          size(void) const;
	unsigned int    length(void) const;

	unsigned int    Total(void) const;
	char* c_str(void) const;
	wString& SetLength(const unsigned int num);

	wString         Trim(void);
	wString         rtrim(void);
	wString         LTrim(void);
	static void     Rtrimch(char* sentence, const char cut_char);
	int             sprintf(const char* format, ...);
	int             cat_sprintf(const char* format, ...);
	int             last_delimiter(const char* delim) const;
	wString         strsplit(const char* delimstr);
	int             find(const wString& str, int index = 0) const;
	int             find(const char* str, int index = 0) const;
	int             find(char ch, int index = 0) const;
	int             rfind(const wString& str, size_t index = 0) const;
	int             rfind(const char* str, size_t index = 0) const;
	int             rfind(char ch, size_t index = 0) const;
	//URL and encode
	static char* linux_file_name(char* FileName);
	static char* windows_file_name(char* FileName);
	wString         uri_encode(void);
	wString         uri_decode(void);
	wString         htmlspecialchars(void);
	wString         add_slashes(void);
	//ANISSTRING
	bool            insert_list_string(wString& src, int pos);
	bool            insert_list_string(const char* src, int pos);
	wString         get_list_string(const int pos);
	int             getLines(void);
	//    int             Count(void);
	void            ResetLength(unsigned int num);
	void            Add(const wString& str);
	void            Add(const char* str);
	//HEADER
	void            headerInit(size_t content_length, int expire = 1, const char* mime_type = "text/html");
	void            headerPrint(int socket, int endflag);
	wString         headerPrintMem(void);
	int             header(const char* str, int flag = true, int status = 0);
	//FILE OPERATION
	static bool     rename_file(const wString& src, const wString& dst);
	static int      FileCopy(const char* fname_r, const char* fname_w);
	static int      delete_file(const wString& str);
	static int      directory_exists(const char* str);
	static int      directory_exists(const wString& str);
	static wString  file_stats(const char* str, int mode = 0);
	static wString  file_stats(const wString& str, int mode = 0);
	static int      file_exists(const char* str);
	static int      file_exists(const wString& str);
	static wString  extract_file_dir(const wString& str);
	static wString  extract_file_name(const char* str, const char* delim = DELIMITER);
	static wString  extract_file_name(const wString& str, const char* delim = DELIMITER);
	static wString  extract_file_ext(const wString& str);
	static int      create_dir(const wString& str);
	static wString  change_file_ext(const wString& str, const char* ext);
	static unsigned long file_size_by_name(char* str);
	static unsigned long file_size_by_name(wString& str);
	static wString  enum_folder(const wString& Path);
	static wString  enum_folder_json(const wString& Path);
	static bool     check_url(const wString& url);
	int             load_from_file(const char* FileName);
	void            load_from_file(const wString& str);
	void            load_from_csv(const char* FileName);
	void            load_from_csv(const wString& str);
	int             save_to_file(const char* FileName);
	int             save_to_file(const wString& str);
	wString         nkfcnv(const wString& option);
	//HTTP接続用
#ifdef web
	static int      http_size(const wString& url);
	static SOCKET   sock_connect(const wString& host, const int port);
	static SOCKET   sock_connect(const char* host, const int port);
	static wString  http_get(const wString& url, const off_t offset);
	static wString  http_get(const char* url, const off_t offset);
	static char* get_local_address(void);

	static void convert_language_code(const unsigned char* in, size_t len, int in_flag, int out_flag);
#endif
};
#endif
