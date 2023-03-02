/*
 * fflog.cpp
 *
 *  Created on: May 6, 2022
 *      Author: Jesse Wang
 */

#include "log.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>


struct fflog_t
{
    char path[512];
    int max_num_files;
    int max_filesize;
    pthread_mutex_t mutex;
    int opened_log_index;
    FILE* log_fp;
    char buf[FFLOG_BUFSIZE];
};

static int fflog_file_exists(const char* path)
{
    struct stat path_stat;
    int r = stat(path, &path_stat);
    if(r == 0)
        return 1;
    else
        return 0;
}

fflog_t* fflog_create(const char* path, int max_num_files, int max_filesize)
{
    fflog_t* ret = new fflog_t;
    strcpy(ret->path, path);
    ret->max_num_files = max_num_files;
    ret->max_filesize = max_filesize;
    ret->log_fp = NULL;
    ret->opened_log_index = 0;
    pthread_mutex_init(&ret->mutex, NULL);
    char log_path[512];
    for(;ret->opened_log_index < ret->max_num_files; ret->opened_log_index++)
    {
        sprintf(log_path, "%s%d", ret->path, ret->opened_log_index);
        if(!fflog_file_exists(log_path))
            break;
    }
    if(ret->opened_log_index > 0)
    {
        ret->opened_log_index--;
        sprintf(log_path, "%s%d", ret->path, ret->opened_log_index);
    }
    ret->log_fp = fopen(log_path, "a");
    if(ret->log_fp == NULL)
    {
        pthread_mutex_destroy(&ret->mutex);
        delete ret;
        return NULL;
    }
    return ret;
}

void fflog_destroy(fflog_t* ffl)
{
    pthread_mutex_destroy(&ffl->mutex);
    if(ffl->log_fp)
        fclose(ffl->log_fp);
    delete ffl;
}

static void ff_log_to_file(fflog_t* ffl, char* buf, int len)
{
    if(ffl->log_fp)
    {
        int pos = ftell(ffl->log_fp);
        if(pos+len > ffl->max_filesize)
        {
            if(ffl->opened_log_index+1 < ffl->max_num_files)
            {
                ffl->opened_log_index++;
                fclose(ffl->log_fp);
                char log_path[256];
                sprintf(log_path, "%s%d", ffl->path, ffl->opened_log_index);
                ffl->log_fp = fopen(log_path, "a");
                if(ffl->log_fp == NULL)
                {
                    return;
                }
            }
            else
            {
                fclose(ffl->log_fp);
                char log_path[256];
                char log_path2[256];
                char cmd[256];
                sprintf(log_path, "%s%d", ffl->path, 0);
                if(fflog_file_exists(log_path))
                    remove(log_path);
                int i;
                for(i=0;i<ffl->max_num_files;i++)
                {
                    if(!fflog_file_exists(log_path))
                    {
                        int k;
                        for(k=i+1;k<ffl->max_num_files;k++)
                        {
                            sprintf(log_path2, "%s%d", ffl->path, k);
                            if(fflog_file_exists(log_path2))
                            {
                                sprintf(cmd, "mv %s %s", log_path2, log_path);
                                system(cmd);
                                break;
                            }
                        }
                        if(k == ffl->max_num_files)
                            break;
                    }
                    if(i+1 < ffl->max_num_files)
                        sprintf(log_path, "%s%d", ffl->path, i+1);
                }
                if(i==ffl->max_num_files)
                {
                    i = 0;
                    sprintf(log_path, "%s%d", ffl->path, 0);
                }
                ffl->opened_log_index = i;
                ffl->log_fp = fopen(log_path, "a");
                if(ffl->log_fp == NULL)
                {
                    return;
                }
            }
        }
        if(len+1 > FFLOG_BUFSIZE)
           return;
        buf[len] = '\n';
        fwrite(buf, 1, len+1, ffl->log_fp);
        fflush(ffl->log_fp);
    }
}

void fflog_out(fflog_t* ffl, const char* format, ...)
{
    pthread_mutex_lock(&ffl->mutex);
    va_list va;
    va_start(va, format);
    time_t rawtime;
    struct tm info;
    time(&rawtime);
    localtime_r(&rawtime, &info);
    sprintf(ffl->buf, "%02d:%02d:%02d", info.tm_hour, info.tm_min, info.tm_sec);
    int len = strlen(ffl->buf);
    ffl->buf[len] = 0x20;
    int r = vsnprintf(&ffl->buf[len+1], FFLOG_BUFSIZE-(len+1), format, va);
    if(r < FFLOG_BUFSIZE-(len+1))
    {
        ff_log_to_file(ffl, ffl->buf, r+len+1);
    }
    va_end(va);
    pthread_mutex_unlock(&ffl->mutex);
}

void fflog_out_timeless(fflog_t* ffl, const char* format, ...)
{
    pthread_mutex_lock(&ffl->mutex);
    va_list va;
    va_start(va, format);
    int r = vsnprintf(&ffl->buf[0], FFLOG_BUFSIZE, format, va);
    if(r < FFLOG_BUFSIZE)
        ff_log_to_file(ffl, ffl->buf, r);
    va_end(va);
    pthread_mutex_unlock(&ffl->mutex);
}
