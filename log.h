/*
 * fflog.h
 *
 *  Created on: Apr 16, 2018
 *      Author: Jesse Wang
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define FFLOG_BUFSIZE 4096
#define FFLOG_DEBUG_PRINT_ENABLED 1
#define FFLOG_PRINT_ENABLED 1

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "libff"
#define fflog_debug_print(...) do{if(FFLOG_DEBUG_PRINT_ENABLED){\
    char strbuf[1024];\
    int r = sprintf(strbuf, "%s:%d:%s(): ", __FILE__, __LINE__, __func__);\
    sprintf(strbuf+r, __VA_ARGS__);\
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"%s", strbuf);}}while(0)

#define fflog_print(...) do{if(FFLOG_PRINT_ENABLED){\
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__);}}while(0)
#else

#define fflog_debug_print(...) do{if(FFLOG_DEBUG_PRINT_ENABLED){\
    char strbuf[1024];\
    int r = sprintf(strbuf, "%s:%d:%s(): ", __FILE__, __LINE__, __func__);\
    sprintf(strbuf+r, __VA_ARGS__);\
    printf("%s", strbuf); fflush(stdout);}}while(0)

#define fflog_print(...) do{if(FFLOG_PRINT_ENABLED){\
    printf(__VA_ARGS__); fflush(stdout);}}while(0)
#endif

struct fflog_t;

//path - Directory and file prefix for logs.
//       Example: /var/mylog would create log paths such as
//                /var/mylog0, /var/mylog1, etc.
//max_num_files - Max num files of 2 would keep a maximum
//                of 2 files named, for example, /var/mylog0 and
//                /var/mylog1
//max_filesize - Max filesize of each file in bytes.
//Returns NULL if creation failed.
fflog_t* fflog_create(const char* path, int max_num_files, int max_filesize);

void fflog_destroy(fflog_t* ffl);

//Outputs to the log file with the highest post-fix number.
//Note that if the final output length is greater than
//FFLOG_BUFSIZE, this function call is ignored.
//The output content is prepended with the time.
void fflog_out(fflog_t* jzl, const char* format, ...);

//The output content is not prepended with the time.
void fflog_out_timeless(fflog_t* jzl, const char* format, ...);

#endif // LOG_H
