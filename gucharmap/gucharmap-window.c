/*
 * Copyright © 2004 Noah Levitt
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

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "gucharmap-window.h"
#include "gucharmap-search-dialog.h"
#include "gucharmap-settings.h"

static void gucharmap_window_class_init (GucharmapWindowClass *klass);
static void gucharmap_window_init       (GucharmapWindow *window);

G_DEFINE_TYPE (GucharmapWindow, gucharmap_window, GTK_TYPE_WINDOW)

static void
status_message (GtkWidget       *widget, 
                const gchar     *message, 
                GucharmapWindow *guw)
{
  gtk_statusbar_pop (GTK_STATUSBAR (guw->status), 0);

  if (message)
    gtk_statusbar_push (GTK_STATUSBAR (guw->status), 0, message);
}

static gboolean
update_progress_bar (GucharmapWindow *guw)
{
  gdouble fraction_completed;

  fraction_completed = gucharmap_search_dialog_get_completed (GUCHARMAP_SEARCH_DIALOG (guw->search_dialog));

  if (fraction_completed < 0 || fraction_completed > 1)
    {
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (guw->progress), 0);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (guw->progress), NULL);
      return FALSE;
    }
  else
    {
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (guw->progress), fraction_completed);
      return TRUE;
    }
}

/* "progress" aka "busy-interactive" cursor (pointer + watch)
 * from mozilla 
 * caller should gdk_cursor_unref */
GdkCursor *
_gucharmap_window_progress_cursor (void)
{
  /* MOZ_CURSOR_SPINNING */
  static const char moz_spinning_bits[] = 
    {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
      0x00, 0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00,
      0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xfc,
      0x01, 0x00, 0x00, 0xfc, 0x3b, 0x00, 0x00, 0x7c, 0x38, 0x00, 0x00,
      0x6c, 0x54, 0x00, 0x00, 0xc4, 0xdc, 0x00, 0x00, 0xc0, 0x44, 0x00,
      0x00, 0x80, 0x39, 0x00, 0x00, 0x80, 0x39, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };
  static const char moz_spinning_mask_bits[] = 
    {
      0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00,
      0x00, 0x1e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00,
      0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xfe,
      0x3b, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00,
      0xfe, 0xfe, 0x00, 0x00, 0xee, 0xff, 0x01, 0x00, 0xe4, 0xff, 0x00,
      0x00, 0xc0, 0x7f, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00, 0x80, 0x39,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };

  GdkPixmap *cursor, *mask;
  GdkCursor *gdkcursor;
  GdkColor fg = { 0, 0, 0, 0 };             /* black */
  GdkColor bg = { 0, 65535, 65535, 65535 }; /* white */

  cursor = gdk_bitmap_create_from_data (NULL, moz_spinning_bits, 32, 32);
  mask = gdk_bitmap_create_from_data (NULL, moz_spinning_mask_bits, 32, 32);

  gdkcursor = gdk_cursor_new_from_pixmap (cursor, mask, &fg, &bg, 2, 2);

  g_object_unref (cursor);
  g_object_unref (mask);

  return gdkcursor;
}

static void
search_start (GucharmapSearchDialog *search_dialog,
              GucharmapWindow       *guw)
{
  GdkCursor *cursor;
  GtkAction *action;

  cursor = _gucharmap_window_progress_cursor ();
  gdk_window_set_cursor (GTK_WIDGET (guw)->window, cursor);
  gdk_cursor_unref (cursor);

  action = gtk_action_group_get_action (guw->action_group, "Find");
  gtk_action_set_sensitive (action, FALSE);
  action = gtk_action_group_get_action (guw->action_group, "FindNext");
  gtk_action_set_sensitive (action, FALSE);
  action = gtk_action_group_get_action (guw->action_group, "FindPrevious");
  gtk_action_set_sensitive (action, FALSE);

  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (guw->progress), _("Searching..."));
  g_timeout_add (100, (GSourceFunc) update_progress_bar, guw);
}

