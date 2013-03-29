#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include "display.h"
#include "tetris.h"

#define TABLE_WIDTH      T_COL  // column
#define TABLE_HEIGHT     T_ROW  // row
#define TE_PIXEL_WIDTH   30  // pixel
#define TE_PIXEL_HEIGHT  30  // pixel
#define TETRIMINO_WIDTH  TE_PIXEL_WIDTH  // pixels

static GtkWidget *drawable_table;
static GtkWidget *drawable_next;
static GtkWidget *label_score;
static gboolean is_pause = TRUE;
static gboolean is_game_over = FALSE;

static void d_force_table_update (void)
{
	GdkWindow *window;
	GdkRectangle rect;

	window = gtk_widget_get_window (drawable_table);

	rect.x = 0;
	rect.y = 0;
	rect.width = TABLE_WIDTH * TETRIMINO_WIDTH;
	rect.height = TABLE_HEIGHT * TETRIMINO_WIDTH;

	gdk_window_invalidate_rect (window, &rect, FALSE);
	gdk_window_process_updates (window, FALSE);
}

static void d_force_next_update (void)
{
	GdkWindow *window;
	GdkRectangle rect;

	window = gtk_widget_get_window (drawable_next);

	rect.x = 0;
	rect.y = 0;
	rect.width = 4 * TETRIMINO_WIDTH;
	rect.height = 4 * TETRIMINO_WIDTH;

	gdk_window_invalidate_rect (window, &rect, FALSE);
	gdk_window_process_updates (window, FALSE);
}

////////////////////////////////////////////////////////////////////////
static void d_game_over (void)
{
	GtkWidget *dialog = NULL;

	dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
			GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
			"LOSER");
}

static void d_update_score (void)
{
	char buf[10] = "";

	sprintf (buf, "%d", t_get_score ());
	gtk_label_set_text (GTK_LABEL (label_score), buf);
}

static gboolean on_play_timeout (gpointer data)
{
	if (is_game_over)
		return FALSE;

	if (t_key_down () == 0) {
		d_force_table_update ();
	} else {
		if (t_te_out () == 0) {
			t_check_and_score ();
			d_update_score ();
			t_te_create_next ();
			d_force_table_update ();
			d_force_next_update ();
		} else {
			is_game_over = TRUE;
			return FALSE;
		}
	}

	if (is_pause) {
		return FALSE;
	} else {
		return TRUE;
	}
}

static void second_to_print (gint second, gchar *buffer)  // OK
{
	gint hour, min, sec;
	gchar shour[5] = "";
	gchar smin[5] = "";
	gchar ssec[5] = "";

	sec = second % 60;
	min = second / 60;
	hour = min / 60;
	min = min % 60;

	if (hour < 10)
		sprintf (shour, "0%d", hour);
	else
		sprintf (shour, "%d", hour);

	if (min < 10)
		sprintf (smin, "0%d", min);
	else
		sprintf (smin, "%d", min);

	if (sec < 10)
		sprintf (ssec, "0%d", sec);
	else
		sprintf (ssec, "%d", sec);

	sprintf (buffer, "%s:%s:%s", shour, smin, ssec);
}

static gboolean on_key_press (GtkWidget *widget,
		GdkEventKey *event,
		gpointer data)
{
	if (is_game_over)
		return FALSE;

	if (is_pause)
		return FALSE;

	switch (event->keyval) {
	case GDK_KEY_Up:
		t_key_turn ();
		d_force_table_update ();
		break;

	case GDK_KEY_Down:
		if (t_key_down () == 0) {
			d_force_table_update ();
		} else {
			t_check_and_score ();
			d_update_score ();
			t_te_create_next ();
			t_te_out ();
			d_force_table_update ();
			d_force_next_update ();
		}
		break;

	case GDK_KEY_Left:
		t_key_left ();
		d_force_table_update ();
		break;

	case GDK_KEY_Right:
		t_key_right ();
		d_force_table_update ();
		break;

	default:;
	}

	return FALSE;
}

static gboolean on_expose_next (GtkWidget *widget,
		GdkEventExpose *event,
		gpointer data)
{	
	cairo_t *cr;
	cairo_surface_t *image;
	P_NEXT p_next;
	int i = 0;
	int x = 0, y = 0;
	int width = 0, height = 0;

	p_next = t_get_next ();

	cr = gdk_cairo_create (gtk_widget_get_window (widget));
	image = cairo_image_surface_create_from_png ("te.png");

	width = 4 * TETRIMINO_WIDTH;
	height = 4 * TETRIMINO_WIDTH;
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);

	for (i = 0; i < 4; i++) {
		x = ((*p_next)[i]).x * TETRIMINO_WIDTH;
		y = ((*p_next)[i]).y * TETRIMINO_WIDTH;
		cairo_set_source_surface (cr, image, x, y);
		cairo_paint (cr);
	}

	cairo_surface_destroy (image);
	cairo_destroy (cr);
	return FALSE;
}

static gboolean on_expose_table (GtkWidget *widget,
		GdkEventExpose *event,
		gpointer data)
{
	cairo_t *cr;
	cairo_surface_t *image;
	P_TABLE p_table;
	int i = 0, j = 0;
	int x = 0, y = 0;
	int width = 0, height = 0;

	p_table = t_get_table ();

	cr = gdk_cairo_create (gtk_widget_get_window (widget));
	image = cairo_image_surface_create_from_png ("te.png");

	width = TABLE_WIDTH * TETRIMINO_WIDTH;
	height = TABLE_HEIGHT * TETRIMINO_WIDTH;
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);

	for (i = 0; i < TABLE_HEIGHT; i++)
	for (j = 0; j < TABLE_WIDTH; j++) {
		if (p_table[i][j] > 0) {
			x = j * TETRIMINO_WIDTH;
			y = i * TETRIMINO_WIDTH;
			cairo_set_source_surface (cr, image, x, y);
			cairo_paint (cr);
		}
	}

	cairo_surface_destroy (image);
	cairo_destroy (cr);
	return FALSE;
}

