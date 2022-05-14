#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include <gtk/gtk.h>
#include <pjmedia.h>
#include "pjgtkstuff.h"


typedef struct _ringtone_port_info {
    int ring_on;
    int ring_slot;
    pjmedia_port *ring_port;
    pj_pool_t *pool;
} ringtone_port_info_t;

static ringtone_port_info_t ringtone_port_info;
char server_ip[15]="192.168.56.102";
char user_name[20]="6000";
char password[30]="PASSWORD";

GtkWidget *ip_label, *ip_entry;
GtkWidget *login_label, *login_entry;
GtkWidget *password_label, *password_entry;
GtkWidget *sip_label, *sip_entry;
GtkWidget *call_label;
GtkWidget *answer_button;
GtkWidget *call_button;

pjsua_acc_id acc_id;	
pjsua_call_id call_to_answer;
pj_status_t stop_ring();
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
	char sip_call[40];
	sprintf(sip_call,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) sip_entry)));
	//printf("%s\n",sip_call);
	if(strcmp(sip_call,user_name) != 0)
	{
		//puts("there");
		if(strstr(sip_call,"sip:") != NULL && strstr(sip_call,"@") != NULL)
		{
			//puts("here");
			char verify_cash[20];
			strcpy(verify_cash,&sip_call[4]);
			int c = strchr(verify_cash, '@') - verify_cash;
			strncpy(verify_cash, verify_cash, c);
			verify_cash[c] = '\0';
			if(strcmp(verify_cash,user_name) == 0)
				return;	
		}
	} else return; 
	
	char ct[60];
	if (pjsua_verify_url(sip_call) != PJ_SUCCESS) 
	{
		//puts("Bad url!!!");
		snprintf(ct, sizeof(ct), "sip:%s@%s", sip_call, server_ip); 
		if (pjsua_verify_url(ct) != PJ_SUCCESS) 
		{
			//printf("Invalid URL entered. Try again. \n");
		}
			else 
		{
			//printf("%s\n",ct);
			call_someone(server_ip, ct);	
			return;	
		}			
	} 
	gtk_label_set_text((GtkLabel*)call_label,"Исходящий звонок");
	call_someone(server_ip, sip_call);
		
	return;
}

void answer_button_clicked(GtkWidget *button, gpointer data)
{
   stop_ring();
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
    ip_label = gtk_label_new("Введите IP:          ");
    ip_entry = gtk_entry_new();

    // Создаем ярлык и поле ввода логина
    login_label = gtk_label_new("Введите логин:  ");
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
    GtkWidget *hbox0, *hbox1, *hbox2;
    GtkWidget *vbox;

    gtk_init (NULL, NULL);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "SIPHONE");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Создаем ярлык и поле ввода логина
    sip_label = gtk_label_new("SIP-адрес:");
    call_label = gtk_label_new("Нет звонков");
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

    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), call_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, FALSE, 5);
	
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), call_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), answer_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, FALSE, 5);	
    
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
pj_status_t start_ring() {
    pj_status_t status;

    if (ringtone_port_info.ring_on) {
        printf("Ringtone port already connected\n");
        return PJ_SUCCESS;
    }

    //printf("Starting ringtone\n");
    status = pjsua_conf_connect(ringtone_port_info.ring_slot, 0);
    ringtone_port_info.ring_on = 1;
    if (status != PJ_SUCCESS)
        pjsua_perror(THIS_FILE, "Error connecting ringtone port", status);
    return status;
}

pj_status_t stop_ring() {
    pj_status_t status;

    if (!ringtone_port_info.ring_on) {
        printf("Ringtone port already disconnected\n");
        return PJ_SUCCESS;
    }

    //printf("Stopping ringtone\n");
    status = pjsua_conf_disconnect(ringtone_port_info.ring_slot, 0);
    ringtone_port_info.ring_on = 0;
    if (status != PJ_SUCCESS)
        pjsua_perror(THIS_FILE, "Error disconnecting ringtone port", status);
    return status;
}

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
	if (pjsua_call_get_count() > 1) {
		pjsua_call_answer(call_id, 486, NULL, NULL);
		return;
	}
	printf("Incoming call from %.*s\n", (int)ci.remote_info.slen, ci.remote_info.ptr);
	
	char who_is_calling[30];
	snprintf(who_is_calling, sizeof(who_is_calling), "%.*s", (int)ci.remote_info.slen, ci.remote_info.ptr);
	puts(who_is_calling);
	
	strcpy(who_is_calling,&who_is_calling[5]);
	puts(who_is_calling);
	int c = strchr(who_is_calling, '@') - who_is_calling;
	strncpy(who_is_calling, who_is_calling, c);
	who_is_calling[c] = '\0';
	char result_string[40] = "Входящий вызов: ";
	strcat(result_string, who_is_calling);
	
	gtk_label_set_text((GtkLabel*)call_label,result_string);
	gtk_widget_set_sensitive(answer_button, TRUE);
	start_ring();
	call_to_answer = call_id;
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

static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
	pjsua_call_info ci;

	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &ci);
 	   PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id, (int)ci.state_text.slen, ci.state_text.ptr));


	if (ci.state == PJSIP_INV_STATE_DISCONNECTED) { 
		stop_ring();
		gtk_widget_set_sensitive(answer_button, FALSE);
		gtk_label_set_text((GtkLabel*)call_label,"Нет звонков");	    	
	}
}

pj_status_t call_someone(char* server_ip,char* call_sip)
{
	pj_status_t status;
	pj_str_t uri=pj_str(call_sip);
	//printf("calling to %s\n",uri);
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
	return status;	
}


//INITIALISATION STUFF
void pj_initialisation()
{
	configurate_init_PJSUA();
	create_transport();
	start_PJSUA();
	configurate_account(server_ip,user_name,password);
	account_registration();
	init_ringtone_player();
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
	ua_cfg.cb.on_call_state = &on_call_state;
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

static void init_ringtone_player() {

    int file_slot;
    pj_pool_t *pool;
    pjmedia_port *file_port;
    pj_status_t status;

    pool = pjsua_pool_create("wav", 4000, 4000);

    status = pjmedia_wav_player_port_create(pool, "telephone.wav", 0, 0, 0, &file_port);

    if (status != PJ_SUCCESS) {
        pjsua_perror(THIS_FILE, "Error creating WAV player port", status);
        return;
    }

    status = pjsua_conf_add_port(pool, file_port, &file_slot);

    if (status != PJ_SUCCESS) {
        pjsua_perror(THIS_FILE, "Error adding port to conference", status);
        return;
    }

    ringtone_port_info = (ringtone_port_info_t) { .ring_on = 0, 
        .ring_slot = file_slot, .ring_port = file_port , .pool = pool };

}













