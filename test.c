/*
 * test.c - Test window for the GtkDrop widget
 *
 * Copyright (C) 2008 Gaute Hope <eg@gaute.vetsj.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

# include <stdlib.h>
# include <stdio.h>
# include <glib.h>
# include <gtk/gtk.h>
# include <gdk/gdkkeysyms.h>

# include "gtkdrop.h"

typedef struct _TestDrop TestDrop;

struct _TestDrop {
	GtkWindow *win;
	GtkDrop *drop;
	GtkWidget *button;
};

gboolean quit (GtkWidget *w, GdkEvent *e, gpointer *data);
gboolean key_pressed (GtkWidget *w, GdkEventKey *e, gpointer *data);
gboolean button_toggled (GtkWidget *w, GdkEvent *e, gpointer *data);
gboolean drop_hidden (GtkWidget *w, GdkEvent *e, gpointer *data);
void calc_drop_pos (int *x, int *y);

static TestDrop *test = NULL;

int main (int argc, char ** argv)
{
  printf ("Test program for the GtkDrop widget\n");
	printf ("Copyright (C) 2008 Gaute Hope <eg@gaute.vetsj.com>\n");

  /* {{{ Set up test window and toggle button --- */
  gtk_init (&argc, &argv);
  test = (TestDrop*) malloc (sizeof (TestDrop));

  GtkWidget *w = GTK_WIDGET(gtk_window_new (GTK_WINDOW_TOPLEVEL));
  test->win = GTK_WINDOW(w);

  gtk_widget_show (GTK_WIDGET(test->win));
  gtk_window_set_resizable (GTK_WINDOW (test->win), FALSE);
  gtk_window_set_title (GTK_WINDOW (test->win), "GtkDrop test");

  g_signal_connect (G_OBJECT (test->win), "delete-event", G_CALLBACK (quit), NULL);
  g_signal_connect (G_OBJECT (test->win), "key-press-event", G_CALLBACK (key_pressed), NULL);

  GtkWidget *button = gtk_toggle_button_new ();
  g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (button_toggled), NULL);
  test->button = button;

	GtkWidget *button_label = gtk_label_new ("Toggle");

  GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), button_label, FALSE, FALSE, 5);
  GtkWidget *arrow = gtk_arrow_new (GTK_ARROW_UP, GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (hbox), arrow, FALSE, FALSE, 5);

  GtkWidget *mvbox = gtk_vbox_new (FALSE, 50);
  GtkWidget *mhbox = gtk_hbox_new (FALSE, 50);

  gtk_container_add (GTK_CONTAINER (button), hbox);
  gtk_box_pack_start (GTK_BOX (mhbox), button, FALSE, FALSE, 50);
  gtk_box_pack_start (GTK_BOX (mvbox), mhbox, FALSE, FALSE, 50);
 	gtk_container_add (GTK_CONTAINER (w), mvbox);
  /* End setting up test window }}} */

  /* Set up GtkDrop */

  /* We have to either use gtk_drop_position_widget ()
   * _or_ gtk_drop_set_parent () for GtkDrop to know
   * which GtkWindow to look for events on and be
   * transient for */

  GtkWidget *drop = gtk_drop_new ();
 	test->drop = GTK_DROP (drop);

  /* It will be positioned relative to the toggle button; test->button 
   * 
   * The alternative is to make your own function that returns the
   * position for the GtkDrop window; specify it with
   * gtk_drop_set_position_function (). Make sure you also call
   * gtk_drop_set_parent () with the window it will be transient for.
   */
 	gtk_drop_set_position_widget (test->drop, button);
 	gtk_drop_set_align (test->drop, GTK_DROP_ALIGN_UP);

 	g_signal_connect (G_OBJECT (drop), "drop-hidden", G_CALLBACK (drop_hidden), NULL);

  /* Add some widgets to the GtkDrop window/container */
 	GtkFrame *frame = GTK_FRAME (gtk_frame_new (NULL));
 	gtk_container_add (GTK_CONTAINER (drop), GTK_WIDGET (frame));
 	gtk_widget_show (GTK_WIDGET (frame));

  GtkWidget *e_hbox = gtk_hbox_new (FALSE, 5);

 	GtkWidget *l = gtk_label_new ("Test label:");
 	gtk_box_pack_start (GTK_BOX (e_hbox), l, FALSE, FALSE, 5);

 	GtkWidget *e = gtk_entry_new ();
 	gtk_box_pack_start (GTK_BOX (e_hbox), e, FALSE, FALSE, 5);

 	GtkWidget *b = gtk_button_new_with_label ("Test");
 	gtk_box_pack_start (GTK_BOX (e_hbox), b, FALSE, FALSE, 5);

 	gtk_container_add (GTK_CONTAINER (frame), e_hbox);
 	gtk_widget_show_all (e_hbox);

  /* Show test window */
 	gtk_widget_show_all (w);

  /* Run program */
 	gtk_main ();
 	return 0;
}

/* Callback to run when GtkDrop is hidden: reset toggle button */
gboolean drop_hidden (GtkWidget *w, GdkEvent *e, gpointer *data)
{
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (test->button), FALSE);
}

/* Toggle button pushed, show/hide GtkDrop */
gboolean button_toggled (GtkWidget *w, GdkEvent *e, gpointer *data)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w))) {
		gtk_drop_show (test->drop);
	} else {
		gtk_drop_hide (test->drop);
	}
}

/* Exit Test program when ESC is pressed */
gboolean key_pressed (GtkWidget *w, GdkEventKey *e, gpointer *data)
{
  if (e->type == GDK_KEY_PRESS) {
    if (e->keyval == GDK_Escape) {
      gtk_main_quit ();
    }
  }
  return TRUE;
}

/* Quit program and clean up */
gboolean quit (GtkWidget *w, GdkEvent *e, gpointer *data)
{
  gtk_main_quit ();
  return TRUE;
}

