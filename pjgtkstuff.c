#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include <gtk/gtk.h>
#include <pjmedia.h>
#include "pjgtkstuff.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

typedef struct _ringtone_port_info {
    int ring_on;
    int ring_slot;
    pjmedia_port *ring_port;
    pj_pool_t *pool;
} ringtone_port_info_t;

static ringtone_port_info_t ringtone_port_info;
char server_ip[15];// = "192.168.56.102";
char user_name[20];// = "6000";
char password[30];// = "PASSWORD";
int tube_button_mode = 0;//0 for call 1 for accept 2 for nothing
int decline_button_mode = 1;//0 for decline 1 for nothing

GtkWidget *ip_label, *ip_entry;
GtkWidget *login_label, *login_entry;
GtkWidget *password_label, *password_entry;

GtkTextBuffer *chat_buff;

GtkWidget *sip_label, *sip_entry, *chat_window, *chat_view;
GtkWidget *call_label, *message_entry;
GtkWidget *ancall_button, *decline_button, *send_button, *add_button, *delete_button;
GtkWidget *list, *list_scroll;
GtkTreeSelection *selection; 

char message_cash[10000] = {};
char all_abonents[100][30] = {};
int number_of_abonents=0;

pjsua_acc_id acc_id;	
pjsua_call_id call_to_answer;
pj_status_t stop_ring();
#define THIS_FILE "Sip Client"

//MAIN INTERFACE STUFF

enum {
	LIST_ITEM = 0,
	N_COLUMNS
};

void init_list(GtkWidget *list) {

	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkListStore *store;

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes("List Items",
		  renderer, "text", LIST_ITEM, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(list), 
	    GTK_TREE_MODEL(store));

	g_object_unref(store);
}



void add_to_list(GtkWidget *list, const gchar *str) {
    
	GtkListStore *store;
	GtkTreeIter iter;

	store = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW(list)));
	
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, LIST_ITEM, str, -1);
}

void delete_button_clicked(GtkWidget *widget, gpointer selection) {
    
	GtkListStore *store;
	GtkTreeModel *model;
	GtkTreeIter  iter;

	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
 	model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

	if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
		return;
		
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) 
    		gtk_list_store_remove(store, &iter);

}

void check_and_add(char* abonent)
{	
	for(int i=0;i<number_of_abonents+1;i++)
	{	
		if(strcmp(all_abonents[i],abonent) == 0)
		{
			return;
		}
	}
	strcpy(all_abonents[number_of_abonents],abonent);
	number_of_abonents++;	
	add_to_list(list, abonent);	
}

void set_button_clicked(GtkWidget *button, gpointer data)
{
	sprintf(server_ip,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) ip_entry)));
	sprintf(user_name,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry)));
	sprintf(password,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry)));
	gtk_main_quit();
}

void design_message(char* user_name, char* message)
{
	strcat(message_cash,user_name);
	strcat(message_cash,": ");
	strcat(message_cash,message);
	strcat(message_cash,"\n");
	gtk_text_buffer_set_text(chat_buff, message_cash, -1);	
}

void send_button_clicked(GtkWidget *button, gpointer data)
{
	char message[100];
	char for_whom[50];
	sprintf(for_whom,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) sip_entry)));
	sprintf(message,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) message_entry)));
	pj_str_t who;
	if((strcmp(for_whom," ") == 0) ||((strcmp(message," ") == 0))) return;
	if((strcmp(for_whom,"") == 0) ||((strcmp(message,"") == 0))) return;
	
	if(strcmp(for_whom,user_name) != 0)
	{
		if(strstr(for_whom,"sip:") != NULL && strstr(for_whom,"@") != NULL)
		{
			//puts("here");
			char verify_cash[100];
			strcpy(verify_cash,&for_whom[4]);
			int c = strchr(verify_cash, '@') - verify_cash;
			strncpy(verify_cash, verify_cash, c);
			verify_cash[c] = '\0';
			if(strcmp(verify_cash,user_name) == 0)
				return;	
		}
	} else return;
	
	if (pjsua_verify_url(for_whom) != PJ_SUCCESS) 
	{
		char ct[100];
		snprintf(ct, sizeof(ct), "sip:%s@%s", for_whom, server_ip); 
		if (pjsua_verify_url(ct) != PJ_SUCCESS) 
		{
			printf("Invalid URL entered. Try again. \n");
		}
			else 
		{
			who = pj_str(ct);	
		}			
	} else who = pj_str(for_whom);
	pj_str_t text = pj_str(message);
	pjsua_im_send(acc_id, &who, NULL, &text, NULL, NULL);	
	
	design_message(user_name, message);
	
	gtk_entry_set_text(GTK_ENTRY((GtkWidget*) message_entry), "");
}

