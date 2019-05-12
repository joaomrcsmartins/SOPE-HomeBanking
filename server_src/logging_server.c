#include "logging_server.h"

void open_server(int bank_id){
    FILE* f = fopen(SERVER_LOGFILE, "w");
    int fd = fileno(f);
    pthread_t tid = pthread_self();

    logBankOfficeOpen(fd, bank_id, tid);

    close(fd);
    fclose(f);
}

void close_server(int bank_id){
    FILE* f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);
    pthread_t tid = pthread_self();

    logBankOfficeClose(fd, bank_id, tid);

    close(fd);
    fclose(f);
}

void log_reply(tlv_reply_t *reply, int bank_id){
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logReply(fd, bank_id, reply);
    
    close(fd);
    fclose(f);
}

void log_request(tlv_request_t *request, int bank_id){
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logRequest(fd, bank_id, request);
    
    close(fd);
    fclose(f);
}
