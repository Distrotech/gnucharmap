/* $Id$ */
/*
 * Copyright (c) 2003 Noah Levitt
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

#include "unicode-names.h"
#include "unicode-blocks.h"
#include "unicode-nameslist.h"
#include "unicode-categories.h"
#if ENABLE_UNIHAN
# include "unicode-unihan.h"
#endif

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <gucharmap_intl.h>
#include <gucharmap/gucharmap-unicode-info.h>

/* constants for hangul (de)composition, see UAX #15 */
#define SBase 0xAC00
#define LBase 0x1100
#define VBase 0x1161
#define TBase 0x11A7
#define LCount 19
#define VCount 21
#define TCount 28
#define NCount (VCount * TCount)
#define SCount (LCount * NCount)

static const gchar * const JAMO_L_TABLE[] = {
  "G", "GG", "N", "D", "DD", "R", "M", "B", "BB",
  "S", "SS", "", "J", "JJ", "C", "K", "T", "P", "H", NULL
};

static const gchar * const JAMO_V_TABLE[] = {
  "A", "AE", "YA", "YAE", "EO", "E", "YEO", "YE", "O",
  "WA", "WAE", "OE", "YO", "U", "WEO", "WE", "WI",
  "YU", "EU", "YI", "I"
};

static const gchar * const JAMO_T_TABLE[] = {
  "", "G", "GG", "GS", "N", "NJ", "NH", "D", "L", "LG", "LM",
  "LB", "LS", "LT", "LP", "LH", "M", "B", "BS",
  "S", "SS", "NG", "J", "C", "K", "T", "P", "H"
};

/* computes the hangul name as per UAX #15 */
G_CONST_RETURN gchar *
get_hangul_syllable_name (gunichar s)
{
  static gchar buf[32];
  gint SIndex = s - SBase;
  gint LIndex, VIndex, TIndex;

  if (SIndex < 0 || SIndex >= SCount)
    return "";

  LIndex = SIndex / NCount;
  VIndex = (SIndex % NCount) / TCount;
  TIndex = SIndex % TCount;

  g_snprintf (buf, 32, _("HANGUL SYLLABLE %s%s%s"), JAMO_L_TABLE[LIndex],
              JAMO_V_TABLE[VIndex], JAMO_T_TABLE[TIndex]);

  return buf;
}


G_CONST_RETURN gchar *
gucharmap_get_unicode_name (gunichar uc)
{
  if (uc >= 0x3400 && uc <= 0x4DB5)
    return _("<CJK Ideograph Extension A>");
  else if (uc >= 0x4e00 && uc <= 0x9fa5)
    return _("<CJK Ideograph>");
  else if (uc >= 0xac00 && uc <= 0xd7af)
    return get_hangul_syllable_name (uc);
  else if (uc >= 0xD800 && uc <= 0xDB7F) 
    return _("<Non Private Use High Surrogate>");
  else if (uc >= 0xDB80 && uc <= 0xDBFF) 
    return _("<Private Use High Surrogate>");
  else if (uc >= 0xDC00 && uc <= 0xDFFF)
    return _("<Low Surrogate, Last>");
  else if (uc >= 0xE000 && uc <= 0xF8FF) 
    return _("<Private Use>");
  else if (uc >= 0xF0000 && uc <= 0xFFFFD)
    return _("<Plane 15 Private Use>");
  else if (uc >= 0x100000 && uc <= 0x10FFFD)
    return _("<Plane 16 Private Use>");
  else if (uc >= 0x20000 && uc <= 0x2A6D6)
    return _("<CJK Ideograph Extension B>");
  else
    {
      const gchar *x = gucharmap_get_unicode_data_name (uc);
      if (x == NULL)
        return _("<not assigned>");
      else
        return x;
    }
}