void add_button_clicked(GtkWidget *button, gpointer data)
{
	char sip_call[50];
	sprintf(sip_call,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) sip_entry)));
	check_and_add(sip_call);
}

void ancall_button_clicked(GtkWidget *button, gpointer data)
{	
	if(tube_button_mode == 0)
	{
		//puts("Can make call");
		char sip_call[50];
		char result_char[90]="Исходящий звонок: ";
		sprintf(sip_call,"%s",gtk_entry_get_text(GTK_ENTRY((GtkWidget*) sip_entry)));
		
		if(strcmp(sip_call," ") == 0) return;
		if(strcmp(sip_call,"") == 0) return;
		decline_button_mode = 0;
		//printf("%s\n",sip_call);
		if(strcmp(sip_call,user_name) != 0)
		{
			if(strstr(sip_call,"sip:") != NULL && strstr(sip_call,"@") != NULL)
			{
				//puts("here");
				char verify_cash[100];
				strcpy(verify_cash,&sip_call[4]);
				int c = strchr(verify_cash, '@') - verify_cash;
				strncpy(verify_cash, verify_cash, c);
				verify_cash[c] = '\0';
				strcat(result_char,verify_cash);
				if(strcmp(verify_cash,user_name) == 0)
					return;	
			}
			else strcat(result_char,sip_call);
		} else return;
		
		char ct[100];
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
				gtk_label_set_text((GtkLabel*)call_label,result_char);
				call_someone(server_ip, ct);	
				return;	
			}			
		} 
		gtk_label_set_text((GtkLabel*)call_label,result_char);
		call_someone(server_ip, sip_call);
	}
	if(tube_button_mode == 1)
	{
		//puts("Can accept call");
		stop_ring();
		pjsua_call_answer(call_to_answer, 200, NULL, NULL);
		tube_button_mode = 2;
	}
	if(tube_button_mode == 2)
	{
		//puts("Can do nothing");
	}
	return;
}

void decline_button_clicked(GtkWidget *button, gpointer data)
{
	if(decline_button_mode == 0)
	{
		//puts("Can do nothing");
		stop_ring();
		pjsua_call_hangup_all();
		decline_button_mode = 1;
	}
}

void del_button_clicked(GtkWidget *button, gpointer data)
{
	gtk_entry_set_text(GTK_ENTRY((GtkWidget*) ip_entry), "");
	gtk_entry_set_text(GTK_ENTRY((GtkWidget*) login_entry), "");
	gtk_entry_set_text(GTK_ENTRY((GtkWidget*) password_entry), "");
}

void pick_abonent(GtkWidget *widget) 
{ 
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *value;

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {
		gtk_tree_model_get(model, &iter, LIST_ITEM, &value,  -1);
		gtk_entry_set_text(GTK_ENTRY(sip_entry), value);
		g_free(value);
	}
}

void check_and_load_cache()
{
	gtk_init (NULL, NULL);
  	list = gtk_tree_view_new();
    	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);
    	init_list(list);
	struct stat st = {0};
	if (stat("cache", &st) == -1) {
		registration_interface();
    		return;
	}
	
	DIR *dir;
	FILE *fp;
	struct dirent *A;
	char *cash_file;
	char filepath[20] = {};
 	if ((dir=opendir("cache"))!=NULL)
 	{
	    	//while ((A=readdir(dir)) != NULL)
	    	//{
	      		//printf("%s\n", A->d_name);
	    	//}
		A=readdir(dir);
		cash_file = A->d_name;
		if(strcmp(cash_file,"..") == 0)
		{
			registration_interface();
    			return;
		}
     		closedir(dir);
 	}
 	strcat(filepath,"cache/");
	strcat(filepath,cash_file);
 	if ((fp = fopen(filepath, "r")) == NULL)
  	{
    		printf("Cache load failed");
    		registration_interface();
    		return;
  	}
  	
  	if(fscanf(fp,"%s%s%s%d",server_ip,user_name,password,&number_of_abonents)!=4)
  	{
  		registration_interface();
    		return;	
  	}
  	//fscanf(fp,"%s",user_name);
  	//fscanf(fp,"%s",password);
  	//fscanf(fp,"%d",&number_of_abonents);
  	
  	for(int i=0;i<number_of_abonents;i++)
  	{
  		if(fscanf(fp,"%s",all_abonents[i])!=1)
  		{
  			return;
  		}
  		add_to_list(list, all_abonents[i]);
  	}
  	while(1)
  	{
  		char message[100]={};
  		char name[20]={};
  		
  		if(fscanf(fp,"%s%s",name,message)!=2)
  		{
  			registration_interface();
    			return;
  		}
  		if(strcmp(name,"#") == 0 || strcmp(message,"#") == 0)
  			break;
  		strcat(message_cash,name);
		strcat(message_cash," ");
		strcat(message_cash,message);
		strcat(message_cash,"\n");
		gtk_text_buffer_set_text(chat_buff, message_cash, -1);
  	}
  	gtk_text_buffer_set_text(chat_buff, message_cash, -1);
  	//fscanf(fp,"#");
  	fclose(fp);
	
}