static void
search_finish (GucharmapSearchDialog *search_dialog,
               gunichar               found_char,
               GucharmapWindow       *guw)
{
  GtkAction *action;

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (guw->progress), 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (guw->progress), NULL);

  if (found_char != (gunichar)(-1))
    gucharmap_charmap_set_active_character (guw->charmap, found_char);
  /* not-found dialog handled by GucharmapSearchDialog */

  gdk_window_set_cursor (GTK_WIDGET (guw)->window, NULL);

  action = gtk_action_group_get_action (guw->action_group, "Find");
  gtk_action_set_sensitive (action, TRUE);
  action = gtk_action_group_get_action (guw->action_group, "FindNext");
  gtk_action_set_sensitive (action, TRUE);
  action = gtk_action_group_get_action (guw->action_group, "FindPrevious");
  gtk_action_set_sensitive (action, TRUE);
}

static void
search_find (GtkAction       *action, 
             GucharmapWindow *guw)
{
  g_assert (GUCHARMAP_IS_WINDOW (guw));

  if (guw->search_dialog == NULL)
    {
      guw->search_dialog = gucharmap_search_dialog_new (guw);
      g_signal_connect (guw->search_dialog, "search-start", G_CALLBACK (search_start), guw);
      g_signal_connect (guw->search_dialog, "search-finish", G_CALLBACK (search_finish), guw);
    }

  gtk_window_present (GTK_WINDOW (guw->search_dialog));
}

static void
search_find_next (GtkAction       *action, 
                  GucharmapWindow *guw)
{
  if (guw->search_dialog)
    gucharmap_search_dialog_start_search (GUCHARMAP_SEARCH_DIALOG (guw->search_dialog), GUCHARMAP_DIRECTION_FORWARD);
  else
    search_find (action, guw);
}

static void
search_find_prev (GtkAction       *action, 
                  GucharmapWindow *guw)
{
  if (guw->search_dialog)
    gucharmap_search_dialog_start_search (GUCHARMAP_SEARCH_DIALOG (guw->search_dialog), GUCHARMAP_DIRECTION_BACKWARD);
  else
    search_find (action, guw);
}

static void
font_bigger (GtkAction       *action, 
             GucharmapWindow *guw)
{
  gint size, increment;

  size = gucharmap_mini_font_selection_get_font_size (GUCHARMAP_MINI_FONT_SELECTION (guw->fontsel));
  increment = MAX (size / 5, 1);
  gucharmap_mini_font_selection_set_font_size (GUCHARMAP_MINI_FONT_SELECTION (guw->fontsel), size + increment);
}

static void
font_smaller (GtkAction       *action, 
              GucharmapWindow *guw)
{
  gint size, increment;

  size = gucharmap_mini_font_selection_get_font_size (GUCHARMAP_MINI_FONT_SELECTION (guw->fontsel));
  increment = MAX (size / 5, 1);
  gucharmap_mini_font_selection_set_font_size (GUCHARMAP_MINI_FONT_SELECTION (guw->fontsel), size - increment);
}

static void
font_default (GtkAction       *action, 
              GucharmapWindow *guw)
{
  gucharmap_mini_font_selection_reset_font_size (GUCHARMAP_MINI_FONT_SELECTION (guw->fontsel));
}

static void
snap_cols_pow2 (GtkAction        *action, 
                GucharmapWindow  *guw)
{
  gboolean is_active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));
  gucharmap_chartable_set_snap_pow2 (gucharmap_charmap_get_chartable (guw->charmap), is_active);
  gucharmap_settings_set_snap_pow2 (is_active);
}

static void
open_url (GtkWindow *parent,
          const char *url)
{
  GError *error = NULL;
  GdkScreen *screen;
  char *command;

  screen = gtk_widget_get_screen (GTK_WIDGET (parent));

  command = g_strconcat ("gnome-open ", url, NULL);
  gdk_spawn_command_line_on_screen (screen, command, &error);
  g_free (command);

  if (error) {
    GtkWidget *d;

    d = gtk_message_dialog_new (parent,
                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                "%s", error->message);
    g_signal_connect (d, "response", G_CALLBACK (gtk_widget_destroy), NULL);
    gtk_window_present (GTK_WINDOW (d));
    g_error_free (error);
  }
}

static void
help_contents (GtkAction *action,
               GtkWindow *window)
{
  const char *lang;
  char *uri = NULL, *url;
  guint i;
 
  const char * const * langs = g_get_language_names ();
  for (i = 0; langs[i]; i++) {
    lang = langs[i];
    if (strchr (lang, '.')) {
      continue;
    }
 
    uri = g_build_filename (DATADIR,
                            "/gnome/help/gucharmap/",
                            lang,
                            "/gucharmap.xml",
                            NULL);
					
    if (g_file_test (uri, G_FILE_TEST_EXISTS)) {
      break;
    }
  }

  if (!uri)
    return;

  url = g_strconcat ("ghelp://", uri, NULL);
  open_url (window, url);
  g_free (url);
}