G_CONST_RETURN gchar *
gucharmap_get_unicode_category_name (gunichar uc)
{
  switch (gucharmap_unichar_type (uc))
    {
      case G_UNICODE_CONTROL: return _("Other, Control");
      case G_UNICODE_FORMAT: return _("Other, Format");
      case G_UNICODE_UNASSIGNED: return _("Other, Not Assigned");
      case G_UNICODE_PRIVATE_USE: return _("Other, Private Use");
      case G_UNICODE_SURROGATE: return _("Other, Surrogate");
      case G_UNICODE_LOWERCASE_LETTER: return _("Letter, Lowercase");
      case G_UNICODE_MODIFIER_LETTER: return _("Letter, Modifier");
      case G_UNICODE_OTHER_LETTER: return _("Letter, Other");
      case G_UNICODE_TITLECASE_LETTER: return _("Letter, Titlecase");
      case G_UNICODE_UPPERCASE_LETTER: return _("Letter, Uppercase");
      case G_UNICODE_COMBINING_MARK: return _("Mark, Spacing Combining");
      case G_UNICODE_ENCLOSING_MARK: return _("Mark, Enclosing");
      case G_UNICODE_NON_SPACING_MARK: return _("Mark, Non-Spacing");
      case G_UNICODE_DECIMAL_NUMBER: return _("Number, Decimal Digit");
      case G_UNICODE_LETTER_NUMBER: return _("Number, Letter");
      case G_UNICODE_OTHER_NUMBER: return _("Number, Other");
      case G_UNICODE_CONNECT_PUNCTUATION: return _("Punctuation, Connector");
      case G_UNICODE_DASH_PUNCTUATION: return _("Punctuation, Dash");
      case G_UNICODE_CLOSE_PUNCTUATION: return _("Punctuation, Close");
      case G_UNICODE_FINAL_PUNCTUATION: return _("Punctuation, Final Quote");
      case G_UNICODE_INITIAL_PUNCTUATION: return _("Punctuation, Initial Quote");
      case G_UNICODE_OTHER_PUNCTUATION: return _("Punctuation, Other");
      case G_UNICODE_OPEN_PUNCTUATION: return _("Punctuation, Open");
      case G_UNICODE_CURRENCY_SYMBOL: return _("Symbol, Currency");
      case G_UNICODE_MODIFIER_SYMBOL: return _("Symbol, Modifier");
      case G_UNICODE_MATH_SYMBOL: return _("Symbol, Math");
      case G_UNICODE_OTHER_SYMBOL: return _("Symbol, Other");
      case G_UNICODE_LINE_SEPARATOR: return _("Separator, Line");
      case G_UNICODE_PARAGRAPH_SEPARATOR: return _("Separator, Paragraph");
      case G_UNICODE_SPACE_SEPARATOR: return _("Separator, Space");
      default: return "";
    }
}


/* counts the number of entries in gucharmap_unicode_blocks with start <= max */
gint 
gucharmap_count_blocks (gunichar max)
{
  gint i;

  for (i = 0;  gucharmap_unicode_blocks[i].start != (gunichar)(-1)
               && gucharmap_unicode_blocks[i].start < max;  i++);

  return i;
}


/* http://www.unicode.org/unicode/reports/tr15/#Hangul */
static gunichar *
hangul_decomposition (gunichar s, gsize *result_len)
{
  gunichar *r = g_malloc (3 * sizeof (gunichar));
  gint SIndex = s - SBase;

  /* not a hangul syllable */
  if (SIndex < 0 || SIndex >= SCount)
    {
      r[0] = s;
      *result_len = 1;
    }
  else
    {
      gunichar L = LBase + SIndex / NCount;
      gunichar V = VBase + (SIndex % NCount) / TCount;
      gunichar T = TBase + SIndex % TCount;

      r[0] = L;
      r[1] = V;

      if (T != TBase) 
        {
          r[2] = T;
          *result_len = 3;
        }
      else
        *result_len = 2;
    }

  return r;
}