void save_cache()
{
	//char message_cash[10000];
	//char all_abonents[100][30];
	//int number_of_abonents=0;
	//char server_ip[15] = "192.168.56.102";
	//char user_name[20] = "6000";
	//char password[30] = "PASSWORD";
	struct stat st = {0};
	if (stat("cache", &st) == -1) {
    		mkdir("cache", 0700);
	}
	FILE *fp;
	char filepath[20] = {};
	strcat(filepath,"cache/");
	strcat(filepath,user_name);
	if ((fp = fopen(filepath, "wb+")) == NULL)
  	{
    		printf("Cache save failed");
    		return;
  	}
  	fprintf(fp,"%s\n",server_ip);
  	fprintf(fp,"%s\n",user_name);
  	fprintf(fp,"%s\n",password);
  	fprintf(fp,"%d\n",number_of_abonents);
  	for(int i=0;i<number_of_abonents;i++)
  		fprintf(fp,"%s\n",all_abonents[i]);
  	fprintf(fp,"EB");
  	fprintf(fp,"%s",message_cash);
  	fprintf(fp,"# @");
  	fclose(fp);
}

void close_prog()
{
	exit(0);
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
    gtk_entry_set_invisible_char(GTK_ENTRY(password_entry), '*');
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
    GtkWidget *scrolls;
    gtk_init (NULL, NULL);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    GtkImage *green,*red,*send;
    green = (GtkImage *)gtk_image_new_from_file("resources/green.jpg");
    red = (GtkImage *)gtk_image_new_from_file("resources/red.jpg");	
    send = (GtkImage *)gtk_image_new_from_file("resources/send.jpg");	
    GtkSettings *default_settings = gtk_settings_get_default();
    g_object_set(default_settings, "gtk-button-images", TRUE, NULL);
    
    gtk_window_set_title (GTK_WINDOW (window), "SIPHONE");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    
    // Создаем ярлык и поле ввода логина
    sip_label = gtk_label_new("SIP-адрес:");
    call_label = gtk_label_new("Нет звонков");
    gtk_widget_set_size_request(list, 180, 200);
    
    list_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(list_scroll, 200, 200);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
    g_signal_connect(selection, "changed", G_CALLBACK(pick_abonent), sip_entry);
    
    sip_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(sip_entry),"Введите абонента...");
    message_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_entry),"Напишите сообщение...");
    //GIcon *icon= g_file_icon_new(g_file_new_for_path("resources/send.jpg")); 
    //gtk_entry_set_icon_from_gicon(GTK_ENTRY(message_entry),GTK_ENTRY_ICON_SECONDARY, icon);
    
    // Создаем кнопки
    ancall_button = gtk_button_new_with_label("");
    gtk_button_set_image (GTK_BUTTON(ancall_button),(GtkWidget *)green); 
    //gtk_button_set_relief(GTK_BUTTON(ancall_button),GTK_RELIEF_NONE);
    decline_button = gtk_button_new_with_label("");
    gtk_button_set_image (GTK_BUTTON(decline_button),(GtkWidget *)red); 
    send_button = gtk_button_new_with_label("");
    gtk_button_set_image (GTK_BUTTON(send_button),(GtkWidget *)send); 
    add_button = gtk_button_new_with_label("Добавить");
    delete_button = gtk_button_new_with_label("Удалить");
    
    //создаем окно чата
    chat_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view),FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(chat_view), FALSE);
    scrolls = gtk_scrolled_window_new(NULL, NULL); 
    gtk_widget_set_size_request(scrolls, 288, 180);
    chat_buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
    gtk_container_add(GTK_CONTAINER(scrolls), chat_view);
    
    
    
    // Задаем функции кнопок
    g_signal_connect(GTK_BUTTON(ancall_button), "clicked", G_CALLBACK(ancall_button_clicked), login_entry);
    g_signal_connect(GTK_BUTTON(decline_button), "clicked", G_CALLBACK(decline_button_clicked), password_entry);
    g_signal_connect(GTK_BUTTON(send_button), "clicked", G_CALLBACK(send_button_clicked), password_entry);
     g_signal_connect(GTK_BUTTON(delete_button), "clicked", G_CALLBACK(delete_button_clicked), selection);
    g_signal_connect(GTK_BUTTON(add_button), "clicked", G_CALLBACK(add_button_clicked), password_entry);

    GtkWidget *hbox0, *hbox1, *hbox2, *hbox3;
    GtkWidget *vbox;
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    hbox0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox0), sip_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox0), sip_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox0), ancall_button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox0), decline_button, TRUE, FALSE, 5);
    
    GtkWidget *hbox4;
    hbox4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER (list_scroll),list);
   // gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_scroll),list);
    gtk_box_pack_start(GTK_BOX(hbox4), list_scroll, TRUE, FALSE, 5);
    //gtk_box_pack_start(GTK_BOX(hbox4), list, TRUE, FALSE, 0);
    
    GtkWidget *hbox40;
    hbox40 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox40), delete_button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox40), add_button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox4), hbox40, TRUE, FALSE, 0);
    //gtk_box_pack_start(GTK_BOX(hbox4), call_label, TRUE, FALSE, 5); think about it later
    
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), hbox4, TRUE, FALSE, 5);
     
    hbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), hbox0, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), scrolls, TRUE, FALSE, 0);
    hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox3), message_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox3), send_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), hbox3, TRUE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(hbox1), hbox2, TRUE, FALSE, 5);
    
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, FALSE, 5);
    
    
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_text_buffer_set_text(chat_buff, message_cash, -1);
    gtk_widget_show_all(window);
    gtk_widget_set_size_request(message_entry, gtk_widget_get_allocated_width(scrolls)-gtk_widget_get_allocated_width(send_button), 20);
    gtk_main();
    gtk_widget_destroy(window);
    pjsua_destroy(); 
    save_cache(); 
    exit(0);
}


