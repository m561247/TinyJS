#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#pragma comment(lib, "shlwapi.lib")
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <dirent.h>
#include <assert.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>
#ifdef linux
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <dirent.h>
#else
#include <windows.h>
#include <process.h>
//#include <dir.h>
#include <Shlwapi.h>
#include <direct.h>
#include <io.h>
#endif
#include <time.h>
#include "wizd_String.h"
#include "define.h"

//linux/windows���p�I�[�v��
//�ǉ�: O_CREAT | O_APPEND | O_WRONLY(�܂���O_RDWR) | (O_BINARY) , S_IREAD | S_IWRITE
//�V�K: O_CREAT | O_TRUNC  | O_WRONLY(�܂���O_RDWR) | (O_BINARY) , S_IREAD | S_IWRITE
//�Ǎ�: O_RDONLY                                     | (O_BINARY) 
int myopen(const char* filename, int amode, int option)
{
#ifdef linux
	if (option != 0) {
		return open(filename, amode, option);
	}
	else {
		return open(filename, amode);
	}
#else
	char work[1024];
	strcpy(work, filename);
	int ptr = 0;
	while (work[ptr]) {
		if (work[ptr] == '/') {
			work[ptr] = '\\';
		}
		ptr++;
	}
	if (option != 0) {
		return _open(work, amode, option);
	}
	else {
		return _open(work, amode);
	}
#endif
}
// **************************************************************************
// fd����A�P�s(CRLF���ALF�P�Ƃ������܂�)��M
// CRLF�͍폜����B
// ��M�����T�C�Y��return����B
// **************************************************************************
int readLine(int fd, char* line_buf_p, int line_max)
{
	char byte_buf;
	int  line_len = 0;
	int	 recv_len;
	// �P�s��M���s
	while (1) {
		recv_len = read(fd, &byte_buf, 1);
		if (recv_len != 1) { // ��M���s�`�F�b�N
			return (-1);
		}
		// CR/LF�`�F�b�N
		if (byte_buf == '\r') {
			continue;
		}
		else if (byte_buf == '\n') {
			*line_buf_p = 0;
			break;
		}
		// �o�b�t�@�ɃZ�b�g
		*line_buf_p++ = byte_buf;
		// ��M�o�b�t�@�T�C�Y�`�F�b�N
		if (++line_len >= line_max) {
			// �o�b�t�@�I�[�o�[�t���[���m
			return (-1);
		}
	}
	return line_len;
}
#ifndef strrstr
//*********************************************************
// strrstr()
//   ������ ���� ������ ����������B
//   �Ō� �Ɍ��ꂽ������̐擪���w�� �|�C���^ ��Ԃ��B
//   �����񂪌�����Ȃ��ꍇ�� null ��Ԃ��B
//   pattern �� �󕶎��� �̏ꍇ�͏�� ������I�[ ��Ԃ��B
//   ������I�[���� '\0' �͌����ΏۂƂȂ�Ȃ��B
//   ���p�p�� �� �啶���Ə����� ����ʂ���B
//
// const char *string
//   �����ΏۂƂȂ镶����
//
// const char *pattern
//   �����񂩂猟�����镶����
//
//*********************************************************
char* // ������ւ̃|�C���^
strrstr(const char* string, const char* pattern)
{
	// ������I�[�ɒB����܂Ō������J��Ԃ��B
	const char* last = NULL;
	for (const char* p = string; ; p++) {
		if (0 == *p) {
			return (char*)last;
		}
		p = strstr(p, pattern);
		if (p == NULL) {
			break;
		}
		last = p;
	}
	return (char*)last;
}//strrstr
#endif
//---------------------------------------------------------------------------
//source
//---------------------------------------------------------------------------
const int wString::npos = (int)(-1);
//---------------------------------------------------------------------------
//�ʏ�R���X�g���N�^
wString::wString(void)
{
	//������
	len = 0;
	total = 1;
	String = (char*)new char[1];
	*String = 0;
}
//---------------------------------------------------------------------------
//�ʏ�R���X�g���N�^
wString::wString(int mylen)
{
	//������
	if (mylen < 0) mylen = 0;
	len = 0;
	//�����O�̕�
	total = mylen + 1;
	String = (char*)new char[mylen + 1];
	*String = 0;
}
//---------------------------------------------------------------------------
//������R���X�g���N�^
wString::wString(const char* str)
{
	//������
	len = strlen(str);
	if (len) {
		total = len + 1;
		String = (char*)new char[total];
#ifdef __linux
		strcpy(String, str);
#else
		strcpy_s(String, total, str);
#endif
	}
	else {
		total = 1;
		String = (char*)new char[1];
		String[0] = 0;
	}
}
//---------------------------------------------------------------------------
//�R�s�[�R���X�g���N�^
wString::wString(const wString& str)
{
	//������
	len = str.len;
	if (str.len) {
		total = str.total;
		String = new char[str.total];
		memcpy(String, str.String, str.total);
		//*String = 0;
	}
	else {
		total = 1;
		String = (char*)new char[1];
		String[0] = 0;
	}
}
//---------------------------------------------------------------------------
//�f�X�g���N�^
wString::~wString()
{
	delete[] String;
}
//---------------------------------------------------------------------------
//�f�B�[�v�R�s�[���\�b�h
//void wString::copy(wString* dst,const wString* src)
//{
//    src->myrealloc(src->total);
//    memcpy( dst->String, src->String, src->total);
//    dst->len = src->len;
//}
//---------------------------------------------------------------------------
//�f�B�[�v�R�s�[���\�b�h
//void wString::copy(wString* dst,const wString& src)
//{
//    dst->myrealloc(src.total);
//    memcpy( dst->String, src.String, src.total);
//    dst->len = src.len;
//}
//---------------------------------------------------------------------------
wString wString::operator+(const wString& str) const
{
	wString temp(*this);
	temp += str;
	return temp;
}
//---------------------------------------------------------------------------
wString wString::operator+(const char* str) const
{
	wString temp(*this);
	temp += str;
	return temp;
}
//---------------------------------------------------------------------------
wString operator+(const char* str1, const wString str2)
{
	wString temp(str1);
	temp += str2;
	return temp;
}
//---------------------------------------------------------------------------
void wString::operator+=(const wString& str)
{
	unsigned int newLen = len + str.len;
	myrealloc(newLen + 1);
	memcpy(String + len, str.String, str.len);
	String[newLen] = 0;
	len = newLen;
	return;
}
//---------------------------------------------------------------------------
void wString::operator+=(const char* str)
{
	unsigned int slen = strlen(str);
	unsigned int newLen = slen + len;
	myrealloc(newLen);
	strcpy(String + len, str);
	len = newLen;
	return;
}
//---------------------------------------------------------------------------
void wString::operator+=(const char ch)
{
	int tmpl = ((len >> 4) + 1) << 4;
	myrealloc(tmpl);
	String[len++] = ch;
	String[len] = 0;
	return;
}
//---------------------------------------------------------------------------
bool wString::operator==(const wString& str) const
{
	if (len != str.len) {
		return false;
	}
	return(strncmp(String, str.String, len) == 0);
}
//---------------------------------------------------------------------------
bool wString::operator==(const char* str) const
{
	unsigned int mylen = strlen(str);
	if (len != mylen) {
		return false;
	}
	return(strncmp(String, str, len) == 0);
}
//---------------------------------------------------------------------------
bool wString::operator!=(const wString& str) const
{
	if (len != str.len) {
		return true;
	}
	return(strncmp(String, str.String, len) != 0);
}
//---------------------------------------------------------------------------
bool wString::operator!=(const char* str) const
{
	if (len != strlen(str)) {
		return true;
	}
	return(strncmp(String, str, len) != 0);
}
//---------------------------------------------------------------------------
bool wString::operator>=(const wString& str) const
{
	unsigned int maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) >= 0);
}
//---------------------------------------------------------------------------
bool wString::operator>=(const char* str) const
{
	unsigned int mylen = strlen(str);
	unsigned int maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) >= 0);
}
//---------------------------------------------------------------------------
bool wString::operator<=(const wString& str) const
{
	unsigned int maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) <= 0);
}
//---------------------------------------------------------------------------
bool wString::operator<=(const char* str) const
{
	unsigned int mylen = strlen(str);
	unsigned int maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) <= 0);
}
//---------------------------------------------------------------------------
bool wString::operator>(const wString& str) const
{
	unsigned int maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) > 0);
}
//---------------------------------------------------------------------------
bool wString::operator>(const char* str) const
{
	unsigned int mylen = strlen(str);
	unsigned int maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) > 0);
}
//---------------------------------------------------------------------------
bool wString::operator<(const wString& str) const
{
	unsigned int maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) < 0);
}
//---------------------------------------------------------------------------
bool wString::operator<(const char* str) const
{
	unsigned int mylen = strlen(str);
	unsigned int maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) < 0);
}
//---------------------------------------------------------------------------
void wString::operator=(const wString& str)
{
	myrealloc(str.total);
	memcpy(String, str.String, str.total);
	len = str.len;
	return;
}
//---------------------------------------------------------------------------
void wString::operator=(const char* str)
{
	int newLen = strlen(str);
	myrealloc(newLen);
	strcpy(String, str);
	len = newLen;
	return;
}
//---------------------------------------------------------------------------
void wString::operator=(const int num)
{
	this->sprintf("%d", num);
	return;
}
//---------------------------------------------------------------------------
void wString::operator=(const double num)
{
	this->sprintf("%f", num);
	return;
}
//---------------------------------------------------------------------------
char wString::operator[](unsigned int index) const
{
	if (index < len) {
		return String[index];
	}
	else {
		perror("out bound");
		return -1;
	}
}
//---------------------------------------------------------------------------
char wString::at(unsigned int index) const
{
	if (index < len) {
		return String[index];
	}
	else {
		perror("out bound");
		return -1;
	}
}
//---------------------------------------------------------------------------
wString& wString::SetLength(const unsigned int num)
{
	myrealloc(num);
	return *this;
}
//---------------------------------------------------------------------------
// ��r
//---------------------------------------------------------------------------
int wString::compare(const wString& str) const
{
	return strcmp(String, str.String);
}
//---------------------------------------------------------------------------
// ��r
//---------------------------------------------------------------------------
int wString::compare(const char* str) const
{
	return strcmp(String, str);
}
//---------------------------------------------------------------------------
// �N���A
//---------------------------------------------------------------------------
void  wString::clear(void)
{
	len = 0;
	if (*String) {
		*String = 0;
	}
}
//---------------------------------------------------------------------------
// ����������
//---------------------------------------------------------------------------
wString  wString::SubString(int start, int mylen) const
{
	if (start + mylen > (int)len) mylen = (int)len - start;
	wString temp(mylen);
	if (mylen > 0) {
		memcpy(temp.String, String + start, mylen);
		temp.String[mylen] = 0;
		//�����s��B�����Ȃ���
		temp.len = mylen;
	}
	return temp;
}
//---------------------------------------------------------------------------
// substr
//---------------------------------------------------------------------------
wString wString::substr(int start, int mylen) const
{
	if (mylen < 0) mylen = len;
	if (start + mylen > (int)len) mylen = (int)len - start;
	wString temp(mylen);
	if (mylen > 0) {
		memcpy(temp.String, String + start, mylen);
		temp.String[mylen] = 0;
		//�����s��B�����Ȃ���
		temp.len = mylen;
	}
	return temp;
}

