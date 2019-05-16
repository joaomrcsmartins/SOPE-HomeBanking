#include "server.h"
#include "server_parser.h"
#include "process_request.h"
#include "../auxiliary_code/show_info.h"
#include "request_queue.h"
#include "e_counter.h"
#include <time.h>

//static bank_account_t admin; //is it necessary?

extern bool server_shutdown;

static request_queue_t* request_queue;

int receive_requests();

int main (int argc, char *argv []) {
    srand(time(NULL));

    if(argc != 3) {
        printf("Wrong number of arguments.\n");
        show_usage_server();
        return FAILURE;
    }

    //parse input and create admin account
    int nthr;
    bank_account_t* admin = malloc(sizeof(bank_account_t));
    memset(admin,0,sizeof(bank_account_t));

    if(input_parser(argv,admin,&nthr) != SUCCESS) {
        show_usage_server();
        return FAILURE;
    }

    //initalize request queue
    request_queue = request_queue_init(nthr);

    //create threads (MISSING)
    init_e_counters();
    create_e_counters(request_queue,nthr);

    //logBankOfficeOpen
    open_office(ADMIN_ACCOUNT_ID);

    //load admin into bank accounts
    load_admin(admin);
    
    //log admin account creation 
    log_creat_acc(admin,ADMIN_ACCOUNT_ID);


    if(mkfifo(SERVER_FIFO_PATH,RDWR_USGR) < 0) {
        if(errno==EEXIST) {
            perror(SERVER_FIFO_PATH);
            return FAILURE;
        }
        else
            return FAILURE;
    }

    int rc = receive_requests();

    //after all threads finish processing their requests call this
    clean_accounts();

    if(unlink(SERVER_FIFO_PATH) < 0) {
        printf("Error when destroying FIFO '%s'\n",SERVER_FIFO_PATH);
        return FAILURE;
    }
    else
        printf("FIFO '%s' has been destroyed\n",SERVER_FIFO_PATH);
    
    close_office(ADMIN_ACCOUNT_ID);

    request_queue_delete(request_queue);

    return rc;
}

int receive_requests() {

    //receive user requests
    int rq;
    tlv_request_t request;
    memset(&request,0,sizeof(tlv_request_t));
    
    if((rq = open(SERVER_FIFO_PATH,O_RDONLY)) == -1) {
        return FAILURE;
    }    
    
    do
    {
        if(read(rq, &request, sizeof(request)) == 0) {
            usleep(500);
            continue;
        }

        request_queue_push(request_queue,request);
        
    } while (!server_shutdown); //needs to check for valid request

    wait_for_e_counters();

    close(rq);
    
    return SUCCESS;
}