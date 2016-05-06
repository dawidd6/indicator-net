#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N 50

/* Variables */
GtkWidget *menu;
GtkWidget *item_quit;
AppIndicator *indicator;
unsigned int tx_old, tx_new, rx_old, rx_new;

/* Functions */
static gboolean update(gpointer arg)
{
	FILE *file_tx = fopen("/sys/class/net/wlp3s0/statistics/tx_bytes", "r");
	FILE *file_rx = fopen("/sys/class/net/wlp3s0/statistics/rx_bytes", "r");
	char tx_out[N] = "\u2191", rx_out[N] = "\u2193", tx_temp[N], rx_temp[N];

	fscanf(file_tx, "%i", &tx_new);
	fscanf(file_rx, "%i", &rx_new);

	unsigned int tx = (tx_new - tx_old) / 1024;
	unsigned int rx = (rx_new - rx_old) / 1024;

	sprintf(tx_temp, "%i", tx);
	sprintf(rx_temp, "%i", rx);

	strcat(tx_out, tx_temp);
	strcat(rx_out, rx_temp);
	strcat(tx_out, " kB/s ");
	strcat(rx_out, " kB/s ");

	app_indicator_set_label(indicator, strcat(rx_out, tx_out), NULL);

	tx_old = tx_new;
	rx_old = rx_new;
	fclose(file_tx);
	fclose(file_rx);
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

