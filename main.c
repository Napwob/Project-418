#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include "pjgtkstuff.h"
#include <gtk/gtk.h>

#define THIS_FILE "Sip Client"

int main()
{	
	//registration_interface();
	pj_initialisation();
	main_interface();
	pjsua_destroy();  
	return 0;	
}
