#ifndef PJSTUFF_H
#define PJSTUFF_H

pj_status_t configurate_init_PJSUA();
pj_status_t create_transport();
pj_status_t start_PJSUA();
pj_status_t configurate_account(char* server_ip,char* user_name,char* password);
pj_status_t account_registration();
pj_status_t call_someone(char* server_ip);
#endif