/*
 * See http://bugzilla.gnome.org/show_bug.cgi?id=100456
 *
 * gucharmap_unicode_canonical_decomposition:
 * @ch: a Unicode character.
 * @result_len: location to store the length of the return value.
 *
 * Computes the canonical decomposition of a Unicode character.  
 * 
 * Return value: a newly allocated string of Unicode characters.
 *   @result_len is set to the resulting length of the string.
 */
gunichar *
gucharmap_unicode_canonical_decomposition (gunichar ch, 
                                           gsize *result_len)
{
  if (ch >= 0xac00 && ch <= 0xd7af)  /* Hangul syllable */
    return hangul_decomposition (ch, result_len);
  else 
    return g_unicode_canonical_decomposition (ch, result_len);
}



/* does a binary search on unicode_names */
G_CONST_RETURN gchar *
gucharmap_get_unicode_data_name (gunichar uc)
{
  gint min = 0;
  gint mid;
  gint max = sizeof (unicode_names) / sizeof (UnicodeName) - 1;

  if (uc < unicode_names[0].index || uc > unicode_names[max].index)
    return "";

  while (max >= min) 
    {
      mid = (min + max) / 2;
      if (uc > unicode_names[mid].index)
        min = mid + 1;
      else if (uc < unicode_names[mid].index)
        max = mid - 1;
      else
        return unicode_names[mid].name;
    }

  return NULL;
}


/* ascii case-insensitive substring search (source ripped from glib) */
static G_CONST_RETURN gchar *
ascii_case_strrstr (const gchar *haystack, const gchar *needle)
{
  gsize i;
  gsize needle_len;
  gsize haystack_len;
  const gchar *p;
      
  g_return_val_if_fail (haystack != NULL, NULL);
  g_return_val_if_fail (needle != NULL, NULL);

  needle_len = strlen (needle);
  haystack_len = strlen (haystack);

  if (needle_len == 0)
    return haystack;

  if (haystack_len < needle_len)
    return NULL;
  
  p = haystack + haystack_len - needle_len;

  while (p >= haystack)
    {
      for (i = 0; i < needle_len; i++)
        if (g_ascii_tolower (p[i]) != g_ascii_tolower (needle[i]))
          goto next;
      
      return p;
      
    next:
      p--;
    }
  
  return NULL;
}


/* case insensitive; returns (gunichar)(-1) if nothing found */
/* direction must be +1 or -1 */
gunichar
gucharmap_find_substring_match (gunichar start, 
                                const gchar *search_text,
                                gint direction)
{
  gint max = sizeof (unicode_names) / sizeof (UnicodeName) - 1;
  gint i0;
  gint i;

  g_assert (direction == -1 || direction == 1);

  /* locate the start character by binary search */
  if (start < unicode_names[0].index || start > UNICHAR_MAX)
    i0 = 0;
  else
    {
      gint min = 0, mid = 0;

      while (max >= min) 
        {
          mid = (min + max) / 2;
          if (start > unicode_names[mid].index)
            min = mid + 1;
          else if (start < unicode_names[mid].index)
            max = mid - 1;
          else
            break;
        }

      i0 = mid;
    }

  max = sizeof (unicode_names) / sizeof (UnicodeName) - 1;
  /* try substring match on each */
  for (i = i0 + direction;  i != i0;  )
    {
      if (unicode_names[i].index > UNICHAR_MAX)
        {
          i += direction;
          continue;
        }

      if (ascii_case_strrstr (unicode_names[i].name, search_text) != NULL)
        return unicode_names[i].index;

      i += direction;
      if (i > max)
        i = 0;
      else if (i < 0)
        i = max;
    }

  /* if the start character matches we want to return a match */
  if (ascii_case_strrstr (unicode_names[i].name, search_text) != NULL)
    return unicode_names[i].index;

  return (gunichar)(-1);
}


#if ENABLE_UNIHAN

/* does a binary search; also caches most recent, since it will often be
 * called in succession on the same character */
