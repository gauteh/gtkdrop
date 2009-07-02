/*
 * gtkdrop.c - GtkDrop widget
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
# include <gtk/gtk.h>
# include <gdk/gdkkeysyms.h>

# include "gtkdrop.h"

static void gtk_drop_class_init 	(GtkDropClass *klass);
static void gtk_drop_init 			(GtkDrop *drop);

void gtk_drop_calculate_position (GtkDrop *drop);
gint gtk_drop_delete (GtkDrop *drop, GdkEvent *e, gpointer *data);
void gtk_drop_parent_event (GtkWidget *parent, GdkEvent *e, gpointer *data);
void gtk_drop_parent_moved (GtkWidget *parent, GdkEvent *e, gpointer *data);


static GtkWindowClass *parent_class = NULL;

enum {
  DROP_SHOWN_SIGNAL,
  DROP_HIDDEN_SIGNAL,
  LAST_SIGNAL
};

static guint drop_signals[LAST_SIGNAL] = { 0 };

static void gtk_drop_class_init (GtkDropClass *klass)
{
	// init any fields and set up signals
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	GtkContainerClass *container_class;
	GtkBinClass *bin_class;
	
	object_class = (GtkObjectClass*) klass;
	widget_class = (GtkWidgetClass*) klass;
	container_class = (GtkContainerClass*) klass;
	bin_class = (GtkBinClass*) klass;
	
	parent_class = gtk_type_class (gtk_window_get_type ());	
	
	drop_signals[DROP_SHOWN_SIGNAL] = 
	  g_signal_new ("drop-shown",
	                G_TYPE_FROM_CLASS (klass),
	                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
	                G_STRUCT_OFFSET (GtkDropClass, gtkdrop),
	                NULL, NULL,
	                g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	                
	drop_signals[DROP_HIDDEN_SIGNAL] = 
	  g_signal_new ("drop-hidden",
	                G_TYPE_FROM_CLASS (klass),
	                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
	                G_STRUCT_OFFSET (GtkDropClass, gtkdrop),
	                NULL, NULL,
	                g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	                
}

static void gtk_drop_init (GtkDrop *drop)
{
	// set fields to standard & create widgets
	drop->parent_window = NULL;
	drop->hidden = TRUE;
	drop->align = GTK_DROP_ALIGN_DOWN;
	drop->auto_calc_pos = TRUE;
	drop->margin = 0;
	drop->allocation = malloc (sizeof (GtkAllocation));
	drop->allocation->x = 0;
	drop->allocation->y = 0;
	drop->allocation->width = 0;
	drop->allocation->height = 0;
	drop->pos_widget = NULL;
	drop->calculate_position = NULL;
	drop->parentalloc = malloc (sizeof (GtkAllocation));

	
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (drop), TRUE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (drop), TRUE);
	gtk_window_set_modal (GTK_WINDOW (drop), FALSE);
	gtk_window_set_decorated (GTK_WINDOW (drop), FALSE);
	gtk_window_set_deletable (GTK_WINDOW (drop), FALSE);
	gtk_window_set_type_hint (GTK_WINDOW (drop), GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_set_resizable (GTK_WINDOW (drop), FALSE);
	gtk_window_set_accept_focus (GTK_WINDOW (drop), TRUE);
	
	g_signal_connect (G_OBJECT (drop), "delete-event", G_CALLBACK (gtk_drop_delete), NULL);
  /* the rest of the signals will be set in gtk_drop_new, since they need the parent_window */
  
}

guint gtk_drop_get_type ()
{
  static guint drop_type = 0;

  if (!drop_type)
    {
      GtkTypeInfo drop_info =
      {
			"GtkDrop",
			sizeof (GtkDrop),
			sizeof (GtkDropClass),
			(GtkClassInitFunc) gtk_drop_class_init,
			(GtkObjectInitFunc) gtk_drop_init,
			 NULL,
      };

      drop_type = gtk_type_unique (GTK_TYPE_WINDOW, &drop_info);
    }

  return drop_type;
}


GtkWidget * gtk_drop_new (GtkWindow *parent_window)
{
	if (GTK_IS_WINDOW (parent_window)) {
		GtkDrop *drop = gtk_type_new (gtk_drop_get_type ());
		drop->parent_window = parent_window;
		gtk_window_set_transient_for (GTK_WINDOW (drop), parent_window);
		
	  gtk_widget_add_events (GTK_WIDGET (drop->parent_window), GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_STRUCTURE_MASK);
	  
		g_signal_connect (G_OBJECT (parent_window), "button-press-event", G_CALLBACK (gtk_drop_parent_event), drop);
		g_signal_connect (G_OBJECT (parent_window), "key-press-event", G_CALLBACK (gtk_drop_parent_event), drop);

		g_signal_connect (G_OBJECT (parent_window), "event-after", G_CALLBACK (gtk_drop_parent_moved), drop);

		
		g_signal_connect (G_OBJECT (parent_window), "delete-event", G_CALLBACK (gtk_drop_delete), drop);
		
		return GTK_WIDGET (drop);
	} else {
		return NULL;
	}
}

