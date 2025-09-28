#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 64

typedef struct {
    GtkWidget *label;
    int a;
    int b;
    int result;
    char operation;
    gboolean entering_a;
    char buff[BUF_SIZE];
    char display[BUF_SIZE];
} calc_state;

// TO-DO: backend



static void number_clicked(GtkWidget *button, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    const char *digit = gtk_button_get_label(GTK_BUTTON(button));
    if(strlen(st->display) + strlen(digit) < BUF_SIZE) {
        strcat(st->display, digit);
        strcat(st->buff, digit);
        gtk_label_set_text(GTK_LABEL(st->label), st->display);
        if(st->entering_a) {
             st->a = atoi(st->buff);
         } else {
            st->b = atoi(st->buff);
         }
    }
}

static void operation_clicked(GtkWidget *button, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    const char *user_operation = gtk_button_get_label(GTK_BUTTON(button));
    if(strlen(st->display) + strlen(user_operation) < BUF_SIZE) {
        if(st->entering_a == FALSE) {
            char error[] = "Only one operation allowed.";
            gtk_label_set_text(GTK_LABEL(st->label), error);
        } else {
             if(st->a == 0) {
                const char *empty = "0";
                strcat(st->display, empty);
             }
             st->operation = *user_operation;
             st->buff[0] = '\0';
             strcat(st->display, user_operation);
             gtk_label_set_text(GTK_LABEL(st->label), st->display);
             st->entering_a = FALSE;
             st->operation = *user_operation;
        }
    }
}

// backend
static void equals_clicked(GtkWidget *button, gpointer user_data) { 
    calc_state *st = (calc_state *)user_data;
    switch(st->operation) {
        case '+': { st->result = st->a + st->b; break; }
        case '-': { st->result = st->a - st->b; break; }
        case '*': { st->result = st->a * st->b; break; }
        case '/': { st->result = st->a / st->b; break; }
        default :
            st->result = -1;
    }
    st->entering_a = TRUE;
    sprintf(st->buff, "%d", st->result);
    sprintf(st->display, "%d", st->result);
    gtk_label_set_text(GTK_LABEL(st->label), st->buff);
    st->a = st->result; 
    st->b = 0;
    st->operation = 0;
}

// remove the most recent character in the buffer
static void backspace_clicked(GtkWidget *button, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    int len_display = strlen(st->display);
    if(len_display > 0) {
        st->display[len_display-1] = '\0';
    }
    int len_buff = strlen(st->buff);
    if(len_buff > 0) {
        st->buff[len_buff-1] = '\0';
        if(st->entering_a) {
            st->a = (len_buff == 1) ? 0 : atoi(st->buff);
        } else {
            st->b = (len_buff == 1) ? 0 : atoi(st->buff);
        }
    }
    gtk_label_set_text(GTK_LABEL(st->label), st->display);
}
static void clear_clicked(GtkWidget *widget, gpointer user_data) {
    calc_state *st = (calc_state *)user_data;
    st->buff[0] = '\0';
    st->display[0] = '\0';
    st->a = 0;
    st->b = 0;
    st->operation = 0;
    st->result = 0;
    gtk_label_set_text(GTK_LABEL(st->label), st->buff);
    st->entering_a = TRUE;
}

static GtkWidget *create_button(const char *label, GCallback callback,
                                gpointer user_data, GtkWidget *grid, 
                                int col, int row,
                                int size_x, int size_y) {
    GtkWidget *button = gtk_button_new_with_label(label);
    g_signal_connect(button, "clicked", callback, user_data);
    gtk_grid_attach(GTK_GRID(grid), button, col, row, size_x, size_y);
    return button;
}

static void activate (GtkApplication *app,
                      gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  char *label_text[BUF_SIZE];

  grid = gtk_grid_new();
  gtk_container_add (GTK_CONTAINER (window), grid);
  
  GtkWidget *label = gtk_label_new("I'm a petty ass bitch");
 
  calc_state *st = g_malloc(sizeof(calc_state));
  st->label = label;
  st->buff[0] = '\0';
  st->entering_a = TRUE;  
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
  create_button("0", G_CALLBACK(number_clicked), st, grid, 0, 5, 3, 1);
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