static void
about_open_url (GtkAboutDialog *about,
                const char *link,
                gpointer data)
{
  open_url (GTK_WINDOW (about), link);
}

static void
help_about (GtkAction       *action, 
            GucharmapWindow *guw)
{
  const gchar *authors[] =
    { 
      "Noah Levitt <nlevitt@columbia.edu>", 
      "Daniel Elstner <daniel.elstner@gmx.net>", 
      "Padraig O'Briain <Padraig.Obriain@sun.com>",
      NULL 
    };

  const gchar *documenters[] =
    {
      "Chee Bin HOH <cbhoh@gnome.org>",
      "Sun Microsystems",
      NULL
    };	  

  const gchar *license[] = {
    N_("Gucharmap is free software; you can redistribute it and/or modify "
       "it under the terms of the GNU General Public License as published by "
       "the Free Software Foundation; either version 2 of the License, or "
       "(at your option) any later version."),
    N_("Permission is hereby granted, free of charge, to any person obtaining "
       "a copy of the Unicode data files to deal in them without restriction, "
       "including without limitation the rights to use, copy, modify, merge, "
       "publish, distribute, and/or sell copies."),
    N_("Gucharmap and the Unicode data files are distributed in the hope that "
       "they will be useful, but WITHOUT ANY WARRANTY; without even the implied "
       "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See "
       "the GNU General Public License and Unicode Copyright for more details."),
    N_("You should have received a copy of the GNU General Public License "
       "along with Gucharmap; if not, write to the Free Software Foundation, Inc., "
       "59 Temple Place, Suite 330, Boston, MA  02110-1301  USA"),
    N_("Also you should have received a copy of the Unicode Copyright along "
       "with Gucharmap; you can always find it at Unicode's website: "
       "http://www.unicode.org/copyright.html")
  };
  gchar *license_trans;
  license_trans = g_strconcat (_(license[0]), "\n\n", _(license[1]), "\n\n",
			       _(license[2]), "\n\n", _(license[3]), "\n\n",
			       _(license[4]), "\n\n", NULL);

  gtk_about_dialog_set_url_hook (about_open_url, NULL, NULL);

  gtk_show_about_dialog (GTK_WINDOW (guw),
  			 "authors", authors,
			 "comments", _("GNOME Character Map\n"
				       "based on the Unicode Character Database"),
			 "copyright", "Copyright © 2004 Noah Levitt <nlevitt@columbia.edu>\n"
				      "Copyright © 1991-2005 Unicode, Inc.",
			 "documenters", documenters,
			 "license", license_trans,
			 "program-name", _("Gucharmap"),
			 "logo-icon-name", GUCHARMAP_ICON_NAME,
			 "translator-credits", _("translator-credits"),
			 "version", VERSION,
			 "website", "http://live.gnome.org/Gucharmap",
			 "wrap-license", TRUE,
			 NULL);

  g_free (license_trans);
}

static void
next_or_prev_character (GtkAction       *action,
                        GucharmapWindow *guw)
{
  GucharmapChartable *chartable;
  GucharmapChartableClass *klass;
  GtkBindingSet *binding_set;
  const char *name;
  guint keyval = 0;

  name = gtk_action_get_name (action);
  if (strcmp (name, "NextCharacter") == 0) {
    keyval = GDK_Right;
  } else if (strcmp (name, "PreviousCharacter") == 0) {
    keyval = GDK_Left;
  }

  chartable = gucharmap_charmap_get_chartable (guw->charmap);
  klass = GUCHARMAP_CHARTABLE_GET_CLASS (chartable);
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_set_activate (gtk_binding_set_by_class (klass),
                            keyval,
                            0,
                            GTK_OBJECT (chartable));
}

static void
next_chapter (GtkAction       *action,
              GucharmapWindow *guw)
{
  gucharmap_chapters_view_next (gucharmap_charmap_get_chapters_view (guw->charmap));
}

static void
prev_chapter (GtkAction       *action,
              GucharmapWindow *guw)
{
  gucharmap_chapters_view_previous (gucharmap_charmap_get_chapters_view (guw->charmap));
}