void gtk_drop_parent_moved (GtkWidget *parent, GdkEvent *e, gpointer *data)
{
  GtkDrop *drop = GTK_DROP (data);
  if (drop->hidden == FALSE) {
    gint x, y, height, width;
    gtk_window_get_position (GTK_WINDOW (drop->parent_window), &x, &y);
    gtk_window_get_size (GTK_WINDOW (drop->parent_window), &width, &height);
    if ((x != drop->parentalloc->x) || (y != drop->parentalloc->y) || (height != drop->parentalloc->height) || (width != drop->parentalloc->width)) 
    {
      //gtk_drop_hide (drop);
      gtk_drop_refresh_position (drop);
    }   
  }
}

void gtk_drop_parent_event (GtkWidget *parent, GdkEvent *e, gpointer *data)
{ 
  GtkDrop *drop = GTK_DROP (data);

  if (drop->hidden == FALSE) {
    gtk_drop_hide (drop);
  }
}

gint gtk_drop_delete (GtkDrop *drop, GdkEvent *e, gpointer *data)
{
  gtk_drop_hide (drop);
  return 1;
}

void gtk_drop_refresh_position (GtkDrop *drop)
{
  gtk_drop_calculate_position (drop);
  gtk_window_move (GTK_WINDOW (drop), drop->allocation->x, drop->allocation->y);
}

void gtk_drop_set_position_widget (GtkDrop *drop, GtkWidget *widget)
{
	drop->auto_calc_pos = TRUE;
	drop->calculate_position = NULL;
	drop->pos_widget = widget;
}

void gtk_drop_set_position_function (GtkDrop *drop, void *calc_func)
{
	drop->calculate_position = calc_func;
	drop->auto_calc_pos = FALSE;
}

void gtk_drop_show (GtkDrop *drop)
{
  if (drop->hidden) {
	  gtk_drop_refresh_position (drop);
	  gtk_widget_show (GTK_WIDGET (drop));
	  drop->hidden = FALSE;
	  g_signal_emit (G_OBJECT (drop),
	                 drop_signals[DROP_SHOWN_SIGNAL], 0);	
  }
}

void gtk_drop_hide (GtkDrop *drop) 
{
  if (drop->hidden == FALSE) {
	  gtk_widget_hide (GTK_WIDGET (drop));
	  if (GTK_IS_WINDOW (drop->parent_window)) {
  	}
  	g_signal_emit (G_OBJECT (drop),
	                 drop_signals[DROP_HIDDEN_SIGNAL], 0);	
	  drop->hidden = TRUE;
  }
}

void gtk_drop_set_align (GtkDrop *drop, GtkDropAlign align)
{
	drop->align = align;
}

void gtk_drop_calculate_position (GtkDrop *drop)
{
  gtk_window_get_position (GTK_WINDOW (drop->parent_window), &(drop->parentalloc->x), &(drop->parentalloc->y));
	gtk_window_get_size (GTK_WINDOW (drop->parent_window), &(drop->parentalloc->width), &(drop->parentalloc->height));
	  
	if ((drop->auto_calc_pos == TRUE) || (drop->calculate_position == NULL)) {
		if (drop->pos_widget) {
			gint x, y;
			GdkWindow *parent_window = gtk_widget_get_parent_window (drop->pos_widget);
			gdk_window_get_origin (parent_window, &x, &y);
			GtkAllocation *widget_alloc = &(drop->pos_widget->allocation);
				
			gtk_window_get_size (GTK_WINDOW (drop), &(drop->allocation->width), &(drop->allocation->height));
			
			switch (drop->align) {
				case GTK_DROP_ALIGN_UP:
					x = x + widget_alloc->x;
					y = y + widget_alloc->y - drop->allocation->height - drop->margin;
					break;
				default:
				case GTK_DROP_ALIGN_DOWN:
					x = x + widget_alloc->x;
					y = y + widget_alloc->y + widget_alloc->height + drop->margin;
					break;
				case GTK_DROP_ALIGN_RIGHT:
					x = x + widget_alloc->x + widget_alloc->width + drop->margin;
					y = y + widget_alloc->y;
					break;
				case GTK_DROP_ALIGN_LEFT:
					x = x + widget_alloc->x - drop->allocation->width - drop->margin;
					y = y + widget_alloc->y;
					break;
			}
			
			drop->allocation->x = x;
			drop->allocation->y = y;
			
		} else {
			drop->allocation->x = 0;
			drop->allocation->y = 0;
		}		
	} else {
		drop->calculate_position (drop, &(drop->allocation->x), &(drop->allocation->y));
	}
}

