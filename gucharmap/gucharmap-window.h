/* $Id$ */
/*
 * Copyright (c) 2003  Noah Levitt <nlevitt аt columbia.edu>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */


#ifndef GUCHARMAP_WINDOW_H
#define GUCHARMAP_WINDOW_H

#include <gtk/gtk.h>
#include <gucharmap/gucharmap-charmap.h>


G_BEGIN_DECLS

#define GUCHARMAP_WINDOW(obj) (\
        G_TYPE_CHECK_INSTANCE_CAST ((obj), gucharmap_window_get_type (), \
                                    GucharmapWindow))

#define GUCHARMAP_WINDOW_CLASS(clazz) (\
        G_TYPE_CHECK_CLASS_CAST ((clazz), gucharmap_window_get_type () \
                                 GucharmapWindowClass))

#define IS_GUCHARMAP_WINDOW(obj) (\
        G_TYPE_CHECK_INSTANCE_TYPE ((obj), gucharmap_window_get_type ()))


typedef struct _GucharmapWindow GucharmapWindow;
typedef struct _GucharmapWindowClass GucharmapWindowClass;


struct _GucharmapWindow
{
  GtkWindow parent;

  GucharmapCharmap *charmap;
  GtkWidget *status;
  GtkAccelGroup *accel_group;

  GtkWidget *fontsel;
  GtkWidget *text_to_copy_container; /* the thing to show/hide */
  GtkWidget *text_to_copy_entry;

  GtkWidget *unicode_options_menu_item;
  GtkWidget *unihan_options_menu_item;
  GtkWidget *nameslist_options_menu_item;

  GtkWidget *file_menu_item;
  GtkWidget *quit_menu_item;

  GdkPixbuf *icon;

  gchar *last_search;
  gboolean search_in_progress;

  gboolean font_selection_visible;
  gboolean text_to_copy_visible;
  gboolean file_menu_visible;

  /* points to the element we are currently at (could be in the middle of
   * the list) */
  GList *history;
  GtkWidget *back_menu_item;
  GtkWidget *forward_menu_item;
};


struct _GucharmapWindowClass
{
  GtkWindowClass parent_class;
};


GType gucharmap_window_get_type ();
GtkWidget * gucharmap_window_new ();
void gucharmap_window_set_font_selection_visible (GucharmapWindow *guw, 
                                                  gboolean visible);
void gucharmap_window_set_text_to_copy_visible (GucharmapWindow *guw, 
                                                gboolean visible);
void gucharmap_window_set_file_menu_visible (GucharmapWindow *guw, 
                                             gboolean visible);



G_END_DECLS

#endif /* #ifndef GUCHARMAP_WINDOW_H */


