#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

/* Variables */
GtkWidget *menu;
GtkWidget *item_quit;
AppIndicator *indicator;
std::string tx_new, tx_old, rx_new, rx_old;
std::string interface = "wlp3s0";
std::string tx_path = "/sys/class/net/" + interface + "/statistics/tx_bytes";
std::string rx_path = "/sys/class/net/" + interface + "/statistics/rx_bytes";

/* Functions */
static gboolean update(gpointer)
{
	std::fstream file_tx, file_rx;
	file_tx.open(tx_path, std::fstream::in);
	file_rx.open(rx_path, std::fstream::in);
	getline(file_tx, tx_new);
	getline(file_rx, rx_new);
	std::string out_tx = "\u2191" + std::to_string((atoi(tx_new.c_str()) - atoi(tx_old.c_str())) / 1024) + " kB/s";
	std::string out_rx = "\u2193" + std::to_string((atoi(rx_new.c_str()) - atoi(rx_old.c_str())) / 1024) + " kB/s ";
	std::string bundle = out_rx + out_tx;
	app_indicator_set_label(indicator, bundle.c_str(), NULL);
	tx_old = tx_new;
	rx_old = rx_new; 
	file_tx.close();
	file_rx.close();
	return 1;
}

/* Main */
int main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	menu = gtk_menu_new();

	item_quit = gtk_menu_item_new_with_label("Quit");
        gtk_menu_shell_append (GTK_MENU_SHELL(menu), item_quit);
	g_signal_connect(item_quit, "activate", gtk_main_quit, NULL);
	
	indicator = app_indicator_new ("indicator-net", "network-transmit-receive", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_menu (indicator, GTK_MENU(menu));
	app_indicator_set_title (indicator, "NET Indicator");

	g_timeout_add_seconds(1, update, NULL);

	gtk_widget_show_all(menu);
	gtk_main();
	return 0;
}