static void
chapters_set_labels (const gchar     *labelnext,
		     const gchar     *labelprev,
		     GucharmapWindow *guw)
{
  GtkAction *action;

  action = gtk_action_group_get_action (guw->action_group, "NextChapter");
  g_object_set ( G_OBJECT (action), "label", labelnext, NULL);
  action = gtk_action_group_get_action (guw->action_group, "PreviousChapter");
  g_object_set ( G_OBJECT (action), "label", labelprev, NULL);
}

enum {
  VIEW_BY_SCRIPT,
  VIEW_BY_BLOCK
};

static void
set_chapters_model (GucharmapWindow *guw,
                    GucharmapChaptersMode mode)
{
  GucharmapChaptersModel *model = NULL;

  switch (mode)
    {
      case GUCHARMAP_CHAPTERS_SCRIPT:
      	model = gucharmap_script_chapters_model_new ();
	chapters_set_labels (_("Next Script"), _("Previous Script"), guw);
	break;
      
      case GUCHARMAP_CHAPTERS_BLOCK:
      	model = gucharmap_block_chapters_model_new ();
	chapters_set_labels (_("Next Block"), _("Previous Block"), guw);
	break;
      
      default:
        g_assert_not_reached ();
    }

  gucharmap_charmap_set_chapters_model (guw->charmap, model);
  g_object_unref (model);
}

static void
view_by (GtkAction        *action,
	 GtkRadioAction   *radioaction,
         GucharmapWindow  *guw)
{
  GucharmapChaptersMode mode;

  switch (gtk_radio_action_get_current_value (radioaction))
    {
      case VIEW_BY_SCRIPT:
        mode = GUCHARMAP_CHAPTERS_SCRIPT;
	break;
      
      case VIEW_BY_BLOCK:
        mode = GUCHARMAP_CHAPTERS_BLOCK;
	break;
      
      default:
        g_assert_not_reached ();
    }

  set_chapters_model (guw, mode);
  gucharmap_settings_set_chapters_mode (mode);
}

#ifdef DEBUG_chpe
static void
move_to_next_screen_cb (GtkAction *action,
                        GtkWidget *widget)
{
  GdkScreen *screen;
  GdkDisplay *display;
  int number_of_screens, screen_num;

  screen = gtk_widget_get_screen (widget);
  display = gdk_screen_get_display (screen);
  screen_num = gdk_screen_get_number (screen);
  number_of_screens =  gdk_display_get_n_screens (display);

  if ((screen_num + 1) < number_of_screens) {
    screen = gdk_display_get_screen (display, screen_num + 1);
  } else {
    screen = gdk_display_get_screen (display, 0);
  }

  gtk_window_set_screen (GTK_WINDOW (widget), screen);
}
#endif

/* create the menu entries */

static const char ui_info [] =
  "<menubar name='MenuBar'>"
    "<menu action='File'>"
      "<menuitem action='Close' />"
#ifdef DEBUG_chpe
      "<menuitem action='MoveNextScreen' />"
#endif
    "</menu>"
    "<menu action='View'>"
      "<menuitem action='ByScript' />"
      "<menuitem action='ByUnicodeBlock' />"
      "<separator />"
      "<menuitem action='SnapColumns' />"
      "<separator />"
      "<menuitem action='ZoomIn' />"
      "<menuitem action='ZoomOut' />"
      "<menuitem action='NormalSize' />"
    "</menu>"
    "<menu action='Search'>"
      "<menuitem action='Find' />"
      "<menuitem action='FindNext' />"
      "<menuitem action='FindPrevious' />"
    "</menu>"
    "<menu action='Go'>"
      "<menuitem action='NextCharacter' />"
      "<menuitem action='PreviousCharacter' />"
      "<separator />"
      "<menuitem action='NextChapter' />"
      "<menuitem action='PreviousChapter' />"
    "</menu>"
    "<menu action='Help'>"
      "<menuitem action='HelpContents' />"
      "<menuitem action='About' />"
    "</menu>"
  "</menubar>";

