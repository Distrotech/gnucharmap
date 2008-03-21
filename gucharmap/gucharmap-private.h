/*
 * Copyright (c) 2004 Noah Levitt
 * Copyright (C) 2007 Christian Persch
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
 * 59 Temple Place, Suite 330, Boston, MA 02110-1301  USA
 */

#include <gtk/gtk.h>

#include "gucharmap-types.h"

#define I_(string) g_intern_static_string (string)

/* The last unicode character we support */
#define UNICHAR_MAX (0x0010FFFFUL)


struct _GucharmapChaptersModel
{
  GtkListStore parent_instance;

  /*< protected >*/
  GucharmapCodepointList *book_list;
};

struct _GucharmapChaptersModelClass
{
  GtkListStoreClass parent_class;

  const char *title;
  gboolean (* character_to_iter) (GucharmapChaptersModel *chapters,
                                  gunichar wc,
                                  GtkTreeIter *iter);
  GucharmapCodepointList * (* get_codepoint_list) (GucharmapChaptersModel *chapters,
                                                   GtkTreeIter *iter);
  G_CONST_RETURN GucharmapCodepointList * (* get_book_codepoint_list) (GucharmapChaptersModel *chapters);
};


struct _GucharmapBlockChaptersModel
{
  GucharmapChaptersModel parent;
};

struct _GucharmapBlockChaptersModelClass
{
  GucharmapChaptersModelClass parent_class;
};


struct _GucharmapChaptersView
{
  GtkTreeView parent_instance;

  /*< private >*/
  GtkTreeViewColumn *column;
  GucharmapChaptersModel *model;
};

struct _GucharmapChaptersViewClass
{
  GtkTreeViewClass parent_class;
};


struct _GucharmapCharmap
{
  GtkHPaned parent;

  GucharmapChaptersView *chapters_view;
  GucharmapChartable *chartable;
  GtkTextView *details_view;
  GtkTextTag *text_tag_gimongous;
  GtkTextTag *text_tag_big;

  PangoFontDescription *font_desc;

  GdkCursor *hand_cursor;
  GdkCursor *regular_cursor;

  guint hovering_over_link   : 1;
  guint showing_details_page : 1;
  guint last_character_set   : 1;
};


struct _GucharmapCharmapClass
{
  GtkHPanedClass parent_class;

  void (* status_message) (GucharmapCharmap *charmap, const gchar *message);
  void (* link_clicked) (GucharmapCharmap *charmap, 
                         gunichar old_character,
                         gunichar new_character);
};


struct _GucharmapChartableCellAccessible
{
  AtkObject parent;

  GtkWidget    *widget;
  int           index;
  AtkStateSet  *state_set;
  gchar        *activate_description;
  guint         action_idle_handler;
};

struct _GucharmapChartableCellAccessibleClass
{
  AtkObjectClass parent_class;
};


struct _GucharmapChartable
{
  GtkDrawingArea parent_instance;

  /* scrollable implementation */
  GtkAdjustment *vadjustment;
  gulong vadjustment_changed_handler_id;

  /* Font */
  PangoFontDescription *font_desc;
  int drag_font_size;

  /* Geometry */
  int bare_minimal_column_width; /* depends only on font_desc */
  int bare_minimal_row_height;   /* depends only on font_desc */
  int minimal_column_width;      /* depends on font_desc and size allocation */
  int minimal_row_height;        /* depends on font_desc and size allocation */
  int n_padded_columns;          /* columns 0..n-1 will be 1px wider than minimal_column_width */
  int n_padded_rows;             /* rows 0..n-1 will be 1px taller than minimal_row_height */
  int rows;
  int cols;
  int page_size;       /* rows * cols */

  int page_first_cell; /* the cell index of the top left corner */
  int active_cell;     /* the active cell index */
  int old_page_first_cell;
  int old_active_cell;

  /* Drawing */
  GdkPixmap *pixmap;
  PangoLayout *pango_layout;

  /* Zoom popup */
  GtkWidget *zoom_window;
  GtkWidget *zoom_image;

  /* for dragging (#114534) */
  gdouble click_x, click_y; 

  GtkTargetList *target_list;

  GucharmapCodepointList *codepoint_list;
  int last_cell; /* from gucharmap_codepoint_list_get_last_index */
  gboolean codepoint_list_changed;

  /* Settings */
  guint snap_pow2_enabled : 1;
  guint zoom_mode_enabled : 1;
};

struct _GucharmapChartableClass
{
  GtkDrawingAreaClass parent_class;

  void    (* set_scroll_adjustments) (GucharmapChartable *chartable,
                                      GtkAdjustment      *hadjustment,
                                      GtkAdjustment      *vadjustment);
  gboolean (* move_cursor)           (GucharmapChartable *chartable,
                                      GtkMovementStep     step,
                                      gint                count);
  void (* activate) (GucharmapChartable *chartable);

  void (* set_active_char) (GucharmapChartable *chartable, guint ch);
  void (* status_message) (GucharmapChartable *chartable, const gchar *message);
};

gint _gucharmap_chartable_cell_column	(GucharmapChartable *chartable,
					 guint cell);
gint _gucharmap_chartable_column_width	(GucharmapChartable *chartable,
					 gint col);
gint _gucharmap_chartable_x_offset	(GucharmapChartable *chartable,
					 gint col);
gint _gucharmap_chartable_row_height	(GucharmapChartable *chartable,
		 			 gint row);
gint _gucharmap_chartable_y_offset	(GucharmapChartable *chartable,
					 gint row);
void _gucharmap_chartable_redraw	(GucharmapChartable *chartable,
					 gboolean move_zoom);


struct _GucharmapCodepointList
{
  GObject parent;
};

struct _GucharmapCodepointListClass
{
  GObjectClass parent_class;

  /* zero is the first index */
  gint     (*get_last_index) (GucharmapCodepointList *list);
  gunichar (*get_char)       (GucharmapCodepointList *list, 
                              gint                    index);
  gint     (*get_index)      (GucharmapCodepointList *list, 
                              gunichar                wc);
};


struct _GucharmapMiniFontSelection
{
  GtkHBox parent;

  GtkListStore         *family_store;
  GtkWidget            *family; /* combo box */
  GtkWidget            *bold;   /* toggle button*/
  GtkWidget            *italic; /* toggle button*/

  GtkObject            *size_adj; 
  GtkWidget            *size;   /* spin button */
  
  PangoFontDescription *font_desc;

  gint                  default_size;
};

struct _GucharmapMiniFontSelectionClass
{
  GtkHBoxClass parent_class;

  void (* changed) (GucharmapMiniFontSelection *fontsel);
};


struct _GucharmapScriptChaptersModel
{
  GucharmapChaptersModel parent;
};

struct _GucharmapScriptChaptersModelClass
{
  GucharmapChaptersModelClass parent_class;
};


struct _GucharmapScriptCodepointList
{
  GucharmapCodepointList parent;
};

struct _GucharmapScriptCodepointListClass
{
  GucharmapCodepointListClass parent_class;
};


struct _GucharmapSearchDialog
{
  GtkDialog parent;
};

struct _GucharmapSearchDialogClass
{
  GtkDialogClass parent_class;

  /* signals */
  void (* search_start)  (void);
  void (* search_finish) (gunichar found_char);
};


struct _GucharmapWindow
{
  GtkWindow parent;

  GucharmapCharmap *charmap; 
};

struct _GucharmapWindowClass
{
  GtkWindowClass parent_class;
};

GdkCursor * _gucharmap_window_progress_cursor (void);
