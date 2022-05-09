#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include "pjstuff.h"

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
	
	configurate_init_PJSUA();
	create_transport();
	start_PJSUA();
	configurate_account(server_ip,user_name,password);
	account_registration();
        
        sleep(30);
	pjsua_destroy();  
	return 0;	
}
