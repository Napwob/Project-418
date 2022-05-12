#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <pjsua-lib/pjsua.h>
#include <ncurses.h>
#include "pjstuff.h"
#include <gtk/gtk.h>

#define THIS_FILE "Sip Client"

GtkWidget *ip_label, *ip_entry;
GtkWidget *login_label, *login_entry;
GtkWidget *password_label, *password_entry;
GtkWidget *sip_label, *sip_entry;
char server_ip[15];
char user_name[20];
char password[30];

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
   // pjsua_call_answer(call_id, 200, NULL, NULL);
    //gtk_widget_set_sensitive(answer_button, FALSE);
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
    ip_label = gtk_label_new("Введите IP:");
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
    GtkWidget *call_button, *answer_button;

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
}	

int main()
{
	//printf("Server IP: ");
	//scanf("%s",server_ip);
	//printf("User name: ");
	//scanf("%s",user_name);
	//printf("%s's password: ",user_name);
	//scanf("%s",password);
	registration_interface();
	printf("Server IP: %s\n",server_ip);
	printf("User name: %s\n", user_name);
	printf("%s's password: %s\n",user_name,password);
	
	configurate_init_PJSUA();
	create_transport();
	start_PJSUA();
	configurate_account(server_ip,user_name,password);
	account_registration();
	main_interface();
	
        sleep(30);
	pjsua_destroy();  
	endwin();
	return 0;	
}
