#ifndef PJGTKSTUFF_H
#define PJGTKSTUFF_H

pj_status_t configurate_init_PJSUA();
pj_status_t create_transport();
pj_status_t start_PJSUA();
pj_status_t configurate_account(char* server_ip,char* user_name,char* password);
pj_status_t account_registration();
pj_status_t call_someone(char* server_ip,char* call_sip);
void check_and_load_cache();
void registration_interface();
void main_interface();
void pj_initialisation();
static void init_ringtone_player();
#endif
