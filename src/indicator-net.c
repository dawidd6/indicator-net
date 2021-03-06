#include <libappindicator/app-indicator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

/* Vars */
AppIndicator *indicator;
GtkWidget *menu, *submenu;
GtkWidget *item_refresh, *item_quit, *item_list, *item_interfaces, *item_current_interface, *item_icon;
double tx_new, tx_old, rx_new, rx_old, tx, rx;
char tx_path[50];
char rx_path[50];
char output[50];
char format[30];
char interface[10];
char icon[50];
char icons[2][50];
char config_path[50];
FILE *file_tx;
FILE *file_rx;

/* Functions' declarations */
void interface_change(GtkMenuItem *item);
void interface_get();
void icon_change();
static inline gboolean update(gpointer ptr);
void config_handle(const char *mode);

/* Main */
int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    sprintf(config_path, "%s/.config/indicator-net.conf", getenv("HOME"));
    strcpy(interface, "lo");
    strcpy(icons[0], "network-transmit-receive");
    strcpy(icons[1], "network-transmit-receive-symbolic");
    strcpy(icon, icons[0]);
    sprintf(tx_path, "/sys/class/net/%s/statistics/tx_bytes", interface);
    sprintf(rx_path, "/sys/class/net/%s/statistics/rx_bytes", interface);
    file_tx = fopen(tx_path, "r");
    file_rx = fopen(rx_path, "r");
    config_handle("r");

    menu = gtk_menu_new();
    indicator = app_indicator_new("indicator-net", icon, APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_menu(indicator, GTK_MENU(menu));
    app_indicator_set_title(indicator, "NET Indicator");

    item_current_interface = gtk_menu_item_new_with_label(interface);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_current_interface);
    gtk_widget_set_sensitive(item_current_interface, FALSE);

    item_refresh = gtk_menu_item_new_with_label("Refresh");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_refresh);
    g_signal_connect(item_refresh, "activate", G_CALLBACK(interface_get), NULL);

    item_icon = gtk_menu_item_new_with_label("Change icon");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_icon);
    g_signal_connect(item_icon, "activate", G_CALLBACK(icon_change), NULL);

    item_quit = gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_quit);
    g_signal_connect(item_quit, "activate", gtk_main_quit, NULL);

    interface_get();

    g_timeout_add_seconds(1, update, NULL);
    gtk_widget_show_all(menu);
    gtk_main();
    fclose(file_tx);
    fclose(file_rx);
    return 0;
}

/* Functions' definitions */
gboolean update(gpointer ptr)
{
    fscanf(file_tx, "%lf", &tx_new);
    fscanf(file_rx, "%lf", &rx_new);
    tx = tx_new - tx_old;
    rx = rx_new - rx_old;

    if(rx > 1048576)
    {
        rx = rx / (1024 * 1024);
        strcpy(format, "\u2193 %.2lf MB/s ");
    }
    else if(rx > 1024)
    {
        rx = rx / 1024;
        strcpy(format, "\u2193 %.2lf kB/s ");
    }
    else
        strcpy(format, "\u2193 0 kB/s ");

    if(tx > 1048576)
    {
        tx = tx / (1024 * 1024);
        strcat(format, "\u2191 %.2lf MB/s");
    }
    else if(tx > 1024)
    {
        tx = tx / 1024;
        strcat(format, "\u2191 %.2lf kB/s");
    }
    else
        strcat(format, "\u2191 0 kB/s");

    sprintf(output, format, rx , tx);
    app_indicator_set_label(indicator, output, NULL);
    tx_old = tx_new;
    rx_old = rx_new;
    fflush(file_tx);
    fflush(file_rx);
    rewind(file_tx);
    rewind(file_rx);
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
    struct dirent *dir;
    DIR *path;

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

void icon_change()
{
    if(strcmp(app_indicator_get_icon(indicator), icons[0]) == 0)
        strcpy(icon, icons[1]);
    else
        strcpy(icon, icons[0]);

    app_indicator_set_icon(indicator, icon);
    config_handle("w");
}

void config_handle(const char *mode)
{
    FILE *config = fopen(config_path, mode);
    if(config)
    {
        if(strcmp(mode, "r") == 0)
            fscanf(config, "%s\n", interface);
        else if(strcmp(mode, "w") == 0)
            fprintf(config, "%s\n", interface);

        if(strcmp(mode, "r") == 0)
            fscanf(config, "%s\n", icon);
        else if(strcmp(mode, "w") == 0)
            fprintf(config, "%s\n", icon);

        sprintf(tx_path, "/sys/class/net/%s/statistics/tx_bytes", interface);
        sprintf(rx_path, "/sys/class/net/%s/statistics/rx_bytes", interface);
        freopen(tx_path, "r", file_tx);
        freopen(rx_path, "r", file_rx);
        fclose(config);
    }
}
