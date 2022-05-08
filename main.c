#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>

#define THIS_FILE "Sip Client"

char server_ip[15];
char user_name[20];
char password[30];

int main()
{
	printf("Server IP: ");
	scanf("%s",server_ip);
	printf("User name: ");
	scanf("%s",user_name);
	printf("%s's password: ",user_name);
	scanf("%s",password);
	
	pj_status_t status;
	pjsua_config ua_cfg;
	pjsua_logging_config log_cfg;
	
	status = pjsua_create();
    	if (status != PJ_SUCCESS) {
        	pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
        	return status;
    	}
    	
    	//pj_status_t status;
    	pjsua_config_default(&ua_cfg);
	pjsua_logging_config_default(&log_cfg);
	status = pjsua_init(&ua_cfg, &log_cfg, NULL);
	
	if (status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
		return status;
    	}
	
	//pj_status_t status;
	pjsua_transport_config transport_cfg;
     
     	pjsua_transport_config_default(&transport_cfg);
     	transport_cfg.port = 0;
     
     	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, NULL);
     	if (status != PJ_SUCCESS) 
     	{
     		pjsua_perror(THIS_FILE, "Error creating transport", status);
     		return status;
        }
        
        //pj_status_t status;
	status = pjsua_start();       
	if (status != PJ_SUCCESS) 
    	{
     		pjsua_perror(THIS_FILE, "Error starting pjsua", status);
     		return status;
        }
        
        //pj_status_t status;
	pjsua_acc_config acc_cfg;
	pjsua_acc_id acc_id;
	
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
	acc_cfg.cred_info[0].data_type = 0;//PJSIP_CRED_DATA_PLAIN_PASSWD;
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
        
	pjsua_destroy();  
	return 0;	
}