static GtkWidget *
make_menu (GucharmapWindow *guw)
{
  /* tooltips are NULL because they are never actually shown in the program */
  const GtkActionEntry menu_entries[] =
  {
    { "File", NULL, N_("_File"), NULL, NULL, NULL },
    { "View", NULL, N_("_View"), NULL, NULL, NULL },
    { "Search", NULL, N_("_Search"), NULL, NULL, NULL },
    { "Go", NULL, N_("_Go"), NULL, NULL, NULL },
    { "Help", NULL, N_("_Help"), NULL, NULL, NULL },

    { "Close", GTK_STOCK_CLOSE, NULL, NULL,
      NULL, G_CALLBACK (gtk_main_quit) },

    { "ZoomIn", GTK_STOCK_ZOOM_IN, NULL, NULL,
      NULL, G_CALLBACK (font_bigger) },
    { "ZoomOut", GTK_STOCK_ZOOM_OUT, NULL, NULL,
      NULL, G_CALLBACK (font_smaller) },
    { "NormalSize", GTK_STOCK_ZOOM_100, NULL, NULL,
      NULL, G_CALLBACK (font_default) },

    { "Find", GTK_STOCK_FIND, NULL, NULL,
      NULL, G_CALLBACK (search_find) },
    { "FindNext", GTK_STOCK_FIND, N_("Find _Next"), "<control>G",
      NULL, G_CALLBACK (search_find_next) },
    { "FindPrevious", GTK_STOCK_FIND, N_("Find _Previous"), "<shift><control>G",
      NULL, G_CALLBACK (search_find_prev) },

    { "NextCharacter", NULL, N_("_Next Character"), "<control>N",
      NULL, G_CALLBACK (next_or_prev_character) },
    { "PreviousCharacter", NULL, N_("_Previous Character"), "<control>P",
      NULL, G_CALLBACK (next_or_prev_character) },
    { "NextChapter", NULL, N_("Next Script"), "<control>Page_Down",
      NULL, G_CALLBACK (next_chapter) },
    { "PreviousChapter", NULL, N_("Previous Script"), "<control>Page_Up",
      NULL, G_CALLBACK (prev_chapter) },

    { "HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1",
      NULL, G_CALLBACK (help_contents) },
    { "About", GTK_STOCK_ABOUT, N_("_About"), NULL,
      NULL, G_CALLBACK (help_about) },

  #ifdef DEBUG_chpe
    { "MoveNextScreen", NULL, "Move window to next screen", NULL,
      NULL, G_CALLBACK (move_to_next_screen_cb) },
  #endif
  };
  const GtkRadioActionEntry radio_menu_entries[] =
  {
    { "ByScript", NULL, N_("By _Script"), NULL,
      NULL, VIEW_BY_SCRIPT },
    { "ByUnicodeBlock", NULL, N_("By _Unicode Block"), NULL,
      NULL, VIEW_BY_BLOCK }
  };
  const GtkToggleActionEntry toggle_menu_entries[] =
  {
    { "SnapColumns", NULL, N_("Snap _Columns to Power of Two"), NULL,
      NULL,
      G_CALLBACK (snap_cols_pow2), FALSE },
  };
  GtkWidget *menubar;
  GtkAction *action;

  guw->uimanager = gtk_ui_manager_new();

  gtk_window_add_accel_group ( GTK_WINDOW (guw),
  			       gtk_ui_manager_get_accel_group (guw->uimanager) );
  
  guw->action_group = gtk_action_group_new ("gucharmap_actions");
  gtk_action_group_set_translation_domain (guw->action_group, GETTEXT_PACKAGE);

  gtk_action_group_add_actions (guw->action_group,
  				menu_entries,
				G_N_ELEMENTS (menu_entries),
				guw);
  gtk_action_group_add_radio_actions (guw->action_group,
  				      radio_menu_entries,
				      G_N_ELEMENTS (radio_menu_entries),
				      gucharmap_settings_get_chapters_mode(),
				      G_CALLBACK (view_by),
				      guw);
  gtk_action_group_add_toggle_actions (guw->action_group,
  				       toggle_menu_entries,
				       G_N_ELEMENTS (toggle_menu_entries),
				       guw);

  action = gtk_action_group_get_action (guw->action_group, "SnapColumns");
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                                gucharmap_settings_get_snap_pow2 ());
  gucharmap_window_set_file_menu_visible (guw, TRUE);

  gtk_ui_manager_insert_action_group (guw->uimanager, guw->action_group, 0);
  g_object_unref (guw->action_group);
  
  gtk_ui_manager_add_ui_from_string (guw->uimanager, ui_info, strlen (ui_info), NULL);
  
  menubar = gtk_ui_manager_get_widget (guw->uimanager, "/MenuBar");

  return menubar;
}

