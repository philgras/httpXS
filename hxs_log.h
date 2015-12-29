#ifndef HXS_LOG_H_
#define HXS_LOG_H_


#include <stdio.h>
#include <stdarg.h>


typedef struct{

    const char* file_name;
    FILE* open_file;

} hxs_log_t;

static inline
int hxs_log_open(hxs_log_t* log, const char* fname){
   log->open_file = fopen(fname, "w");
   if(log->open_file == NULL){
      log->file_name = "";
      return HXS_ERROR;
   }
   log->file_name = fname;
   return HXS_OK;

}

static inline
void hxs_log_errno(hxs_log_t* log, int err, const char* sourcefile,
                    int line){

    fprintf(log->open_file,"[Error] %s at line %d: %s\n",
            sourcefile,line,strerror(err));

}

static inline
void hxs_log_error(hxs_log_t* log, const char* msg, ...){

    va_list para;
    va_start(para, msg);
    fprintf(log->open_file,msg,para);
    va_end(para);

}

static inline
int hxs_log_close(hxs_log_t* log){
    int rc  = HXS_OK;

    fflush(log->open_file);
    if(fclose(log->open_file) != NULL){

        rc =  HXS_ERROR;
    }

    log->open_file = NULL;
    return rc;

}


#endif
