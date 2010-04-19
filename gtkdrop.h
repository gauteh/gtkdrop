/*
 * gtkdrop.h - Header for the GtkDrop widget
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

# ifndef _GTK_DROP_H_
# define _GTK_DROP_H_

# include <gdk/gdk.h>
# include <gtk/gtkwindow.h>


# define GTK_TYPE_DROP 			  (gtk_drop_get_type ())
# define GTK_DROP(obj)          GTK_CHECK_CAST (obj, gtk_drop_get_type (), GtkDrop)
# define GTK_DROP_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_drop_get_type (), GtkDropClass)
# define GTK_IS_DROP(obj)       GTK_CHECK_TYPE (obj, gtk_drop_get_type ())


typedef struct _GtkDrop       GtkDrop;
typedef struct _GtkDropClass  GtkDropClass;

typedef enum {
	GTK_DROP_ALIGN_UP,
	GTK_DROP_ALIGN_DOWN,
	GTK_DROP_ALIGN_RIGHT,
	GTK_DROP_ALIGN_LEFT

} GtkDropAlign;

struct _GtkDrop
{
	GtkWindow window;
	GtkWindow *parent_window;
	GtkWidget *pos_widget;
	GtkDropAlign align;

  int hidden;
  int auto_calc_pos;
  guint margin;

	GtkAllocation *allocation;
	GtkAllocation *parentalloc;


	void (*calculate_position)	(GtkDrop*, gint *, gint *);
};

struct _GtkDropClass
{
  GtkWindowClass parent_class;

  void (* gtkdrop) (GtkDrop *drop);
};

guint       gtk_drop_get_type        		  (void);
GtkWidget*  gtk_drop_new             		  ();
void	      gtk_drop_show           		  (GtkDrop *drop);
void				gtk_drop_hide						      (GtkDrop *drop);
void				gtk_drop_set_position_widget 	(GtkDrop *drop, GtkWidget *widget);
void        gtk_drop_set_parent_window (GtkDrop *drop, GtkWindow *parent_window);
void 				gtk_drop_set_position_function (GtkDrop *drop, void (*calc_func));
void				gtk_drop_refresh_position		  (GtkDrop *drop);
void				gtk_drop_set_align				    (GtkDrop *drop, GtkDropAlign align);


# endif