static G_CONST_RETURN Unihan *
_get_unihan (gunichar uc)
{
  static gunichar most_recent_searched;
  static const Unihan *most_recent_result;
  gint min = 0;
  gint mid;
  gint max = sizeof (unihan) / sizeof (Unihan) - 1;

  if (uc < unihan[0].index || uc > unihan[max].index)
    return NULL;

  if (uc == most_recent_searched)
    return most_recent_result;

  most_recent_searched = uc;

  while (max >= min) 
    {
      mid = (min + max) / 2;
      if (uc > unihan[mid].index)
        min = mid + 1;
      else if (uc < unihan[mid].index)
        max = mid - 1;
      else
        {
          most_recent_result = unihan + mid;
          return unihan + mid;
        }
    }

  most_recent_result = NULL;
  return NULL;
}


G_CONST_RETURN gchar * 
gucharmap_get_unicode_kDefinition (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kDefinition;
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kCantonese (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kCantonese;
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kMandarin (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kMandarin;
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kTang (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kTang;
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kKorean (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kKorean;
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kJapaneseKun (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kJapeneseKun;
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kJapaneseOn (gunichar uc)
{
  const Unihan *uh = _get_unihan (uc);
  if (uh == NULL)
    return NULL;
  else
    return uh->kJapaneseOn;
}

#else /* #if ENABLE_UNIHAN */

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kDefinition (gunichar uc)
{
  return "This feature was not compiled in.";
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kCantonese (gunichar uc)
{
  return "This feature was not compiled in.";
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kMandarin (gunichar uc)
{
  return "This feature was not compiled in.";
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kTang (gunichar uc)
{
  return "This feature was not compiled in.";
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kKorean (gunichar uc)
{
  return "This feature was not compiled in.";
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kJapaneseKun (gunichar uc)
{
  return "This feature was not compiled in.";
}

G_CONST_RETURN gchar * 
gucharmap_get_unicode_kJapaneseOn (gunichar uc)
{
  return "This feature was not compiled in.";
}

#endif /* #else (#if ENABLE_UNIHAN) */


/* does a binary search; also caches most recent, since it will often be
 * called in succession on the same character */
static G_CONST_RETURN NamesList *
get_nameslist (gunichar uc)
{
  static gunichar most_recent_searched;
  static const NamesList *most_recent_result;
  gint min = 0;
  gint mid;
  gint max = sizeof (names_list) / sizeof (NamesList) - 1;

  if (uc < names_list[0].index || uc > names_list[max].index)
    return NULL;

  if (uc == most_recent_searched)
    return most_recent_result;

  most_recent_searched = uc;

  while (max >= min) 
    {
      mid = (min + max) / 2;
      if (uc > names_list[mid].index)
        min = mid + 1;
      else if (uc < names_list[mid].index)
        max = mid - 1;
      else
        {
          most_recent_result = names_list + mid;
          return names_list + mid;
        }
    }

  most_recent_result = NULL;
  return NULL;
}


gboolean
_gucharmap_unicode_has_nameslist_entry (gunichar uc)
{
  return get_nameslist (uc) != NULL;
}

/* returns newly allocated array of gunichar terminated with -1 */
gunichar *
gucharmap_get_nameslist_exes (gunichar uc)
{
  const NamesList *nl;
  gunichar *exes;
  gint i, count;
  
  nl = get_nameslist (uc);

  if (nl == NULL || nl->exes_index == -1)
    return NULL;

  /* count the number of exes */
  for (i = 0;  names_list_exes[nl->exes_index + i].index == uc;  i++);
  count = i;

  exes = g_malloc ((count + 1) * sizeof (gunichar));
  for (i = 0;  i < count;  i++)
    exes[i] = names_list_exes[nl->exes_index + i].value;
  exes[count] = (gunichar)(-1);

  return exes;
}


/* returns newly allocated null-terminated array of gchar* */
/* the items are const, but the array should be freed by the caller */
G_CONST_RETURN gchar **
gucharmap_get_nameslist_equals (gunichar uc)
{
  const NamesList *nl;
  const gchar **equals;
  gint i, count;
  
  nl = get_nameslist (uc);

  if (nl == NULL || nl->equals_index == -1)
    return NULL;

  /* count the number of equals */
  for (i = 0;  names_list_equals[nl->equals_index + i].index == uc;  i++);
  count = i;

  equals = g_malloc ((count + 1) * sizeof (gchar *));
  for (i = 0;  i < count;  i++)
    equals[i] = names_list_equals[nl->equals_index + i].value;
  equals[count] = NULL;

  return equals;
}


/* returns newly allocated null-terminated array of gchar* */
/* the items are const, but the array should be freed by the caller */
G_CONST_RETURN gchar **
gucharmap_get_nameslist_stars (gunichar uc)
{
  const NamesList *nl;
  const gchar **stars;
  gint i, count;

  nl = get_nameslist (uc);

  if (nl == NULL || nl->stars_index == -1)
    return NULL;

  /* count the number of stars */
  for (i = 0;  names_list_stars[nl->stars_index + i].index == uc;  i++);
  count = i;

  stars = g_malloc ((count + 1) * sizeof (gchar *));
  for (i = 0;  i < count;  i++)
    stars[i] = names_list_stars[nl->stars_index + i].value;
  stars[count] = NULL;

  return stars;
}


/* returns newly allocated null-terminated array of gchar* */
/* the items are const, but the array should be freed by the caller */
G_CONST_RETURN gchar **
gucharmap_get_nameslist_pounds (gunichar uc)
{
  const NamesList *nl;
  const gchar **pounds;
  gint i, count;
  
  nl = get_nameslist (uc);

  if (nl == NULL || nl->pounds_index == -1)
    return NULL;

  /* count the number of pounds */
  for (i = 0;  names_list_pounds[nl->pounds_index + i].index == uc;  i++);
  count = i;

  pounds = g_malloc ((count + 1) * sizeof (gchar *));
  for (i = 0;  i < count;  i++)
    pounds[i] = names_list_pounds[nl->pounds_index + i].value;
  pounds[count] = NULL;

  return pounds;
}


/* returns newly allocated null-terminated array of gchar* */
/* the items are const, but the array should be freed by the caller */
G_CONST_RETURN gchar **
gucharmap_get_nameslist_colons (gunichar uc)
{
  const NamesList *nl;
  const gchar **colons;
  gint i, count;
  
  nl = get_nameslist (uc);

  if (nl == NULL || nl->colons_index == -1)
    return NULL;

  /* count the number of colons */
  for (i = 0;  names_list_colons[nl->colons_index + i].index == uc;  i++);
  count = i;

  colons = g_malloc ((count + 1) * sizeof (gchar *));
  for (i = 0;  i < count;  i++)
    colons[i] = names_list_colons[nl->colons_index + i].value;
  colons[count] = NULL;

  return colons;
}


#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000 &&                     \
    ((Char) < 0xD800 || (Char) >= 0xE000) &&  \
    ((Char) < 0xFDD0 || (Char) > 0xFDEF) &&   \
    ((Char) & 0xFFFF) != 0xFFFE && ((Char) & 0xFFFF) != 0xFFFF)

/* an up-to-date replacement for g_unichar_validate */
gboolean
gucharmap_unichar_validate (gunichar ch)
{
  return UNICODE_VALID (ch);
}


/**
 * gucharmap_unichar_to_printable_utf8
 * @uc: a unicode character 
 * @outbuf: output buffer, must have at least 10 bytes of space.
 *          If %NULL, the length will be computed and returned
 *          and nothing will be written to @outbuf.
 *
 * Converts a single character to UTF-8 suitable for rendering. Check the
 * source to see what this means. ;-)
 * 
 *
 * Return value: number of bytes written
 **/
gint
gucharmap_unichar_to_printable_utf8 (gunichar uc, gchar *outbuf)
{
  /* Unicode Standard 3.2, section 2.6, "By convention, diacritical marks
   * used by the Unicode Standard may be exhibited in (apparent) isolation
   * by applying them to U+0020 SPACE or to U+00A0 NO BREAK SPACE." */

  /* 17:10 < owen> noah: I'm *not* claiming that what Pango does currently
   *               is right, but convention isn't a requirement. I think
   *               it's probably better to do the Uniscribe thing and put
   *               the lone combining mark on a dummy character and require
   *               ZWJ
   * 17:11 < noah> owen: do you mean that i should put a ZWJ in there, or
   *               that pango will do that?
   * 17:11 < owen> noah: I mean, you should (assuming some future more
   *               capable version of Pango) put it in there
   */

  if (! gucharmap_unichar_validate (uc) || (! gucharmap_unichar_isgraph (uc) 
      && gucharmap_unichar_type (uc) != G_UNICODE_PRIVATE_USE))
    return 0;
  else if (gucharmap_unichar_type (uc) == G_UNICODE_COMBINING_MARK
      || gucharmap_unichar_type (uc) == G_UNICODE_ENCLOSING_MARK
      || gucharmap_unichar_type (uc) == G_UNICODE_NON_SPACING_MARK)
    {
      gint x;

      outbuf[0] = ' ';
      outbuf[1] = '\xe2'; /* ZERO */ 
      outbuf[2] = '\x80'; /* WIDTH */
      outbuf[3] = '\x8d'; /* JOINER (0x200D) */

      x = g_unichar_to_utf8 (uc, outbuf + 4);

      return x + 4;
    }
  else
    return g_unichar_to_utf8 (uc, outbuf);
}


/**
 * gucharmap_unichar_type:
 * @c: a Unicode character
 * 
 * Classifies a Unicode character by type.
 * 
 * Return value: the type of the character.
 **/
GUnicodeType
gucharmap_unichar_type (gunichar uc)
{
  gint min = 0;
  gint mid;
  gint max = sizeof (unicode_categories) / sizeof (UnicodeCategory) - 1;

  if (uc < unicode_categories[0].first || uc > unicode_categories[max].last)
    return G_UNICODE_UNASSIGNED;

  while (max >= min) 
    {
      mid = (min + max) / 2;
      if (uc > unicode_categories[mid].last)
        min = mid + 1;
      else if (uc < unicode_categories[mid].first)
        max = mid - 1;
      else
        return unicode_categories[mid].category;
    }

  return G_UNICODE_UNASSIGNED;
}


/**
 * gucharmap_unichar_isdefined:
 * @uc: a Unicode character
 * 
 * Determines if a given character is assigned in the Unicode
 * standard.
 *
 * Return value: %TRUE if the character has an assigned value
 **/
gboolean
gucharmap_unichar_isdefined (gunichar uc)
{
  return gucharmap_unichar_type (uc) != G_UNICODE_UNASSIGNED;
}


/**
 * gucharmap_unichar_isgraph:
 * @uc: a Unicode character
 * 
 * Determines whether a character is printable and not a space
 * (returns %FALSE for control characters, format characters, and
 * spaces). g_unichar_isprint() is similar, but returns %TRUE for
 * spaces. Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 * 
 * Return value: %TRUE if @c is printable unless it's a space
 **/
gboolean
gucharmap_unichar_isgraph (gunichar uc)
{
  GUnicodeType t = gucharmap_unichar_type (uc);

  return (t != G_UNICODE_CONTROL
          && t != G_UNICODE_FORMAT
          && t != G_UNICODE_UNASSIGNED
          && t != G_UNICODE_PRIVATE_USE
          && t != G_UNICODE_SURROGATE
          && t != G_UNICODE_SPACE_SEPARATOR);
}

