#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <dirent.h>

/* Variables */
GtkWidget *menu, *submenu;
GtkWidget *item_quit, *item_interfaces, *item_list, *item_current_interface;
AppIndicator *indicator;
std::string tx_new, tx_old, rx_new, rx_old;
std::string interface = "lo";
std::string tx_path = "/sys/class/net/lo/statistics/tx_bytes";
std::string rx_path = "/sys/class/net/lo/statistics/rx_bytes";
std::string homedir = getenv("HOME");
std::string config_path = homedir + "/.config/indicator-net.conf";
struct dirent *dir;
DIR *path;

/* Functions */
static gboolean update(gpointer)
{
	std::ifstream file_tx, file_rx;
	file_tx.open(tx_path);
	file_rx.open(rx_path);
	//std::cout << tx_path << std::endl;
	getline(file_tx, tx_new);
	getline(file_rx, rx_new);
	std::string out = "\u2193" + std::to_string((atoi(rx_new.c_str()) - atoi(rx_old.c_str())) / 1024) + " kB/s " + "\u2191" + std::to_string((atoi(tx_new.c_str()) - atoi(tx_old.c_str())) / 1024) + " kB/s";
	app_indicator_set_label(indicator, out.c_str(), NULL);
	tx_old = tx_new;
	rx_old = rx_new;
	file_tx.close();
	file_rx.close();
	return 1;
}

static void change_interface(GtkMenuItem *item)
{
	interface = gtk_menu_item_get_label(item);
	//std::cout << interface << std::endl;
	tx_path = "/sys/class/net/" + interface + "/statistics/tx_bytes";
	rx_path = "/sys/class/net/" + interface + "/statistics/rx_bytes";
	std::ofstream config;
	config.open(config_path);
	config << interface;
	config.close();
	gtk_menu_item_set_label(GTK_MENU_ITEM(item_current_interface), interface.c_str());
}

static void get_interfaces()
{
	if ((path = opendir("/sys/class/net/")))
	{
		while ((dir = readdir(path)))
		{
			std::string temp_interface = dir->d_name;
			if (temp_interface[0] != '.')
			{
				item_list = gtk_menu_item_new_with_label(temp_interface.c_str());
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item_list);
				g_signal_connect(G_OBJECT(item_list), "activate", G_CALLBACK(change_interface), item_list);
			}
		}
		closedir(path);
	}
}

/* Main */
int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	std::ifstream config;
	config.open(config_path);
	if (config.is_open())
	{
		getline(config, interface);
		tx_path = "/sys/class/net/" + interface + "/statistics/tx_bytes";
		rx_path = "/sys/class/net/" + interface + "/statistics/rx_bytes";
		config.close();
	}

	menu = gtk_menu_new();
	submenu = gtk_menu_new();

	item_current_interface = gtk_menu_item_new_with_label(interface.c_str());
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_current_interface);
	gtk_widget_set_sensitive(item_current_interface, FALSE);

	item_interfaces = gtk_menu_item_new_with_label("Interfaces");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_interfaces);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_interfaces), submenu);

	item_quit = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_quit);
	g_signal_connect(item_quit, "activate", gtk_main_quit, NULL);

	get_interfaces();

	indicator = app_indicator_new("indicator-net", "network-transmit-receive", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_menu(indicator, GTK_MENU(menu));
	app_indicator_set_title(indicator, "NET Indicator");

	g_timeout_add_seconds(1, update, NULL);

	gtk_widget_show_all(menu);
	gtk_main();
	return 0;
}
