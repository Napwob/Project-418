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
	check_and_load_cache();
	pj_initialisation();
	main_interface();
	return 0;	
}
