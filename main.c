#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include "pjgtkstuff.h"
#include <gtk/gtk.h>

#define THIS_FILE "Sip Client"

int main()
{
	//printf("Server IP: ");
	//scanf("%s",server_ip);
	//printf("User name: ");
	//scanf("%s",user_name);
	//printf("%s's password: ",user_name);
	//scanf("%s",password);
	//printf("Server IP: %s\n",server_ip);
	//printf("User name: %s\n", user_name);
	//printf("%s's password: %s\n",user_name,password);
	
	registration_interface();
	pj_initialisation();
	main_interface();
	
        sleep(30);
	pjsua_destroy();  
	return 0;	
}
