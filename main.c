#include <gtk/gtk.h>
#include <stdlib.h>
#include "tetris.h"
#include "display.h"

int main (int argc, char *argv[])
{
	GtkWidget *window;

	gtk_init (&argc, &argv);

	t_init ();
	
	window = d_init ( );

	gtk_widget_show_all (window);
	gtk_main ();
	return 0;
}
