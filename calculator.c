#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 64

typedef struct {
    GtkWidget *label;
    double result;
    char buff[BUF_SIZE];
    char display[BUF_SIZE];
    char current_op;
    gboolean has_result;
} calc_state;

static void number_clicked(GtkWidget *button, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    const char *digit = gtk_button_get_label(GTK_BUTTON(button));
    if(strlen(st->display) + strlen(digit) < BUF_SIZE) {
        if(strcmp(digit, ".") == 0) {
            if(strchr(st->buff, '.') != NULL) {
                return;
            }
            if(strlen(st->buff) == 0) {
                strcat(st->buff, "0");
                strcat(st->display, "0");
            }
        }
    }
    if(strlen(st->display) + strlen(digit) < BUF_SIZE &&
        strlen(st->display) + strlen(digit) < BUF_SIZE) {
        strcat(st->display, digit);
        strcat(st->buff, digit);
        gtk_label_set_text(GTK_LABEL(st->label), st->display);
    }
}

static void operation_clicked(GtkWidget *button, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    const char *user_operation = gtk_button_get_label(GTK_BUTTON(button));
    if(strlen(st->buff) == 0) {
        return;
    }
    double value = (strlen(st->buff) > 0) ? atof(st->buff) : 0;
    if(!st->has_result) {
        st->result = value;
        st->has_result = TRUE;
    } else {
        switch(st->current_op) {
            case '+': st->result += value; break;
            case '-': st->result -= value; break;
            case '*': st->result *= value; break;
            case '/': if(value != 0) st->result /= value; break;
        }
    }
    st->current_op = *user_operation;
    st->buff[0] = '\0';
    strcat(st->display, user_operation);
    gtk_label_set_text(GTK_LABEL(st->label), st->display); 
}

// backend
static void equals_clicked(GtkWidget *button, gpointer user_data) { 
    calc_state *st = (calc_state *)user_data;
    double value = (strlen(st->buff) > 0) ? atof(st->buff) : 0;
    switch(st->current_op) {
        case '+': { st->result += value; break; }
        case '-': { st->result -= value; break; }
        case '*': { st->result *= value; break; }
        case '/': { if(value != 0) st->result /= value; break; }
    }
    snprintf(st->buff, BUF_SIZE, "%g", st->result);
    snprintf(st->display, BUF_SIZE, "%g", st->result);
    gtk_label_set_text(GTK_LABEL(st->label), st->buff);
    st->current_op = 0;
    st->has_result = FALSE;
}

// remove the most recent character in the buffer
static void backspace_clicked(GtkWidget *button, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    int len_display = strlen(st->display);
    if(len_display > 0) {
        char removed = st->display[len_display - 1];
        st->display[len_display-1] = '\0';
        if(removed == '+' || removed == '-' || removed == '*' || removed == '/') {
            st->current_op = '\0';
        } else {
            int len_buff = strlen(st->buff);
            if(len_buff > 0) {
                st->buff[len_buff - 1] = '\0';
            }
        }
        gtk_label_set_text(GTK_LABEL(st->label), st->display);
    }
}
static void clear_clicked(GtkWidget *widget, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    st->buff[0] = '\0';
    st->display[0] = '\0';
    st->result = 0;
    st->current_op = 0;
    st->has_result = FALSE;
    gtk_label_set_text(GTK_LABEL(st->label), st->buff);
}

static GtkWidget *create_button(const char *label, GCallback callback,
                                gpointer user_data, GtkWidget *grid, 
                                int col, int row,
                                int size_x, int size_y) {
    GtkWidget *button = gtk_button_new_with_label(label);
    g_signal_connect(button, "clicked", callback, user_data);
    gtk_grid_attach(GTK_GRID(grid), button, col, row, size_x, size_y);
    gtk_widget_set_hexpand(button, TRUE);
    gtk_widget_set_vexpand(button, TRUE);
    return button;
}

static void resize_label(GtkWidget *widget, GdkRectangle *allocation,
                         gpointer user_data) {
    GtkWidget *label = GTK_WIDGET(user_data);
    int new_size = allocation->height / 20;
    char css[128];
    snprintf(css, sizeof(css), 
             "label { font-size: %dpx; }" 
             "button {font-size: %dpx; }",
             new_size, 
             new_size);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(provider), 
        GTK_STYLE_PROVIDER_PRIORITY_USER);
}

static void activate (GtkApplication *app,
                      gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "karim's calculator");
  gtk_window_set_default_size(GTK_WINDOW(window), 250, 250);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  char *label_text[BUF_SIZE];

  grid = gtk_grid_new();
  gtk_container_add (GTK_CONTAINER (window), grid);
  
  GtkWidget *label = gtk_label_new("Welcome! :)");
  
  gtk_widget_set_hexpand(label, TRUE);
  gtk_widget_set_vexpand(label, TRUE);

  g_signal_connect(window, "size-allocate", G_CALLBACK(resize_label), label);

  calc_state *st = g_malloc(sizeof(calc_state));
  st->label = label;
  st->buff[0] = '\0';
  st->display[0] = '\0';
  st->result = 0;
  st->current_op = 0;
  st->has_result = FALSE;
    
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 4, 1);
  create_button("CLEAR", G_CALLBACK(clear_clicked), st, grid, 0, 1, 2, 1);
  create_button("<-", G_CALLBACK(backspace_clicked), st, grid, 2, 1, 1, 1);
  create_button("/", G_CALLBACK(operation_clicked), st, grid, 3, 1, 1, 1);  
  create_button("1", G_CALLBACK(number_clicked), st, grid, 0, 2, 1, 1);
  create_button("2", G_CALLBACK(number_clicked), st, grid, 1, 2, 1, 1);
  create_button("3", G_CALLBACK(number_clicked), st, grid, 2, 2, 1, 1);
  create_button("*", G_CALLBACK(operation_clicked), st, grid, 3, 2, 1, 1);
  create_button("4", G_CALLBACK(number_clicked), st, grid, 0, 3, 1, 1);
  create_button("5", G_CALLBACK(number_clicked), st, grid, 1, 3, 1, 1);
  create_button("6", G_CALLBACK(number_clicked), st, grid, 2, 3, 1, 1); 
  create_button("-", G_CALLBACK(operation_clicked), st, grid, 3, 3, 1, 1);
  create_button("7", G_CALLBACK(number_clicked), st, grid, 0, 4, 1, 1);
  create_button("8", G_CALLBACK(number_clicked), st, grid, 1, 4, 1, 1);
  create_button("9", G_CALLBACK(number_clicked), st, grid, 2, 4, 1, 1);
  create_button("+", G_CALLBACK(operation_clicked), st, grid, 3, 4, 1, 1);
  create_button("0", G_CALLBACK(number_clicked), st, grid, 0, 5, 2, 1);
  create_button(".", G_CALLBACK(number_clicked), st, grid, 2, 5, 1, 1);
  create_button("=", G_CALLBACK(equals_clicked), st, grid, 3, 5, 1, 1);
  gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