static gboolean on_pause_clicked (GtkWidget *widget,
		gpointer data)
{
	extern gboolean is_pause;
	static gboolean is_first_clicked = TRUE;

	if (is_game_over)
		return FALSE;

	if (is_first_clicked) {
		t_te_create_next ();
		if (t_te_out () == 0) {
			d_force_next_update ();
			d_force_table_update ();
		} else {
			is_game_over = TRUE;
		}

		is_first_clicked = FALSE;
	}

	if (is_pause) {
		is_pause = FALSE;
		gtk_button_set_label (GTK_BUTTON (widget), "Pause");
		g_timeout_add (1000, on_play_timeout, NULL);
	} else {
		is_pause = TRUE;
		gtk_button_set_label (GTK_BUTTON (widget), "Start");
	}
}

static gboolean on_timeout (gpointer data)
{
	GtkWidget *label;
	gchar buf[20] = "";
	static gint time_second = 0;
	extern gboolean is_timing;

	if (!is_pause) {
		time_second++;
		second_to_print (time_second, buf);

		label = (GtkWidget *)data;
		gtk_label_set_text (GTK_LABEL (label), buf);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
static GtkWidget *d_next (void)
{
	GtkWidget *vbox;
	GtkWidget *hbox1, *hbox2;
	GtkWidget *label;

	vbox = gtk_vbox_new (FALSE, 0);
	hbox1 = gtk_hbox_new (FALSE, 0);
	hbox2 = gtk_hbox_new (FALSE, 0);
	label = gtk_label_new ("Next:");
	drawable_next = gtk_drawing_area_new ();

	gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox1), hbox2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), drawable_next, FALSE, FALSE, 0);

	gtk_drawing_area_size (GTK_DRAWING_AREA (drawable_next), 
			4 * TETRIMINO_WIDTH, 
			4 * TETRIMINO_WIDTH);
	g_signal_connect (GTK_OBJECT (drawable_next), "expose-event",
			G_CALLBACK (on_expose_next), NULL);

	return vbox;
}

static GtkWidget *d_score (void)
{
	GtkWidget *vbox;
	GtkWidget *hbox1, *hbox2;
	GtkWidget *label;

	vbox = gtk_vbox_new (FALSE, 0);
	hbox1 = gtk_hbox_new (FALSE, 0);
	hbox2 = gtk_hbox_new (FALSE, 0);
	label = gtk_label_new ("Score:");
	label_score = gtk_label_new ("");

	gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox1), hbox2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), label_score, FALSE, FALSE, 0);

	return vbox;
}

static GtkWidget *d_time (void)
{
	GtkWidget *vbox;
	GtkWidget *hbox1, *hbox2;
	GtkWidget *label, *label_time;

	vbox = gtk_vbox_new (FALSE, 0);
	hbox1 = gtk_hbox_new (FALSE, 0);
	hbox2 = gtk_hbox_new (FALSE, 0);
	label = gtk_label_new ("Time:");
	label_time = gtk_label_new ("");

	gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox1), hbox2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), label_time, FALSE, FALSE, 0);

	g_timeout_add (1000, on_timeout, (gpointer)label_time);

	return vbox;
}

static GtkWidget *d_pause (void)
{
	GtkWidget *hbox;
	GtkWidget *button;

	hbox = gtk_hbox_new (FALSE, 0);
	button = gtk_button_new_with_label ("Start");

	gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 10);

	g_signal_connect (GTK_OBJECT (button), "clicked",
			G_CALLBACK (on_pause_clicked), NULL);

	return hbox;
}

static GtkWidget *d_table (void)
{
	drawable_table = gtk_drawing_area_new ();
	gtk_drawing_area_size (GTK_DRAWING_AREA (drawable_table), 
			TABLE_WIDTH * TETRIMINO_WIDTH, 
			TABLE_HEIGHT * TETRIMINO_WIDTH);
	g_signal_connect (GTK_OBJECT (drawable_table), "expose-event",
			G_CALLBACK (on_expose_table), NULL);

	return drawable_table;
}
//////////////////////////////////////////////////////////////////

GtkWidget *d_init (void)
{
	GtkWidget *window;
	GtkWidget *hbox, *vbox;
	GtkWidget *table, *next, *score, *time, *pause;

	// main window
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Tetris");
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (window), 5);
	g_signal_connect (GTK_OBJECT (window), "destroy",
			G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (window, "key_press_event",
			G_CALLBACK (on_key_press), (gpointer)table);

	hbox = gtk_hbox_new (FALSE, 0);
	vbox = gtk_vbox_new (FALSE, 0);
	table = d_table ();
	next = d_next ();
	score = d_score ();
	time = d_time ();
	pause = d_pause ();

	gtk_box_pack_start (GTK_BOX (vbox), next, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), score, FALSE, FALSE, 20);
	gtk_box_pack_start (GTK_BOX (vbox), time, FALSE, FALSE, 20);
	gtk_box_pack_start (GTK_BOX (vbox), pause, FALSE, FALSE, 70);

	gtk_box_pack_start (GTK_BOX (hbox), table, FALSE, FALSE, 10);
	gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 10);

	gtk_container_add (GTK_CONTAINER (window), hbox);

	t_te_create ();

	return window;
}
