/* $Id$ */
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
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifndef GUCHARMAP_SCRIPT_CHAPTERS_H
#define GUCHARMAP_SCRIPT_CHAPTERS_H

#include <gucharmap/gucharmap-chapters.h>

G_BEGIN_DECLS

#define GUCHARMAP_SCRIPT_CHAPTERS(obj) \
            (G_TYPE_CHECK_INSTANCE_CAST ((obj), gucharmap_script_chapters_get_type (), GucharmapScriptChapters))

#define GUCHARMAP_SCRIPT_CHAPTERS_CLASS(clazz) \
            (G_TYPE_CHECK_CLASS_CAST ((clazz), gucharmap_script_chapters_get_type (), GucharmapScriptChaptersClass))

#define IS_GUCHARMAP_SCRIPT_CHAPTERS(obj) \
            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), gucharmap_script_chapters_get_type ()))

#define GUCHARMAP_SCRIPT_CHAPTERS_GET_CLASS(obj) \
            (G_TYPE_INSTANCE_GET_CLASS ((obj), gucharmap_script_chapters_get_type (), GucharmapScriptChaptersClass))

typedef struct _GucharmapScriptChapters GucharmapScriptChapters;
typedef struct _GucharmapScriptChaptersClass GucharmapScriptChaptersClass;

struct _GucharmapScriptChapters
{
  GucharmapChapters parent;
};

struct _GucharmapScriptChaptersClass
{
  GucharmapChaptersClass parent_class;
};

GType       gucharmap_script_chapters_get_type (void);
GtkWidget * gucharmap_script_chapters_new      (void);

G_END_DECLS

#endif /* #ifndef GUCHARMAP_SCRIPT_CHAPTERS_H */

