/* $Id$ */
/*
 * Copyright (c) 2003  Noah Levitt <nlevitt@users.sourceforge.net>
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

#ifndef MINI_FONTSEL_H
#define MINI_FONTSEL_H

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define MINI_FONT_SELECTION(obj) \
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), mini_font_selection_get_type (), \
                                     MiniFontSelection))

#define MINI_FONT_SELECTION_CLASS(clazz) \
        (G_TYPE_CHECK_CLASS_CAST ((clazz), mini_font_selection_get_type (), \
                                  MiniFontSelectionClass))

#define IS_MINI_FONT_SELECTION(obj) \
        G_TYPE_CHECK_INSTANCE_TYPE ((obj), mini_font_selection_get_type ())


typedef struct _MiniFontSelection MiniFontSelection;
typedef struct _MiniFontSelectionClass MiniFontSelectionClass;


struct _MiniFontSelection
{
  GtkHBox parent;

  GtkWidget *family; /* combo box */
  GtkWidget *style;  /* combo box */

  GtkObject *size_adj; 
  GtkWidget *size;   /* spin button */

  gchar *family_value; /* current family */
  gchar *style_value;  /* current face */
  gint size_value;     /* current size */

  gint style_changed_handler_id;
};


struct _MiniFontSelectionClass
{
  GtkHBoxClass parent_class;
};


GType mini_font_selection_get_type ();
GtkWidget * mini_font_selection_new ();
gboolean mini_font_selection_set_font_name (MiniFontSelection *fontsel,
                                            const gchar *fontname);
gchar * mini_font_selection_get_font_name (MiniFontSelection *fontsel);

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef MINI_FONTSEL_H */