static void
fontsel_changed (GucharmapMiniFontSelection *fontsel, 
                 GucharmapWindow            *guw)
{
  gchar *font_name = gucharmap_mini_font_selection_get_font_name (fontsel);

  gucharmap_charmap_set_font (guw->charmap, font_name);
  gucharmap_settings_set_font (font_name);

  g_free (font_name);
}

static void
insert_character_in_text_to_copy (GucharmapChartable *chartable,
                                  GucharmapWindow *guw)
{
  gchar ubuf[7];
  gint pos;
  gunichar wc;

  wc = gucharmap_chartable_get_active_character (chartable);
  /* Can't copy values that are not valid unicode characters */
  if (!gucharmap_unichar_validate (wc))
    return;

  /* don't do anything if text_to_copy is not active */
  if (!guw->text_to_copy_visible)
    return;

  ubuf[g_unichar_to_utf8 (wc, ubuf)] = '\0';
  gtk_editable_delete_selection (GTK_EDITABLE (guw->text_to_copy_entry));
  pos = gtk_editable_get_position (GTK_EDITABLE (guw->text_to_copy_entry));
  gtk_editable_insert_text (GTK_EDITABLE (guw->text_to_copy_entry), ubuf, -1, &pos);
  gtk_editable_set_position (GTK_EDITABLE (guw->text_to_copy_entry), pos);
}

static void
edit_copy (GtkWidget *widget, GucharmapWindow *guw)
{
  /* if nothing is selected, select the whole thing */
  if (! gtk_editable_get_selection_bounds (
              GTK_EDITABLE (guw->text_to_copy_entry), NULL, NULL))
    gtk_editable_select_region (GTK_EDITABLE (guw->text_to_copy_entry), 0, -1);

  gtk_editable_copy_clipboard (GTK_EDITABLE (guw->text_to_copy_entry));
}

static void
entry_changed_sensitize_button (GtkEditable *editable, GtkWidget *button)
{
  const gchar *entry_text = gtk_entry_get_text (GTK_ENTRY (editable));
  gtk_widget_set_sensitive (button, entry_text[0] != '\0');
}

static GtkWidget *
make_text_to_copy (GucharmapWindow *guw)
{
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *hbox;

  hbox = gtk_hbox_new (FALSE, 6);

  label = gtk_label_new_with_mnemonic (_("_Text to copy:"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  guw->text_to_copy_entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), guw->text_to_copy_entry, TRUE, TRUE, 0);
  gtk_widget_show (guw->text_to_copy_entry);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label), guw->text_to_copy_entry);

  /* the copy button */
  button = gtk_button_new_from_stock (GTK_STOCK_COPY); 
  gtk_widget_show (button);
  gtk_widget_set_sensitive (button, FALSE);
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (edit_copy), guw);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

  g_signal_connect (G_OBJECT (guw->text_to_copy_entry), "changed",
                    G_CALLBACK (entry_changed_sensitize_button), button);

  gtk_widget_set_tooltip_text (button, _("Copy to the clipboard."));

  return hbox;
}

static void
status_realize (GtkWidget       *status,
                GucharmapWindow *guw)
{
  /* FIXMEchpe ewww... */
  /* increase the height a bit so it doesn't resize itself */
  gtk_widget_set_size_request (guw->status, -1, guw->status->allocation.height + 9);
}


static gboolean
save_last_char_idle_cb (GucharmapWindow *guw)
{
  gunichar wc;

  guw->save_last_char_idle_id = 0;

  wc = gucharmap_charmap_get_active_character (guw->charmap);
  gucharmap_settings_set_last_char (wc);

  return FALSE;
}

static void
charmap_sync_active_character (GtkWidget *widget,
                               GParamSpec *pspec,
                               GucharmapWindow *guw)
{
  if (guw->save_last_char_idle_id != 0)
    return;

  guw->save_last_char_idle_id = g_idle_add ((GSourceFunc) save_last_char_idle_cb, guw);
}