//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
int wString::find(const wString& str, size_t index) const
{
	char* ptr = strstr(String + index, str.String);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (size_t)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
int wString::find(const char* str, size_t index) const
{
	char* ptr = strstr(String + index, str);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (size_t)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
int wString::find(char ch, size_t index) const
{
	char* ptr = strchr(String + index, ch);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (size_t)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �s������̌���
//---------------------------------------------------------------------------
int wString::rfind(const wString& str, size_t index) const
{
	char* ptr = strrstr(String + index, str.String);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (size_t)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �s������̌���
// str:������
// index:�J�n�ʒu(�ȗ��\)
//---------------------------------------------------------------------------
int wString::rfind(const char* str, size_t index) const
{
	char* ptr = strrstr(String + index, str);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (size_t)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �s������̌���
// ch:char
// index:�J�n�ʒu(�ȗ��\)
//---------------------------------------------------------------------------
int wString::rfind(char ch, size_t index) const
{
	char* ptr = strrchr(String + index, ch);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (size_t)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
//int wString::Pos(const char* pattern)
//{
//    char* ptr = strstr(String,pattern);
//    if( ptr == NULL ){
//        return npos;
//    }else{
//        return (int)(ptr-String);
//    }
//}
//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
int wString::Pos(const char* pattern, int pos) const
{
	char* ptr = strstr(String + pos, pattern);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
//int wString::Pos(wString& pattern, int pos)
//{
//    return 
//    char* ptr = strstr(String+pos,pattern.c_str());
//    if( ptr == NULL ){
//        return npos;
//    }else{
//        return (int)(ptr-String);
//    }
//}
//---------------------------------------------------------------------------
// �ʒu
//---------------------------------------------------------------------------
int wString::Pos(const wString& pattern, int pos) const
{
	return Pos(pattern.String, pos);
}
//---------------------------------------------------------------------------
//�@Size
//---------------------------------------------------------------------------
size_t  wString::size(void) const
{
	return (size_t)len;
}
//---------------------------------------------------------------------------
//�@Size
//---------------------------------------------------------------------------
size_t  wString::length(void) const
{
	return (size_t)len;
}
//---------------------------------------------------------------------------
// �t�@�C���ǂݍ���
//---------------------------------------------------------------------------
void wString::LoadFromFile(const wString& str)
{
	LoadFromFile(str.String);
}
//---------------------------------------------------------------------------
// �t�@�C���ǂݍ���
//---------------------------------------------------------------------------
int wString::LoadFromFile(const char* FileName)
{
	long flen;
	int  handle;
	if (strncmp(FileName, "http://", 7) == 0) {
		wString tmp;
		//tmp = wString::HTTPGet((char*)FileName,0);
		*this = tmp;
	}
	else {
#ifdef linux
		handle = open(FileName, O_RDONLY | S_IREAD);
#else
		handle = myopen(FileName, O_RDONLY | O_BINARY, S_IREAD);
#endif
		if (handle < 0) {
			return -1;
		}
		flen = lseek(handle, 0, SEEK_END);
		lseek(handle, 0, SEEK_SET);
		SetLength(flen + 1);
		len = read(handle, String, flen);
		close(handle);
		String[len] = 0;
		//\0������ꍇ���l�����strlen�Ƃ��Ă͂����Ȃ�
		//len = strlen(String);
	}
	return 0;
}
//---------------------------------------------------------------------------
// �t�@�C���ǂݍ���
//---------------------------------------------------------------------------
void wString::LoadFromCSV(const wString& str)
{
	LoadFromCSV(str.String);
}
int isNumber(char* str)
{
	for (int i = strlen(str) - 1; i >= 0; i--) {
		if ((!isdigit(str[i])) && str[i] != '.') {
			return 0;
		}
	}
	return 1;
}

//---------------------------------------------------------------------------
// �t�@�C���ǂݍ���
//---------------------------------------------------------------------------
void wString::LoadFromCSV(const char* FileName)
{
	int  fd;
	char s[1024];
	char t[1024];
	int ret;
	int first = 1;
	fd = myopen(FileName, O_RDONLY | O_BINARY, S_IREAD);
	if (fd < 0) {
		printf("%s�t�@�C�����J���܂���\n", FileName);
		return;
	}

	*this = "[";
	//�P�s�ڂ̓^�C�g��
	while (true) {
		ret = readLine(fd, s, sizeof(s));
		if (ret < 0) break;
		//��������
		char* p = strtok(s, ",");
		int ptr = 0;
		if (p) {
			if (isNumber(p)) {
				ptr += ::sprintf(t + ptr, "%s", p);
			}
			else {
				ptr += ::sprintf(t + ptr, "\"%s\"", p);
			}
		}
		while ((p = strtok(NULL, ",")) != 0) {
			if (isNumber(p)) {
				ptr += ::sprintf(t + ptr, ",%s", p);
			}
			else {
				ptr += ::sprintf(t + ptr, ",\"%s\"", p);
			}
		}
		if (first) {
			first = 0;
		}
		else {
			*this += ",";
		}
		*this += wString("[") + t + "]";
	}
	*this += "]";
	close(fd);
	return;
}
//---------------------------------------------------------------------------
// �t�@�C����������
//---------------------------------------------------------------------------
int wString::SaveToFile(const wString& str)
{
	return SaveToFile(str.String);
}
//---------------------------------------------------------------------------
// �t�@�C����������
//---------------------------------------------------------------------------
int wString::SaveToFile(const char* FileName)
{
#ifdef linux
	int handle = myopen(FileName, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#else
	int handle = myopen(FileName, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IREAD | S_IWRITE);
#endif
	if (handle < 0) {
		return handle;
	}
	write(handle, String, len);
	close(handle);
	return 0;
}
//---------------------------------------------------------------------------
// �g����
//---------------------------------------------------------------------------
wString wString::Trim(void)
{
	wString temp(*this);
	if (temp.len) {
		//�擪�̋󔒓��𔲂�
		while (temp.len && *temp.String <= ' ') {
#ifdef linux
			char* src = temp.String;
			char* dst = src + 1;
			while (*src) {
				*src++ = *dst++;
			}
#else
			strcpy((char*)temp.String, (char*)(temp.String + 1));
#endif
			temp.len--;
		}
		//�����̋󔒓��𔲂�
		while (temp.len && temp.String[temp.len - 1] <= ' ') {
			temp.String[--temp.len] = 0;
		}
	}
	return temp;
}
//---------------------------------------------------------------------------
// �g����
//---------------------------------------------------------------------------
wString wString::RTrim(void)
{
	wString temp(*this);
	if (temp.len) {
		//�����̋󔒓��𔲂�
		while (temp.len && temp.String[temp.len - 1] <= ' ') {
			temp.String[--temp.len] = 0;
		}
	}
	return temp;
}
//---------------------------------------------------------------------------
// sentence������̍s���ɁAcut_char���������Ƃ��A�폜
//---------------------------------------------------------------------------
void wString::Rtrimch(char* sentence, char cut_char)
{
	if (sentence == NULL || *sentence == 0) return;
	char* source_p;
	int         length, i;
	length = strlen(sentence);        // ������Get
	source_p = sentence;
	source_p += length;                 // ���[�N�|�C���^�𕶎���̍Ō�ɃZ�b�g�B
	for (i = 0; i < length; i++) {       // ������̐������J��Ԃ��B
		source_p--;                     // �ꕶ�����O�ցB
		if (*source_p == cut_char) {     // �폜�L���� �q�b�g�����ꍇ�폜
			*source_p = '\0';
		}
		else {                          // �Ⴄ�L�������o�Ă����Ƃ���ŏI���B
			break;
		}
	}
	return;
}
//---------------------------------------------------------------------------
// �g����
//---------------------------------------------------------------------------
wString wString::LTrim(void)
{
	wString temp(*this);
	if (temp.len) {
		//�擪�̋󔒓��𔲂�
		while (temp.len && *temp.String <= ' ') {
			char* src = temp.String;
			char* dst = src + 1;
			while (*src) {
				*src++ = *dst++;
			}
			temp.len--;
		}
	}
	return temp;
}
#if 1
//--------------------------------------------------------------------
wString wString::FileStats(const char* str, int mode)
{
	struct stat      stat_buf;
	wString buf;
	if (stat(str, &stat_buf) == 0 && mode == 0) {
		/* �t�@�C������\�� */
		buf.sprintf("{\"permission\":\"%o\",\"size\":%d,\"date\":\"%s\"}", stat_buf.st_mode, stat_buf.st_size, ctime(&stat_buf.st_mtime));
		//printf("�f�o�C�XID : %d\n",stat_buf.st_dev);
		//printf("inode�ԍ� : %d\n",stat_buf.st_ino);
		//printf("�A�N�Z�X�ی� : %o\n",stat_buf.st_mode );
		//printf("�n�[�h�����N�̐� : %d\n",stat_buf.st_nlink);
		//printf("���L�҂̃��[�UID : %d\n",stat_buf.st_uid);
		//printf("���L�҂̃O���[�vID : %d\n",stat_buf.st_gid);
		//printf("�f�o�C�XID�i����t�@�C���̏ꍇ�j : %d\n",stat_buf.st_rdev);
		//printf("�e�ʁi�o�C�g�P�ʁj : %d\n",stat_buf.st_size);
		//printf("�t�@�C���V�X�e���̃u���b�N�T�C�Y : %d\n",stat_buf.st_blksize);
		//printf("���蓖�Ă�ꂽ�u���b�N�� : %d\n",stat_buf.st_blocks);
		//printf("�ŏI�A�N�Z�X���� : %s",ctime(&stat_buf.st_atime));
		//printf("�ŏI�C������ : %s",ctime(&stat_buf.st_mtime));
		//printf("�ŏI��ԕύX���� : %s",ctime(&stat_buf.st_ctime));
	}
	else {
		//date
		if (mode == 1) {
			//char s[128] = {0};
			//time_t timer;
			//struct tm *timeptr;
			//timer = time(NULL);
			//timeptr = localtime(&stat_buf.st_mtime);
			//strftime(s, 128, "%Y/%m/%d %H:%M:%S", timeptr);
			buf.sprintf("%d", stat_buf.st_mtime);
		}
	}
	return buf;
}
//---------------------------------------------------------------------------
wString wString::FileStats(const wString& str, int mode)
{
	return FileStats(str.String, mode);
}
#endif
//---------------------------------------------------------------------------
int wString::FileExists(const char* str)
{
	int  flag = 0;
#ifdef linux
	struct stat send_filestat;
	int  result = stat(str, &send_filestat);
	if ((result == 0) && (S_ISREG(send_filestat.st_mode) == 1)) {
		flag = 1;
	}
#else
	WIN32_FIND_DATAA fd = { 0 };
	HANDLE hFound = FindFirstFileA(str, &fd);
	flag = (hFound != INVALID_HANDLE_VALUE) ? 1 : 0;
	FindClose(hFound);

	//return retval;



	//flag = PathFileExists((char*)str);
#endif
	return flag;
}
//---------------------------------------------------------------------------
int wString::FileExists(const wString& str)
{
	return FileExists(str.String);
}
//---------------------------------------------------------------------------
//�p�X�����𒊏o
wString wString::ExtractFileDir(wString& str)
{
	//todo SJIS/EUC�Ή�����悤��
	wString temp(str);
	int ptr = temp.LastDelimiter(DELIMITER);
	temp.len = ptr;
	temp.String[ptr] = 0;
	return temp;
}
//---------------------------------------------------------------------------
int wString::CreateDir(const wString& str)
{
	int flag = 0;
#ifdef linux
	//0x777�ł͂����ƃt�H���_�ł��Ȃ�
	flag = (mkdir(str.String, 0777) != -1);
#else
	char work[2048];
	strcpy(work, str.c_str());
	WindowsFileName(work);
	if (!DirectoryExists(work)) {
		flag = (_mkdir(work) != -1);
	}
#endif
	return flag;
}
//---------------------------------------------------------------------------
//�@TStringList�΍�
//  �s����Ԃ�
//---------------------------------------------------------------------------
//void wString::ResetLength(unsigned int num)
//{
//    assert(total>(unsigned int)num);
//    String[num] = 0;
//    len = num;
//}
//---------------------------------------------------------------------------
char* wString::c_str(void) const
{
	return String;
}
//---------------------------------------------------------------------------
int wString::Length(void) const
{
	return len;
}
//---------------------------------------------------------------------------
int wString::Total(void) const
{
	return total;
}
//---------------------------------------------------------------------------
int wString::LastDelimiter(const char* delim) const
{
	int pos = -1;
	int dlen = strlen(delim);
	for (int i = len - dlen; i > 0; i--) {
		if (strncmp(String + i, delim, dlen) == 0) {
			pos = i;
			break;
		}
	}
	return pos;
}
//---------------------------------------------------------------------------
bool wString::RenameFile(const wString& src, const wString& dst)
{
	if (rename(src.c_str(), dst.c_str()) >= 0) {
		return true;
	}
	else {
		return false;
	}
}
//---------------------------------------------------------------------------
unsigned long wString::FileSizeByName(char* str)
{
	unsigned long pos;
	int handle;
#ifdef linux
	handle = open(str, 0);
#else
	handle = open(str, O_BINARY);
#endif
	pos = lseek(handle, 0, SEEK_END);
	close(handle);
	return pos;
}
//---------------------------------------------------------------------------
unsigned long wString::FileSizeByName(wString& str)
{
	return FileSizeByName(str.String);
}
//---------------------------------------------------------------------------
wString wString::ExtractFileName(const char* str, const char* delim)
{
	wString tmp(str);
	return ExtractFileName(tmp, delim);
}
//---------------------------------------------------------------------------
wString wString::ExtractFileName(const wString& str, const char* delim)
{
	int pos = str.LastDelimiter(delim);
	return str.substr(pos + 1);
}
//---------------------------------------------------------------------------
wString wString::ExtractFileExt(const wString& str)
{
	int pos = str.LastDelimiter(".");
	return str.substr(pos + 1);
}

//---------------------------------------------------------------------------
wString wString::ChangeFileExt(wString& str, const char* ext)
{
	int pos = str.LastDelimiter(".");
	return str.substr(0, pos + 1) + ext;
}
//---------------------------------------------------------------------------
int wString::DeleteFile(const wString& str)
{
	int flag = 0;
#ifdef linux
	flag = (unlink(str.String) == 0);
#else
	char work[2048];
	strcpy(work, str.c_str());
	WindowsFileName(work);
	if (FileExists(work)) {
		if (unlink(work) == 0) {
			flag = 1;
		}
	}
#endif
	return flag;
}
//---------------------------------------------------------------------------
int wString::DirectoryExists(const char* str)
{
	int flag = 0;
#ifdef linux
	struct stat send_filestat;
	int  result = stat(str, &send_filestat);
	if ((result == 0) && (S_ISDIR(send_filestat.st_mode) == 1)) {
		flag = 1;
	}
#else
	flag = PathIsDirectory((LPCTSTR)str);
#endif
	return flag;
}
//---------------------------------------------------------------------------
int wString::DirectoryExists(const wString& str)
{
	return DirectoryExists(str.String);
}
/********************************************************************************/
// sentence���������key�������rep������Œu������B
// sentence:��������
// slen:��������̒���
// p:�u���O�̈ʒu
// klen:�u���O�̒���
// rep:�u���㕶����
/********************************************************************************/
void wString::replace_character_len(const char* sentence, int slen, const char* p, int klen, const char* rep)
{
	char* str;
	int rlen = strlen((char*)rep);
	int num;
	if (klen == rlen) {
		memcpy((void*)p, rep, rlen);
		//�O�l�ߒu�����̂܂܃R�s�[����΂���
	}
	else if (klen > rlen) {
		num = klen - rlen;
		strcpy((char*)p, (char*)(p + num));
		memcpy((void*)p, rep, rlen);
		//�u�������������̂Ō�l�߂���
	}
	else {
		num = rlen - klen;
		//p����rlen-klen�����̂΂�
		for (str = (char*)(sentence + slen + num); str > p + num; str--) {
			*str = *(str - num);
		}
		memcpy((void*)p, rep, rlen);
	}
	return;
}
//---------------------------------------------------------------------------
// �t�H���_�̃t�@�C���𐔂���
// �����@wString Path:
// �ߒl�@�t�@�C�����X�g�B�ق��Ă����Ă������A�R�s�[���Ăق���
wString wString::EnumFolderjson(const wString& Path)
{
	//DIR                  *dir;
	//struct dirent        *ent;
	wString              temp;
	//wString              Path2;
	//std::vector<wString> list;
	//Path2 = Path;
	////Directory�I�[�v��
	//if ((dir = opendir(Path.String)) != NULL){
	//    //�t�@�C�����X�g
	//    while ((ent = readdir(dir)) != NULL){
	//        if( strcmp(ent->d_name,"." ) != 0 &&
	//        strcmp(ent->d_name,"..") != 0 ){
	//            list.push_back("\""+Path2+DELIMITER+ent->d_name+"\"");
	//        }
	//    }
	//    closedir(dir);
	//    
	//    //sort
	//    if( list.size()>0){
	//        for( unsigned int ii = 0 ; ii < list.size()-1; ii++){
	//            for( unsigned int jj = ii+1 ; jj < list.size(); jj++){
	//                if( strcmp(list[ii].c_str(),list[jj].c_str())> 0 ){
	//                    std::swap(list[ii],list[jj]);
	//                }
	//            }
	//        }
	//    }        temp = "[";
	//    for( unsigned int i = 0 ; i < list.size() ; i++ ){
	//        if( i ) temp += ",";
	//        temp += list[i];
	//    }
	//    temp += "]";
	//}else{
	//    perror("�f�B���N�g���̃I�[�v���G���[");
	//    exit(1);
	//}
	return temp;
}
//---------------------------------------------------------------------------
// �t�H���_�̃t�@�C���𐔂���
// �����@wString Path:
// �ߒl�@�t�@�C�����X�g�B�ق��Ă����Ă������A�R�s�[���Ăق���
wString wString::EnumFolder(const wString& Path)
{
	wString temp;
#ifdef linux
	struct dirent** namelist;
	int n;
	wString Path2;
	Path2 = Path;
	int first = 1;

	n = scandir(Path.String, &namelist, NULL, alphasort);
	if (n < 0) {
		temp = "";
	}
	else
	{
		temp = "[";
		for (int i = 0; i < n; i++) {
			if (first) {
				first = 0;
			}
			else {
				temp += ",";
			}
			temp += "\"" + Path2 + DELIMITER + namelist[i]->d_name + "\"";
			free(namelist[i]);
		}
		temp += "]";
		free(namelist);
	}
#else
	//DIR                  *dir;
	//struct dirent        *ent;
	//wString              temp;
	//wString              Path2;
	//std::vector<wString> list;
	//Path2 = Path;
	////Directory�I�[�v��
	//if ((dir = opendir(Path.String)) != NULL){
	//    //�t�@�C�����X�g
	//    while ((ent = readdir(dir)) != NULL){
	//        if( strcmp(ent->d_name,"." ) != 0 &&
	//        strcmp(ent->d_name,"..") != 0 ){
	//            list.push_back( Path2+DELIMITER+ent->d_name );
	//        }
	//    }
	//    closedir(dir);
	//    if( list.size()>0){
	//        for( unsigned int ii = 0 ; ii < list.size()-1; ii++){
	//            for( unsigned int jj = ii+1 ; jj < list.size(); jj++){
	//                if( strcmp(list[ii].c_str(),list[jj].c_str())> 0 ){
	//                    std::swap(list[ii],list[jj]);
	//                }
	//            }
	//        }
	//    }
	//    for( unsigned int i = 0 ; i < list.size() ; i++ ){
	//        temp.Add(list[i]);
	//    }
	//}else{
	//    perror("�f�B���N�g���̃I�[�v���G���[");
	//    exit(1);
	//}
#endif
	return temp;
}
#ifndef va_copy
int wString::vtsprintf(const char* fmt, va_list arg) {
	int len = 0;
	int size = 0;
	int zeroflag, width;


	while (*fmt) {
		if (*fmt == '%') {        /* % �Ɋւ��鏈�� */
			zeroflag = width = 0;
			fmt++;
			if (*fmt == '0') {
				fmt++;
				zeroflag = 1;
			}
			while ((*fmt >= '0') && (*fmt <= '9')) {
				width *= 10;
				width += *(fmt++) - '0';
			}

			/* printf ("zerof = %d,width = %d\n",zeroflag,width); */

			//llu��lu��u������
			while (*fmt == 'l' || *fmt == 'z') {
				*fmt++;
			}

			switch (*fmt) {
			case 'd':        /* 10�i�� */
				size = tsprintf_decimal(va_arg(arg, signed long), zeroflag, width);
				break;
			case 'u':        /* 10�i�� */
				size = tsprintf_decimalu(va_arg(arg, unsigned long), zeroflag, width);
				break;
			case 'o':        /* 8�i�� */
				size = tsprintf_octadecimal(va_arg(arg, unsigned long), zeroflag, width);
				break;
			case 'x':        /* 16�i�� 0-f */
				size = tsprintf_hexadecimal(va_arg(arg, unsigned long), 0, zeroflag, width);
				break;
			case 'X':        /* 16�i�� 0-F */
				size = tsprintf_hexadecimal(va_arg(arg, unsigned long), 1, zeroflag, width);
				break;
			case 'c':        /* �L�����N�^�[ */
				size = tsprintf_char(va_arg(arg, int));
				break;
			case 's':        /* ASCIIZ������ */
				size = tsprintf_string(va_arg(arg, char*));
				break;
			default:        /* �R���g���[���R�[�h�ȊO�̕��� */
			/* %%(%�ɑΉ�)�͂����őΉ������ */
				len++;
				*this += *fmt;
				break;
			}
			len += size;
			fmt++;
		}
		else {
			*this += *(fmt++);
			len++;
		}
	}
	va_end(arg);
	return (len);
}


/*
���l => 10�i������ϊ�
*/
int wString::tsprintf_decimal(signed long val, int zerof, int width)
{
	//�����O��ۏ�
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	int minus = 0;

	if (!val) {        /* �w��l��0�̏ꍇ */
		*(ptmp--) = '0';
		len++;
	}
	else {
		/* �}�C�i�X�̒l�̏ꍇ�ɂ�2�̕␔����� */
		if (val < 0) {
			val = ~val;
			val++;
			minus = 1;
		}
		while (val && len < 19) {
			/* �o�b�t�@�A���_�[�t���[�΍� */
				//if (len >= 19){
				//    break;
				//}

			*ptmp = (char)((val % 10) + '0');
			val /= 10;
			ptmp--;
			len++;
		}

	}

	/* �����A�����킹�Ɋւ��鏈�� */
	if (zerof) {
		if (minus) {
			width--;
		}
		while (len < width) {
			*(ptmp--) = '0';
			len++;
		}
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
	}
	else {
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
		while (len < width) {
			*(ptmp--) = ' ';
			len++;
		}
	}
	*this += (ptmp + 1);
	return (len);
}
/*
���l => 10�i������ϊ�
*/
int wString::tsprintf_decimalu(unsigned long val, int zerof, int width)
{
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	int minus = 0;

	if (!val) {        /* �w��l��0�̏ꍇ */
		*(ptmp--) = '0';
		len++;
	}
	else {
		while (val) {
			/* �o�b�t�@�A���_�[�t���[�΍� */
			if (len >= 19) {
				break;
			}

			*ptmp = (char)((val % 10) + '0');
			val /= 10;
			ptmp--;
			len++;
		}
	}

	/* �����A�����킹�Ɋւ��鏈�� */
	if (zerof) {
		if (minus) {
			width--;
		}
		while (len < width) {
			*(ptmp--) = '0';
			len++;
		}
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
	}
	else {
		while (len < width) {
			*(ptmp--) = ' ';
			len++;
		}
	}

	*this += (ptmp + 1);
	return (len);
}
/*
���l => 8�i������ϊ�
*/
int wString::tsprintf_octadecimal(unsigned long val, int zerof, int width)
{
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	int minus = 0;

	if (!val) {        /* �w��l��0�̏ꍇ */
		*(ptmp--) = '0';
		len++;
	}
	else {
		while (val) {
			/* �o�b�t�@�A���_�[�t���[�΍� */
			if (len >= 19) {
				break;
			}

			*ptmp = (char)((val % 8) + '0');
			val /= 8;
			ptmp--;
			len++;
		}
	}

	/* �����A�����킹�Ɋւ��鏈�� */
	if (zerof) {
		if (minus) {
			width--;
		}
		while (len < width) {
			*(ptmp--) = '0';
			len++;
		}
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
	}
	else {
		while (len < width) {
			*(ptmp--) = ' ';
			len++;
		}
	}

	*this += (ptmp + 1);
	return (len);
}
/*
���l => 16�i������ϊ�
*/
int wString::tsprintf_hexadecimal(unsigned long val, int capital, int zerof, int width)
{
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	char str_a;

	/* A�`F��啶���ɂ��邩�������ɂ��邩�؂�ւ��� */
	if (capital) {
		str_a = 'A';
	}
	else {
		str_a = 'a';
	}

	if (!val) {        /* �w��l��0�̏ꍇ */
		*(ptmp--) = '0';
		len++;
	}
	else {
		while (val) {
			/* �o�b�t�@�A���_�[�t���[�΍� */
			if (len >= 18) {
				break;
			}

			*ptmp = (char)(val % 16);
			if (*ptmp > 9) {
				*ptmp += (char)(str_a - 10);
			}
			else {
				*ptmp += '0';
			}

			val >>= 4;        /* 16�Ŋ��� */
			ptmp--;
			len++;
		}
	}
	while (len < width) {
		*(ptmp--) = zerof ? '0' : ' ';
		len++;
	}

	*this += (ptmp + 1);
	return(len);
}

/*
���l => 1�����L�����N�^�ϊ�
*/
int wString::tsprintf_char(int ch)
{
	*this += (char)ch;
	return(1);
}

/*
���l => ASCIIZ������ϊ�
*/
int wString::tsprintf_string(char* str)
{
	*this += str;
	return(strlen(str));
}
#endif
//---------------------------------------------------------------------------
//wString �ψ���
int wString::sprintf(const char* format, ...)
{
#ifndef va_copy
	String[0] = 0;
	len = 0;
	va_list ap;
	va_start(ap, format);
	vtsprintf(format, ap);
	va_end(ap);
	return len;
#else
	int stat;
	//�ψ������Q���
	va_list ap1, ap2;
	va_start(ap1, format);
	va_copy(ap2, ap1);
	//�ŏ��̓_�~�[�ŕ����񒷂��V�~�����[�g
	stat = vsnprintf(String, 0, format, ap1);
	SetLength(stat + 1);
	//���ۂɏo��
	stat = vsprintf(String, format, ap2);
	va_end(ap1);
	va_end(ap2);
	len = stat;
	return stat;
#endif
}
//---------------------------------------------------------------------------
//wString �ψ���
int wString::cat_sprintf(const char* format, ...)
{
#ifndef va_copy
	int status;
	va_list ap;
	va_start(ap, format);
	status = this->vtsprintf(format, ap);
	va_end(ap);
	return status;
#else
	int stat;
	//�ψ������Q���
	va_list ap1, ap2;
	va_start(ap1, format);
	va_copy(ap2, ap1);
	//�ŏ��̓_�~�[�ŕ����񒷂��V�~�����[�g
	stat = vsnprintf(String, 0, format, ap1);
	SetLength(stat + len + 1);
	//���ۂɏo��
	stat = vsprintf(String + len, format, ap2);
	va_end(ap1);
	va_end(ap2);
	len += stat;
	return stat;
#endif
}
//---------------------------------------------------------------------------
//��������f���~�^�Ő؂��āA�f���~�^��̕�����Ԃ�
//����
//wString str           :���͕�����
//const char* delimstr  :�ؒf������
//�߂�l
//wString               :�ؒf��̕�����
//������Ȃ��ꍇ�͒����O�̕�����
wString wString::strsplit(const char* delimstr)
{

	wString tmp;
	int delimlen = strlen(delimstr);
	int pos = Pos(delimstr);
	if (pos != npos) {
		tmp = substr(pos + delimlen);
	}
	return tmp;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void wString::myrealloc(const int newsize)
{
	if (len >= total) {
		printf("not good %d %d", len, total);
		exit(1);
	}
	if ((int)total <= newsize) {
		total = newsize + 1;
		char* tmp = new char[total];
		memcpy(tmp, String, len);
		tmp[len] = 0;
		delete[] String;
		String = tmp;
	}
	else {
		//�w��T�C�Y������菬�����̂ŉ������Ȃ�
	}
}
//---------------------------------------------------------------------------
//HTML���ꕶ���ɕϊ�
wString wString::htmlspecialchars(void)
{
	wString temp;
	// �����`�F�b�N
	char* ptr = String;
	for (unsigned int is = 0; is < len; is++) {
		switch (*ptr) {
		case '&':temp += "&amp;"; break;
		case '\"':temp += "&quot;"; break;
			//case '\'':temp += "&#39;";break;
		case '<':temp += "&lt;";  break;
		case '>':temp += "&gt;";  break;
		default:temp += *ptr;    break;
		}
		ptr++;
	}
	return temp;
}
//---------------------------------------------------------------------------
// **************************************************************************
//  URI�G���R�[�h���s���܂�.
//  �@�\ : URI�f�R�[�h���s��
//  ���� : int uri_encode
//  (char* dst,size_t dst_len,const char* src,int src_len);
//  ���� : dst �ϊ����������̏����o����.
//                 dst_len �ϊ����������̏����o����̍ő咷.
//                 src �ϊ����̕���.
//                 src_len �ϊ����̕����̒���.
//  �Ԓl : �G���R�[�h���������̐�(���̂܂܂��܂�)
// **************************************************************************
wString wString::uri_encode(void)
{
	unsigned int is;
	wString dst;
	char work[8];
	// �����`�F�b�N
	for (is = 0; is < len; is++) {
		/* ' '(space) �͂��Ɠ��ʈ����ɂ��Ȃ��Ƃ܂��� */
		if (String[is] == ' ') {
			dst += "%20";
			/* �G���R�[�h���Ȃ������S���W�� */
			//        }else if ( strchr("!$()*,-./:;?@[]^_`{}~", String[is]) != NULL ){
		}
		else if (strchr("!$()*,-.:;/?@[]^_`{}~", String[is]) != NULL) {
			dst += String[is];
			/* �A���t�@�x�b�g�Ɛ����̓G���R�[�h�������̂܂� */
		}
		else if (isalnum(String[is])) {
			dst += String[is];
		}
		/* \�}�[�N�̓G���R�[�h */
		else if (String[is] == '\\') {
			dst += "%5c";
			/* ����ȊO�͂��ׂăG���R�[�h */
		}
		else {
			::sprintf(work, "%%%2X", (unsigned char)String[is]);
			dst += work;
		}
	}
	return dst;
}
int   wString::FileCopy(const char* fname_r, const char* fname_w)
{
	int fpr;
	int fpw;
	int size;
	unsigned char buf[8000];

	fpr = myopen(fname_r, O_RDONLY | O_BINARY, S_IREAD);
	if (fpr < 0) {
		return -1;
	}
	fpw = myopen(fname_w, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
	if (fpw < 0) {
		close(fpr);
		return -1;
	}
	while (1) {
		size = read(fpr, buf, sizeof(buf));
		if (size <= 0) {
			break;
		}
		write(fpw, buf, size);
	}
	close(fpr);
	close(fpw);

	return 0;
}
//---------------------------------------------------------------------------
//QUOTE,ESCAPE,NULL��ی삷��
wString wString::addSlashes(void)
{
	wString tmp;
	for (unsigned int i = 0; i < len; i++) {
		if (String[i] == '\'' || String[i] == '\"' || String[i] == '\\' || String[i] == 0) {
			tmp += '\\';
		}
		tmp += String[i];
	}
	return tmp;
}
// **************************************************************************
// URI�f�R�[�h���s���܂�.
//  �@�\ : URI�f�R�[�h���s��
//  ���� : dst �ϊ����������̏����o����.
//                dst_len �ϊ����������̏����o����̍ő咷.
//                src �ϊ����̕���.
//                src_len �ϊ����̕����̒���.
// �Ԓl : �f�R�[�h���������̐�(���̂܂܂��܂�)
// **************************************************************************
unsigned char htoc(unsigned char x)
{
	if ('0' <= x && x <= '9') return (unsigned char)(x - '0');
	if ('a' <= x && x <= 'f') return (unsigned char)(x - 'a' + 10);
	if ('A' <= x && x <= 'F') return (unsigned char)(x - 'A' + 10);
	return 0;
}
wString wString::uri_decode()
{
	size_t          i;
	unsigned char   code;
	wString dst;
	// =================
	// ���C�����[�v
	// =================
	for (i = 0; i < len && String[i] != '\0'; i++) {
		if (String[i] == '%') {
			if (i + 2 >= len) {
				break;
			}
			code = (unsigned char)(htoc(String[++i]) << 4);
			code += htoc(String[++i]);
			dst += code;
		}
		else if (String[i] == '+') {
			dst += ' ';
		}
		else {
			dst += String[i];
		}
	}
	return dst;
}
#ifdef WEB
void wString::headerInit(size_t content_length, int expire, const char* mime_type)
{
	sprintf("%s", HTTP_OK);
	cat_sprintf("%s", HTTP_CONNECTION);
	cat_sprintf(HTTP_SERVER_NAME, SERVER_NAME);
	cat_sprintf(HTTP_CONTENT_TYPE, mime_type);
	//Date
	time_t timer;
	time(&timer);
	struct tm* utc;
	utc = gmtime(&timer);
	char work[80];
	char you[7][4] = { "Sun", "Mon","Tue", "Wed", "Thu", "Fri", "Sat" };
	char mon[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	::sprintf(work, "%s, %d %s %d %02d:%02d:%02d",
		you[utc->tm_wday], utc->tm_mday, mon[utc->tm_mon], utc->tm_year + 1900, utc->tm_hour, utc->tm_min, utc->tm_sec);
	this->cat_sprintf("Date: %s GMT\r\n", work);
	//expire
	if (expire) {
		timer += 60 * 60;
		utc = gmtime(&timer);
		::sprintf(work, "%s, %d %s %d %02d:%02d:%02d",
			you[utc->tm_wday], utc->tm_mday, mon[utc->tm_mon], utc->tm_year + 1900, utc->tm_hour, utc->tm_min, utc->tm_sec);
		this->cat_sprintf("Expires: %s GMT\r\n", work);
	}
	if (content_length) {
		this->cat_sprintf(HTTP_CONTENT_LENGTH, content_length);
	}
}
void wString::headerPrint(int socket, int endflag)
{
	send(socket, String, len, 0);
	if (endflag) {
		send(socket, HTTP_END, strlen(HTTP_END), 0);
	}
}
wString wString::headerPrintMem(void)
{
	return *this + HTTP_END;
}
int wString::header(const char* str, int flag, int status)
{
	char head[80] = { 0 };
	char body[80] = { 0 };
	if (*str) {
		if (!flag) {
			Add(str);
			return 0;
		}
		//if( strncmp( str, "HTTP/", 5) == 0 ){
		//    memcpy( head, str,5);
		//    head[5]=0;
		//    strcpy( body, str+5);
		//}else{
		for (size_t i = 0; i < strlen(str); i++) {
			if (str[i] == ' ') {
				memcpy(head, str, i + 1);
				head[i + 1] = 0;
				strcpy(body, str + i + 1);
				break;
			}
		}
		//}
		if (*head && *body) {
			if (strcmp(head, "Location: ") == 0) {
				if (status == 301) {
					header("HTTP/1.0 301 Moved Permanetry", true);
				}
				else {
					header("HTTP/1.0 302 Found", true);
				}
			}
			int count = getLines();
			wString str2;
			str2.sprintf("%s%s", head, body);
			//����Γ���ւ�
			for (int i = 0; i < count; i++) {
				wString tmp = GetListString(i);
				if (strncmp(tmp.c_str(), head, strlen(head)) == 0) {
					SetListString(str2.c_str(), i);
					return 0;
				}
			}
			//�Ȃ���Βǉ�
			Add(str2);
			return 0;
		}
	}
	return 1;
}
/********************************************************************************/
// ���{�ꕶ���R�[�h�ϊ��B
// (libnkf�̃��b�p�[�֐�)
//
//      �T�|�[�g����Ă���`���͈ȉ��̒ʂ�B
//              in_flag:        CODE_AUTO, CODE_SJIS, CODE_EUC, CODE_UTF8, CODE_UTF16
//              out_flag:       CODE_SJIS, CODE_EUC
/********************************************************************************/
void wString::convert_language_code(const unsigned char* in, size_t len, int in_flag, int out_flag)
{
	unsigned char       nkf_option[8];
	unsigned char* out;
	out = (unsigned char*)(new char[len * 3]);
	memset(nkf_option, '\0', sizeof(nkf_option));
	//=====================================================================
	// in_flag, out_flag���݂āAlibnkf�ւ̃I�v�V������g�ݗ��Ă�B
	//=====================================================================
	switch (in_flag)
	{
	case CODE_SJIS:
		strncpy((char*)nkf_option, "S", sizeof(nkf_option));
		break;
	case CODE_EUC:
		strncpy((char*)nkf_option, "E", sizeof(nkf_option));
		break;
	case CODE_UTF8:
		strncpy((char*)nkf_option, "W", sizeof(nkf_option));
		break;
	case CODE_UTF16:
		strncpy((char*)nkf_option, "W16", sizeof(nkf_option));
		break;
	case CODE_AUTO:
	default:
		strncpy((char*)nkf_option, "", sizeof(nkf_option));
		break;
	}
	switch (out_flag)
	{
	case CODE_EUC:
		strncat((char*)nkf_option, "e", sizeof(nkf_option) - strlen((char*)nkf_option));
		break;
	case CODE_SJIS:
		strncat((char*)nkf_option, "s", sizeof(nkf_option) - strlen((char*)nkf_option));
		break;
	case CODE_UTF8:
	default:
		strncat((char*)nkf_option, "w", sizeof(nkf_option) - strlen((char*)nkf_option));
		break;
	}
	//=================================================
	// libnkf ���s
	//=================================================
	nkf((const char*)in, (char*)out, len, (const char*)nkf_option);
	strcpy((char*)in, (char*)out);
	delete[]out;
	//memcpy((char*)in,(char*)out,len);
	return;
}
/********************************************************************************/
// ���{�ꕶ���R�[�h�ϊ��B
// (libnkf�̃��b�p�[�֐�)
//
//      �T�|�[�g����Ă���`���͈ȉ��̒ʂ�B
//              in_flag:        CODE_AUTO, CODE_SJIS, CODE_EUC, CODE_UTF8, CODE_UTF16
//              out_flag:       CODE_SJIS, CODE_EUC
/********************************************************************************/
wString wString::nkfcnv(const wString& option)
{
	wString ptr(len * 3);
	//=================================================
	// libnkf ���s
	//=================================================
	nkf((const char*)String, (char*)ptr.c_str(), len * 3, (const char*)option.c_str());
	ptr.len = strlen(ptr.c_str());
	return ptr;
}
#endif
//---------------------------------------------------------------------------
void wString::Add(const char* str)
{
	*this += str;
	*this += "\r\n";
}
//---------------------------------------------------------------------------
void wString::Add(const wString& str)
{
	*this += str;
	*this += "\r\n";
}

//---------------------------------------------------------------------------
//�@TStringList�΍�
//  �s����Ԃ�
//---------------------------------------------------------------------------
int wString::getLines(void)
{
	int count = 0;
	char* ptr = String;
	while (*ptr) {
		if (*ptr == '\r') {
			ptr++;
			continue;
		}
		if (*ptr++ == '\n') {
			count++;
		}
	}
	return count;
}

//---------------- -----------------------------------------------------------
//�@TStringList�΍�
//  �s����Ԃ�
//�@���G���g�����g�łȂ����Ƃɒ���
//�@�߂�l�͎g���̂�
//---------------- -----------------------------------------------------------
bool wString::SetListString(wString& src, int pos)
{
	bool flag;
	flag = SetListString(src.String, pos);
	return flag;
}
//---------------- -----------------------------------------------------------
//�@TStringList�΍�
//  pos�s��(0start)��dst��}��
//---------------- -----------------------------------------------------------
bool wString::SetListString(const char* dst, int pos)
{
	//�s��������
	std::vector<wString> work;
	int count = 0;
	int ptr = 0;
	int last = 0;
	int cchar = 0;
	while (String[ptr]) {
		switch (String[ptr]) {
		case '\r':
			ptr++;
			cchar++;
			continue;
		case '\n':
			ptr++;
			cchar++;
			count++;
			work.push_back(substr(last, ptr - last - cchar));
			cchar = 0;
			last = ptr;
			break;
		default:
			cchar = 0;
			ptr++;
		}
	}
	//�s����crlf���Ȃ������ꍇ�ɑΉ�
	if (ptr - last - cchar) {
		count++;
		work.push_back(substr(last, ptr - last - cchar));
	}
	//�v�f�ǉ�
	if (0 <= pos && pos <= count) {
		work[pos] = dst;
		//��������
		clear();
		for (size_t i = 0; i < work.size(); i++) {
			*this += work[i] + "\r\n";
		}
		return true;
	}
	else {
		//�Ȃɂ����Ȃ�
		return false;
	}
}
//---------------------------------------------------------------------------
//�@TStringList�΍�
//�@pos�s�ڂ̕������Ԃ�
wString wString::GetListString(int pos)
{
	int   lcount = 0;
	int   ptr = 0;
	int   curptr = 0;
	int   lstptr;
	int   spcnt = 0;//\r\n�܂���\n�̕�����
	while (String[ptr]) {
		if (String[ptr] == '\r') {
			ptr++;
			spcnt++;
			continue;
		}
		if (String[ptr++] == '\n') {
			lstptr = curptr;
			curptr = ptr;
			spcnt++;
			if (lcount++ == pos) {
				return substr(lstptr, curptr - lstptr - spcnt);
			}
		}
		spcnt = 0;
	}
	return wString();
}
#if 0
//---------------------------------------------------------------------------
//HTTP��蕶����f�[�^���擾
//����
// char* src    :URL
// off_t offset :�ǂݍ��݃I�t�Z�b�g
//�߂�l
// wString&     :�ǂݍ��񂾕�����B���s�����Ƃ��͒����O
//---------------------------------------------------------------------------
wString wString::HTTPGet(const char* url, off_t offset)
{
	int         recv_len;                       //�ǂݎ�蒷��
	wString     buf;
	wString     ptr;
	wString     host;                           //�z�X�g��
	wString     target;                         //�t�@�C����
	int         work1;
	int         work2;
	int         work3;
	SOCKET      server_socket;                  //�T�[�o�[�\�P�b�g
	int         server_port = HTTP_SERVER_PORT;
	//�o�̓t�@�C���̐ݒ�
	// ================
	// ���̓]���J�n
	// ================
	//buf = (char*)malloc(HTTP_BUF_SIZE);
	//ptr = buf;
	//����
	//�A�h���X����A�z�X�g���ƃ^�[�Q�b�g���擾
	ptr.SetLength(HTTP_STR_BUF_SIZE + 1);
	buf = url;
	//ptr = 0;
	work1 = buf.Pos("://") + 3;
	work2 = buf.Pos("/", work1);
	work3 = buf.Pos(":", work1);
	target = buf.SubString(work2, buf.len - work2);
	if (work3 >= 0) {
		host = buf.SubString(work1, work3 - work1);
		server_port = atoi(buf.c_str() + work3 + 1);
	}
	else {
		host = buf.SubString(work1, work2 - work1);
	}
	//�\�P�b�g�쐬�Ɛڑ�
	server_socket = sock_connect(host.String, server_port);
	if (!SERROR(server_socket)) {
		//HTTP1.0 GET���s
		ptr.sprintf("GET %s HTTP/1.0\r\n"
			"Accept: */*\r\n"
			"User-Agent: %s%s\r\nHost: %s\r\nRange: bytes=%llu-\r\nConnection: close\r\n\r\n",
			target.String,
			//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
			USERAGENT,
			MACADDR,
			host.String,
			offset);
		//ptr.len = strlen(ptr.String);
		//�T�[�o�Ɍq������
		if (send(server_socket, ptr.String, ptr.len, 0) != SOCKET_ERROR) {

			//���񕪂���w�b�_���폜
			recv_len = recv(server_socket, ptr.String, ptr.Total() - 1, 0);
			ptr.String[recv_len] = 0;
			ptr.len = recv_len;
			//������Ȃ�
			work1 = atoi(ptr.String + (ptr.Pos(" ") + 1));
			if (work1 < 200 || 300 <= work1) {
				sClose(server_socket);
				return wString();
			}
			//content_length = atoi(buf.String+buf.Pos("Content-Length:" )+16);

			//\r\n\r\n��T��
			work1 = ptr.Pos(HTTP_DELIMITER) + 4;//sizeof( HTTP_DELIMITER );//���̂̐擪
			recv_len -= work1;
			buf = ptr.SubString(work1, recv_len);
			//�]������
			while (loop_flag) {
				recv_len = recv(server_socket, ptr.String, ptr.Total() - 1, 0);
				if (recv_len <= 0) {
					break;
				}
				//�G���[�ɂȂ�Ȃ��B
				ptr.len = recv_len;
				ptr.String[recv_len] = 0;
				buf += ptr;
			}
		}
		else {
			sClose(server_socket);
			return wString();
		}
		sClose(server_socket);
	}
	else {
		return wString();
	}
	wString tmp(buf);
	//�I��
	return tmp;
}
//---------------------------------------------------------------------------
/* �\�P�b�g���쐬���A����ɐڑ����郉�b�p. ���s = -1 */
//static int sock_connect(char *host, int port)
//---------------------------------------------------------------------------
SOCKET wString::sock_connect(const wString& host, const int port)
{
	return sock_connect(host.String, port);
}
//---------------------------------------------------------------------------
SOCKET wString::sock_connect(const char* host, const int port)
{
	//  int sock;
	SOCKET sock;
	struct sockaddr_in sockadd = { 0 };     //�r�n�b�j�d�s�\����
	struct hostent* hent;
	debug_log_output("sock_connect: %s:%d", host, port);
	//�r�n�b�j�d�s�쐬
	if (SERROR(sock = socket(PF_INET, SOCK_STREAM, 0))) {
		debug_log_output("sock_connect_error:");
		return INVALID_SOCKET;
	}
	//debug_log_output("sock: %d", sock);
	if (NULL == (hent = gethostbyname(host))) {
		sClose(sock);
		return INVALID_SOCKET;
	}
	debug_log_output("hent: %p", hent);
	//�\�P�b�g�\���̂փA�h���X�ݒ�
	memcpy(&sockadd.sin_addr, hent->h_addr, hent->h_length);
	//�\�P�b�g�\���̂փ|�[�g�ݒ�
	sockadd.sin_port = htons((u_short)port);
	//�h�o�u�S�A�h���X�t�@�~����ݒ�
	sockadd.sin_family = AF_INET;
	//�ڑ�
	if (SERROR(connect(sock, (struct sockaddr*) & sockadd, sizeof(sockadd)))) {
		sClose(sock);
		return INVALID_SOCKET;
	}
	return sock;
}
//---------------------------------------------------------------------------
// 2004/11/17 Add start
// �A�N�Z�X�\���`�F�b�N����
//�`����www.make-it.co.jp/index.html
//2005/12/03 �������󂵂Ă����̂ŏC��
bool wString::checkUrl(const wString& url)
{
	wString buf;
	int     recv_len;
	bool    access_flag = false;
	wString host_name;
	wString file_path;
	int     ptr;
	//�O����
	ptr = url.Pos("/");
	// �͂��߂ɏo�Ă���"/"�̑O��ŕ��f
	host_name = url.SubString(0, ptr);
	file_path = url.SubString(ptr, url.Length() - ptr);
	//������Ȃ�������
	if (file_path.Length() == 0) {
		file_path = "/";
	}
	SOCKET server_socket = sock_connect(host_name.c_str(), HTTP_SERVER_PORT);//( PF_INET , SOCK_STREAM , 0 );
	if (!SERROR(server_socket)) {
		buf.sprintf("HEAD %s HTTP/1.0\r\n"
			"User%cAgent: "
			USERAGENT
			"\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			file_path.c_str(),
			'-',
			host_name.c_str()
		);
		int dd = send(server_socket, buf.c_str(), buf.Length(), 0);
		if (dd != SOCKET_ERROR) {
			buf.SetLength(1024);
			recv_len = recv(server_socket, buf.c_str(), buf.Total() - 1, 0);
			buf.ResetLength(recv_len);
			//������Ȃ�
			ptr = atoi(buf.String + (buf.Pos(" ") + 1));
			// ��M�f�[�^����Ȃ��(�t�@�C���L��j�A�f�[�^����͂���B
			if (200 <= ptr && ptr < 300) {
				access_flag = true;
			}
		}
		sClose(server_socket);
	}
	return access_flag;
}
//---------------------------------------------------------------------------
//�ڕW�̃T�C�Y�擾
int wString::HTTPSize(const wString& url)
{

	int         recv_len;                       //�ǂݎ�蒷��
	wString     buf;
	int         work1;
	int         work2;
	int         work3;
	wString     host;
	wString     target;
	int         content_length = -1;
	SOCKET      server_socket;                          //�T�[�o�[�\�P�b�g
	int         server_port = HTTP_SERVER_PORT;
	//�o�̓t�@�C���̐ݒ�
	// ================
	// ���̓]���J�n
	// ================
	//����
	//�A�h���X����A�z�X�g���ƃ^�[�Q�b�g���擾
	work2 = url.Pos("://") + 3;
	work1 = url.Pos("/", work2);
	work3 = url.Pos(":", work1);
	target = url.SubString(work1, url.Length() - work1);
	if (work3 >= 0) {
		host = url.SubString(work2, work3 - work2);
		server_port = atoi(url.c_str() + work3 + 1);
	}
	else {
		host = url.SubString(work2, work1 - work2);
	}
	//strcpy( target, work1);
	//*work1 = 0;
	//strcpy( host, work2 );

	//�\�P�b�g�쐬�Ɛڑ�
	server_socket = sock_connect(host.c_str(), server_port);
	if (!SERROR(server_socket)) {
		//HTTP1.0 GET���s
		buf.sprintf("HEAD %s HTTP/1.0\r\n"
			"Accept: */*\r\n"
			"User-Agent: %s%s\r\n"
			"Host: %s\r\n"
			//                       "%s",
			"Connection: close\r\n\r\n",
			target.uri_encode().c_str(),
			//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
			USERAGENT,
			MACADDR,
			host.c_str()
			//                        GetAuthorization(void),
		);
		//�T�[�o�Ɍq������
		if (send(server_socket, buf.c_str(), buf.Length(), 0) != SOCKET_ERROR) {
			//���񕪂���w�b�_���폜
			buf.SetLength(HTTP_STR_BUF_SIZE);
			recv_len = recv(server_socket, buf.c_str(), buf.Total() - 1, 0);
			//\r\n\r\n��T��
			buf.ResetLength(recv_len);      //���~��
			work1 = atoi(buf.String + (buf.Pos(" ") + 1));
			int pos = buf.Pos("Content-Length:");
			if (pos >= 0) {
				if (200 <= work1 && work1 < 300) {
					//�R���e���c���� TODO Content-Length���Ȃ��ꍇ
					content_length = atoi(buf.c_str() + pos + 16);
				}
			}
			else if (work1 == 302) {
				//Location:----\r\n
				work1 = buf.Pos("Location:");
				if (work1) {
					int num = buf.Pos("\r\n", work1) - work1 - 10;
					buf = buf.SubString(work1 + 10, num);
					content_length = HTTPSize(buf);
				}
			}
		}
		sClose(server_socket);
	}
	//�I��
	return content_length;
}
#endif
//---------------------------------------------------------------------------
// linux�p�t�@�C�����ɕϊ�
//---------------------------------------------------------------------------
char* wString::WindowsFileName(char* FileName)
{
#ifdef linux
	return FileName;
#else
	char* work = FileName;
	int ptr = 0;
	while (work[ptr]) {
		if (work[ptr] == '/') {
			work[ptr] = '\\';
		}
		ptr++;
	}
	return work;
#endif
}
//---------------------------------------------------------------------------
// Linux�p�t�@�C�����ɕϊ�
//---------------------------------------------------------------------------
char* wString::LinuxFileName(char* FileName)
{
#ifdef linux
	return FileName;
#else
	char* work = FileName;
	int ptr = 0;
	while (work[ptr]) {
		if (work[ptr] == '\\') {
			work[ptr] = '/';
		}
		ptr++;
	}
	return work;
#endif
}
#if 0
//---------------------------------------------------------------------------
// Linux�p�t�@�C�����ɕϊ�
//---------------------------------------------------------------------------
char* wString::GetLocalAddress(void)
{
#ifdef linux
	//TODO linux�Ԃ̎�IP/�z�X�g����ݒ肷�邱��
	return (char*)"neon.cx";
#else

	//�z�X�g�����擾����
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) != 0) {
		return 0;
	}
	//puts(hostname);

	//�z�X�g������IP�A�h���X���擾����
	HOSTENT* hostend = gethostbyname(hostname);
	if (hostend == NULL) {
		return 0;
	}
	IN_ADDR inaddr;
	memcpy(&inaddr, hostend->h_addr_list[0], 4);
	static char ip[256];
	strcpy(ip, inet_ntoa(inaddr));
	return ip;
#endif
}
#endif
//---------------------------------------------------------------------------
size_t wString::copy(char* str, size_t slen, size_t index) const
{
	strncpy(str, String + index, slen);
	str[slen] = 0;
	return slen;
}
wString& wString::replace(size_t index, size_t slen, const wString& repstr)
{
	size_t rlen = repstr.len;
	//����
	if (slen == rlen) {
		memcpy((void*)(String + index), (void*)repstr.String, rlen);
		//�O�l�ߒu�����̂܂܃R�s�[����΂���
	}
	else if (slen > rlen) {
		size_t num = slen - rlen;
		char* p = String + index;
		char* q = p + num;
		while (*q) {
			*p++ = *q++;
		}
		memcpy((void*)(String + index), (void*)(repstr.String), rlen);
		len -= num;
		String[len] = 0;
		//�u�������������̂Ō�l�߂���
	}
	else {
		size_t num = rlen - slen;
		myrealloc(len + num + 1);
		for (char* p = (char*)(String + len + num); p > String + index + num; p--) {
			*p = *(p - num);
		}
		memcpy((void*)(String + index), (void*)(repstr.String), rlen);
		len += num;
		String[len] = 0;
	}
	return *this;
}



