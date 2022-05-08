#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>

#define THIS_FILE "Sip Client"

int main()
{
	/*char server_ip[15];
	char name_user[20];
	char password_user[30];
	
	printf("Server IP: ");
	scanf("%s",server_ip);
	printf("User name: ");
	scanf("%s",name_user);
	printf("%s's password: ",name_user);
	scanf("%s",password_user);*/
	
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
        
	return 0;	
}
