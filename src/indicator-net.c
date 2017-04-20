#include <libappindicator/app-indicator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_NORM "\x1B[0m"

/* Vars */
AppIndicator *indicator;
GtkWidget *menu, *submenu;
GtkWidget *item_refresh, *item_quit, *item_list, *item_interfaces, *item_current_interface;
long int tx_new, tx_old, rx_new, rx_old;
char tx_path[50];
char rx_path[50];
char output[50];
char interface[10];
char config_path[50];
FILE *file_tx;
FILE *file_rx;
struct dirent *dir;
DIR *path;

/* Functions' declarations */
void interface_change(GtkMenuItem *item);
void interface_get();
static inline gboolean update(gpointer ptr);
void config_handle(const char *mode);
void logger(unsigned int count, ...);

/* Main */
int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	sprintf(config_path, "%s/.config/indicator-net.conf", getenv("HOME"));
	strcpy(interface, "lo");
	logger(5, COLOR_BLUE, "CONFIG: ", COLOR_NORM, "initialized = ", config_path);

	config_handle("r");

	sprintf(tx_path, "/sys/class/net/%s/statistics/tx_bytes", interface);
	sprintf(rx_path, "/sys/class/net/%s/statistics/rx_bytes", interface);
	menu = gtk_menu_new();
	indicator = app_indicator_new("indicator-net", "network-transmit-receive", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_menu(indicator, GTK_MENU(menu));
	app_indicator_set_title(indicator, "NET Indicator");

	item_current_interface = gtk_menu_item_new_with_label(interface);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_current_interface);
	gtk_widget_set_sensitive(item_current_interface, FALSE);

	item_refresh = gtk_menu_item_new_with_label("Refresh");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_refresh);
	g_signal_connect(item_refresh, "activate", G_CALLBACK(interface_get), NULL);

	item_quit = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_quit);
	g_signal_connect(item_quit, "activate", gtk_main_quit, NULL);

	interface_get();

	g_timeout_add_seconds(1, update, NULL);
	gtk_widget_show_all(menu);
	gtk_main();
	return 0;
}

/* Functions' definitions */
gboolean update(gpointer ptr)
{
	file_tx = fopen(tx_path, "r");
	file_rx = fopen(rx_path, "r");
	fscanf(file_tx, "%ld", &tx_new);
	fscanf(file_rx, "%ld", &rx_new);
	sprintf(output, "\u2193 %ld kB/s \u2191 %ld kB/s", (rx_new - rx_old) / 1024, (tx_new - tx_old) / 1024);
	app_indicator_set_label(indicator, output, NULL);
	tx_old = tx_new;
	rx_old = rx_new;
	fclose(file_tx);
	fclose(file_rx);
	return 1;
}

void interface_change(GtkMenuItem *item)
{
	strcpy(interface, gtk_menu_item_get_label(item));
	config_handle("w");
	gtk_menu_item_set_label(GTK_MENU_ITEM(item_current_interface), interface);
}

void interface_get()
{
	if(GTK_IS_WIDGET(item_interfaces))
		gtk_widget_destroy(item_interfaces);
	item_interfaces = gtk_menu_item_new_with_label("Interfaces");
	gtk_menu_shell_insert(GTK_MENU_SHELL(menu), item_interfaces, 1);

	if(GTK_IS_WIDGET(submenu))
		gtk_widget_destroy(submenu);
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_interfaces), submenu);

	if ((path = opendir("/sys/class/net/")))
	{
		while ((dir = readdir(path)))
		{
			if (dir->d_name[0] != '.')
			{
				item_list = gtk_menu_item_new_with_label(dir->d_name);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item_list);
				g_signal_connect(G_OBJECT(item_list), "activate", G_CALLBACK(interface_change), item_list);
			}
		}
		closedir(path);
	}
	gtk_widget_show_all(menu);
}

void config_handle(const char *mode)
{
	FILE *config = fopen(config_path, mode);
	if(config)
	{
		if(strcmp(mode, "r") == 0)
		{
			fscanf(config, "%s", interface);
			sprintf(tx_path, "/sys/class/net/%s/statistics/tx_bytes", interface);
			sprintf(rx_path, "/sys/class/net/%s/statistics/rx_bytes", interface);
			logger(5, COLOR_BLUE, "CONFIG: ", COLOR_NORM, "loaded = ", interface);
		}
		else if(strcmp(mode, "w") == 0)
		{
			sprintf(tx_path, "/sys/class/net/%s/statistics/tx_bytes", interface);
			sprintf(rx_path, "/sys/class/net/%s/statistics/rx_bytes", interface);
			fprintf(config, "%s", interface);
			logger(5, COLOR_BLUE, "CONFIG: ", COLOR_NORM, "written = ", interface);
		}
		fclose(config);
	}
}

void logger(unsigned int count, ...)
{
	time_t mytime = time(0);
	va_list vl;
	printf("%s[%s]%s ", COLOR_GREEN, strtok(ctime(&mytime), "\n"), COLOR_NORM);

	va_start(vl, count);
	for(unsigned int i = 0; i < count; i++)
	{
		printf("%s", va_arg(vl, const char *));
	}
	va_end(vl);

	printf("\n");
}