static void
pack_stuff_in_window (GucharmapWindow *guw)
{
  GtkWidget *big_vbox;
  GtkWidget *hbox;
  GucharmapChartable *chartable;

  guw->charmap = GUCHARMAP_CHARMAP (gucharmap_charmap_new ());
  g_signal_connect (guw->charmap, "notify::active-character",
                    G_CALLBACK (charmap_sync_active_character), guw);

  big_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (guw), big_vbox);

  gtk_box_pack_start (GTK_BOX (big_vbox), make_menu (guw), FALSE, FALSE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (big_vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  gtk_box_pack_start (GTK_BOX (big_vbox), GTK_WIDGET (guw->charmap),
                      TRUE, TRUE, 0);
  gtk_widget_show (GTK_WIDGET (guw->charmap));

  guw->fontsel = gucharmap_mini_font_selection_new ();
  g_signal_connect (guw->fontsel, "changed", G_CALLBACK (fontsel_changed), guw);
  gtk_box_pack_start (GTK_BOX (hbox), guw->fontsel, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET (guw->fontsel));

  guw->text_to_copy_container = make_text_to_copy (guw);
  gtk_container_set_border_width (GTK_CONTAINER (guw->text_to_copy_container), 6);
  gtk_box_pack_start (GTK_BOX (big_vbox), guw->text_to_copy_container, FALSE, FALSE, 0);
  /* FIXMEchpe!! */
  chartable =gucharmap_charmap_get_chartable (guw->charmap);
  g_signal_connect (chartable, "activate", G_CALLBACK (insert_character_in_text_to_copy), guw);

  
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (big_vbox), hbox, FALSE, FALSE, 0);

  guw->status = gtk_statusbar_new ();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (guw->status), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), guw->status, TRUE, TRUE, 0);
  gtk_widget_show (guw->status);
  g_signal_connect (guw->status, "realize", G_CALLBACK (status_realize), guw);

  guw->progress = gtk_progress_bar_new ();
  gtk_box_pack_start (GTK_BOX (hbox), guw->progress, FALSE, FALSE, 0);

#if 0
  grip = gtk_statusbar_new ();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (grip), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), grip, FALSE, FALSE, 0);
#endif
  gtk_widget_show_all (hbox);

  g_signal_connect (guw->charmap, "status-message", G_CALLBACK (status_message), guw);

  gtk_widget_show (big_vbox);

  set_chapters_model (guw, guw->chapters_mode);

  gucharmap_charmap_set_active_character (guw->charmap,
                                          gucharmap_settings_get_last_char ());
}

static void
gucharmap_window_init (GucharmapWindow *guw)
{
  gtk_window_set_title (GTK_WINDOW (guw), _("Character Map"));

  guw->font_selection_visible = FALSE;
  guw->text_to_copy_visible = FALSE;
  guw->file_menu_visible = FALSE;
  guw->chapters_mode = gucharmap_settings_get_chapters_mode ();

  guw->search_dialog = NULL;

  gtk_window_set_icon_name (GTK_WINDOW (guw), GUCHARMAP_ICON_NAME);

  pack_stuff_in_window (guw);

  gtk_widget_grab_focus (GTK_WIDGET (gucharmap_charmap_get_chartable (guw->charmap)));
}

static void
window_finalize (GObject *object)
{
  GucharmapWindow *guw = GUCHARMAP_WINDOW (object);

  if (guw->save_last_char_idle_id != 0)
    g_source_remove (guw->save_last_char_idle_id);

  G_OBJECT_CLASS (gucharmap_window_parent_class)->finalize (object);
}

static void
gucharmap_window_class_init (GucharmapWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = window_finalize;
}

GtkWidget *
gucharmap_window_new (void)
{
  return GTK_WIDGET (g_object_new (gucharmap_window_get_type (), NULL));
}

void 
gucharmap_window_set_font_selection_visible (GucharmapWindow *guw, 
                                             gboolean         visible)
{
  guw->font_selection_visible = visible != FALSE;

  g_object_set (guw->fontsel, "visible", visible, NULL);
}

void 
gucharmap_window_set_text_to_copy_visible (GucharmapWindow *guw, 
                                           gboolean         visible)
{
  guw->text_to_copy_visible = visible != FALSE;

  g_object_set (guw->text_to_copy_container, "visible", visible, NULL);
}

void 
gucharmap_window_set_file_menu_visible (GucharmapWindow *guw, 
                                        gboolean         visible)
{
  GtkAction *action;

  guw->file_menu_visible = visible != FALSE;

  action = gtk_action_group_get_action (guw->action_group, "File");
  gtk_action_set_visible (action, visible);
  action = gtk_action_group_get_action (guw->action_group, "Close");
  gtk_action_set_sensitive (action, visible);
}

GucharmapMiniFontSelection *
gucharmap_window_get_mini_font_selection (GucharmapWindow *guw)
{
  return GUCHARMAP_MINI_FONT_SELECTION (guw->fontsel);
}
