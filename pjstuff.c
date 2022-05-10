#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>

pjsua_acc_id acc_id;	
#define THIS_FILE "Sip Client"

//MAIN FUNCTIONALITY STUFF
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
	printf("Incoming call from %.*s\n", (int)ci.remote_info.slen, ci.remote_info.ptr);
	int a;
	printf("Answer?(1/0): ");
	scanf("%d",&a);
	if(a == 1)
		pjsua_call_answer(call_id, 200, NULL, NULL);
	else
		pjsua_call_answer(call_id, 486, NULL, NULL);
}

static void on_call_media_state(pjsua_call_id call_id)
{
	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
		pjsua_conf_connect(ci.conf_slot, 0);
		pjsua_conf_connect(0, ci.conf_slot);
	}
}

pj_status_t call_someone(char* server_ip)
{
	pj_status_t status;
	char ct[30];
	char name[10];
	pj_str_t uri;
	printf("Calling to: ");
	scanf("%s",name);
     	if (pjsua_verify_url(name) != PJ_SUCCESS) 
     	{
		snprintf(ct, sizeof(ct), "sip:%s@%s", name, server_ip); 
		if (pjsua_verify_url(ct) != PJ_SUCCESS) 
		{
			printf("Invalid URL entered. Try again. \n");
			return status;
		}
		else uri = pj_str(ct);
			
	} 
		else uri = pj_str(name);
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);	
}


//INITIALISATION STUFF
pj_status_t configurate_init_PJSUA()
{	
	pj_status_t status;
	status = pjsua_create();
    	if (status != PJ_SUCCESS) {
        	pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
        	return status;
    	}
    	
	pjsua_config ua_cfg;
	pjsua_logging_config log_cfg;
	pjsua_media_config media_cfg;
    	
    	//look for it later
    	pjsua_config_default(&ua_cfg);
	pjsua_logging_config_default(&log_cfg);
	pjsua_media_config_default(&media_cfg);
	
	log_cfg.level = 0;
	log_cfg.console_level = 0;
	
	ua_cfg.cb.on_incoming_call = &on_incoming_call;
	ua_cfg.cb.on_call_media_state = &on_call_media_state;
		
	status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
	if (status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
		return status;
    	}
}

pj_status_t create_transport()
{
	pj_status_t status;
	pjsua_transport_config transport_cfg;
     
     	pjsua_transport_config_default(&transport_cfg);
     	transport_cfg.port = 0;
     
     	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, NULL);
     	if (status != PJ_SUCCESS) 
     	{
     		pjsua_perror(THIS_FILE, "Error creating transport", status);
     		return status;
        }
}

pj_status_t start_PJSUA()
{
	pj_status_t status;
	status = pjsua_start();       
	if (status != PJ_SUCCESS) 
    	{
     		pjsua_perror(THIS_FILE, "Error starting pjsua", status);
     		return status;
        }
}

pj_status_t configurate_account(char* server_ip,char* user_name,char* password)
{
	pj_status_t status;
	pjsua_acc_config acc_cfg;
	
	pjsua_acc_config_default(&acc_cfg);
	char sip_uri[50];
	snprintf(sip_uri, sizeof(sip_uri), "sip:%s@%s", user_name, server_ip); 
	char sip[50];
	snprintf(sip, sizeof(sip), "sip:%s", server_ip); 
	acc_cfg.id = pj_str(sip_uri);
	acc_cfg.reg_uri = pj_str(sip);
	acc_cfg.cred_count = 1;
	acc_cfg.cred_info[0].realm = pj_str("asterisk");
	acc_cfg.cred_info[0].scheme = pj_str("digest");
	acc_cfg.cred_info[0].username = pj_str(user_name);
	acc_cfg.cred_info[0].data_type = 0;
	acc_cfg.cred_info[0].data = pj_str(password);
	acc_cfg.register_on_acc_add = PJ_FALSE;
 
	status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
	if (status != PJ_SUCCESS) 
	{
     		pjsua_perror(THIS_FILE, "Error adding account", status);
     		return status;
        }  

	status = pjsua_acc_set_online_status(0, PJ_TRUE);
	if (status != PJ_SUCCESS) 
	{
     		pjsua_perror(THIS_FILE, "Error modifying account presence", status);
     		return status;
        } 
}

pj_status_t account_registration()
{
	pj_status_t status;
	status = pjsua_acc_set_registration(0, PJ_TRUE);
	if (status != PJ_SUCCESS) 
	{
     		pjsua_perror(THIS_FILE, "Error account registration", status);
     		return status;
        } 
}















