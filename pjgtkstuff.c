#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include <gtk/gtk.h>
#include "pjgtkstuff.h"

char server_ip[15];
char user_name[20];
char password[30];
GtkWidget *ip_label, *ip_entry;
GtkWidget *login_label, *login_entry;
GtkWidget *password_label, *password_entry;
GtkWidget *sip_label, *sip_entry;
pjsua_acc_id acc_id;	
GtkWidget *answer_button;
pjsua_call_id call_to_answer;
#define THIS_FILE "Sip Client"

//MAIN INTERFACE STUFF
void set_button_clicked(GtkWidget *button, gpointer data)
{
    sprintf(server_ip,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) ip_entry)));
    sprintf(user_name,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry)));
    sprintf(password,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry)));
    //printf("%s\n%s\n%s\n",server_ip,user_name,password);
    gtk_main_quit();
}

void call_button_clicked(GtkWidget *button, gpointer data)
{
    char sip_call[20];
    sprintf(sip_call,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) sip_entry)));
    call_someone(server_ip, sip_call);
}

void answer_button_clicked(GtkWidget *button, gpointer data)
{
   pjsua_call_answer(call_to_answer, 200, NULL, NULL);
   gtk_widget_set_sensitive(answer_button, FALSE);
}

void del_button_clicked(GtkWidget *button, gpointer data)
{
    gtk_entry_set_text(GTK_ENTRY((GtkWidget*) ip_entry), "");
    gtk_entry_set_text(GTK_ENTRY((GtkWidget*) login_entry), "");
    gtk_entry_set_text(GTK_ENTRY((GtkWidget*) password_entry), "");
}

void registration_interface()
{
    GtkWidget *window;
    GtkWidget *hbox0, *hbox1, *hbox2, *hbox3;
    GtkWidget *vbox;
    GtkWidget *set_button, *del_button;

    gtk_init (NULL, NULL);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "REGISTRATION");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Создаем ярлык и поле ввода логина
    ip_label = gtk_label_new("Введите IP:   ");
    ip_entry = gtk_entry_new();

    // Создаем ярлык и поле ввода логина
    login_label = gtk_label_new("Введите логин:");
    login_entry = gtk_entry_new();

    // Создаем ярлык и поле ввода пароля
    password_label = gtk_label_new("Введите пароль:");
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);

    // Создаем кнопки
    set_button = gtk_button_new_with_label("Регистрация");
    del_button = gtk_button_new_with_label("Очистить");

    // Задаем функции кнопок
    g_signal_connect(GTK_BUTTON(set_button), "clicked", G_CALLBACK(set_button_clicked), login_entry);
    g_signal_connect(GTK_BUTTON(del_button), "clicked", G_CALLBACK(del_button_clicked), password_entry);

    hbox0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox0), ip_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox0), ip_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox0, TRUE, FALSE, 5);

    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), login_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), login_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, FALSE, 5);

    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), password_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), password_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, FALSE, 5);

    hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox3), set_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), del_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox3, TRUE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);

    gtk_main();
    gtk_widget_destroy(window);	
}

void main_interface()
{
    GtkWidget *window;
    GtkWidget *hbox0, *hbox1;
    GtkWidget *vbox;
    GtkWidget *call_button;

    gtk_init (NULL, NULL);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "SIPHONE");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Создаем ярлык и поле ввода логина
    sip_label = gtk_label_new("SIP-адрес:");
    sip_entry = gtk_entry_new();

    // Создаем кнопки
    call_button = gtk_button_new_with_label("Позвонить");
    answer_button = gtk_button_new_with_label("Принять вызов");

    // Задаем функции кнопок
    g_signal_connect(GTK_BUTTON(call_button), "clicked", G_CALLBACK(call_button_clicked), login_entry);
    g_signal_connect(GTK_BUTTON(answer_button), "clicked", G_CALLBACK(answer_button_clicked), password_entry);

    hbox0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox0), sip_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox0), sip_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox0, TRUE, FALSE, 5);

    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), call_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), answer_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, FALSE, 5);	
    
    gtk_widget_set_sensitive(answer_button, FALSE);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);

    gtk_main();
    gtk_widget_destroy(window);
    pjsua_destroy();  
    exit(0);
}


//MAIN FUNCTIONALITY STUFF
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
	printf("Incoming call from %.*s\n", (int)ci.remote_info.slen, ci.remote_info.ptr);
	gtk_widget_set_sensitive(answer_button, TRUE);
	call_to_answer = call_id;
	//pjsua_call_answer(call_id, 200, NULL, NULL);
	/*printf("Answer?(1/0): ");
	scanf("%d",&a);
	if(a == 1)
		pjsua_call_answer(call_id, 200, NULL, NULL);
	else
		pjsua_call_answer(call_id, 486, NULL, NULL);
	*/
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

pj_status_t call_someone(char* server_ip,char* call_sip)
{
	
	pj_status_t status;
	char ct[30];
	pj_str_t uri;
     	if (pjsua_verify_url(call_sip) != PJ_SUCCESS) 
     	{
		snprintf(ct, sizeof(ct), "sip:%s@%s", call_sip, server_ip); 
		if (pjsua_verify_url(ct) != PJ_SUCCESS) 
		{
			printf("Invalid URL entered. Try again. \n");
			return status;
		}
		else uri = pj_str(ct);
			
	} 
		else uri = pj_str(call_sip);
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);	
}


//INITIALISATION STUFF
void pj_initialisation()
{
	configurate_init_PJSUA();
	create_transport();
	start_PJSUA();
	configurate_account(server_ip,user_name,password);
	account_registration();
}

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