//MAIN FUNCTIONALITY STUFF
static void on_pager(pjsua_call_id call_id, const pj_str_t *from, const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type, const pj_str_t *body)
{
	//printf("MESSAGE from %.*s: %.*s\n", (int)from->slen, from->ptr, (int)body->slen, body->ptr);	
	char message[100], who[50];
	
	sprintf(who, "%.*s",(int)from->slen, from->ptr);
	strcpy(who,&who[16]);
	int c = strchr(who, '@') - who;
	strncpy(who, who, c);
	who[c] = '\0';
	sprintf(message, "%.*s",(int)body->slen, body->ptr);
	
	design_message(who,message);
	
	return;
}


/*static void on_pager_status(pjsua_call_id call_id, const pj_str_t *to, const pj_str_t *body, void *user_data, pjsip_status_code status, const pj_str_t *reason)
{
	puts("2 recieved");
	printf("2 recieved\n");
}*/


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
	//puts(who_is_calling);
	
	strcpy(who_is_calling,&who_is_calling[5]);
	//puts(who_is_calling);
	int c = strchr(who_is_calling, '@') - who_is_calling;
	strncpy(who_is_calling, who_is_calling, c);
	who_is_calling[c] = '\0';
	char result_string[40] = "Входящий вызов: ";
	strcat(result_string, who_is_calling);
	
	gtk_label_set_text((GtkLabel*)call_label,result_string);
	tube_button_mode = 1;
	decline_button_mode = 0;
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
 	
 	char who_is_calling[30];
 	snprintf(who_is_calling, sizeof(who_is_calling), "%.*s", (int)ci.remote_info.slen, ci.remote_info.ptr);
 	int b = strchr(who_is_calling, ':') - who_is_calling + 1;
 	strcpy(who_is_calling,&who_is_calling[b]);
	//puts(who_is_calling);
	int c = strchr(who_is_calling, '@') - who_is_calling;
	strncpy(who_is_calling, who_is_calling, c);
	who_is_calling[c] = '\0';
	char result_string[40] = "Звонок: ";
	strcat(result_string, who_is_calling);
	
	if (ci.state == PJSIP_INV_STATE_CONFIRMED) { 
		stop_ring();
		tube_button_mode = 2;
		gtk_label_set_text((GtkLabel*)call_label,result_string);	    	
	}

	if (ci.state == PJSIP_INV_STATE_DISCONNECTED) { 
		stop_ring();
		tube_button_mode = 0;
		decline_button_mode = 1;
		gtk_label_set_text((GtkLabel*)call_label,"Нет звонков");    	
	}
}

pj_status_t call_someone(char* server_ip,char* call_sip)
{
	pj_status_t status;
	printf("Calling to %s\n",call_sip);
	pj_str_t uri=pj_str(call_sip);
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
	tube_button_mode = 2;
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
	
	ua_cfg.cb.on_pager = &on_pager;
	//ua_cfg.cb.on_pager_status = &on_pager_status;
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

    status = pjmedia_wav_player_port_create(pool, "resources/telephone.wav", 0, 0, 0, &file_port);

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













