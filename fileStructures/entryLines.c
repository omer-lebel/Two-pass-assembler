/* ------------------------------- includes ------------------------------- */
#include "entryLines.h"
#include "../utils/vector.h"
/* ---------------------- helper function declaration ---------------------- */
void*   clone_entry_line  (void *elem);
void    free_entry_line   (void *elem);
void    print_entry_line  (const void *elem, FILE *stream);
/* ------------------------------------------------------------------------- */

/**
 * @struct EntryLinesList
 * @brief Internal structure representing the entry lines list,
 * using a vector
 */
struct EntryLinesList
{
    vector *lines;
};

EntryLinesList *new_entry_lines_list (void)
{
  EntryLinesList *entry_list = (EntryLinesList *) malloc (sizeof (EntryLinesList));
  if (entry_list) {
    /* Initialize the vector */
    entry_list->lines = create_vector (sizeof (EntryLine), clone_entry_line,
                                       free_entry_line);
    if (!entry_list->lines) {
      free (entry_list);
      return NULL;
    }
  }
  return entry_list;
}

void *clone_entry_line (void *elem)
{
  EntryLine *entry_line = (EntryLine *) elem;
  LineParts *tmp_part;

  if (entry_line->parts) { /*symbol is unresolved yet */
    tmp_part = malloc (sizeof (LineParts));
    if (!tmp_part) {
      return NULL;
    }
    *tmp_part = *entry_line->parts;
    entry_line->parts = tmp_part;
  }
  return entry_line;
}

void free_entry_line (void *elem)
{
  EntryLine *entry_line = (EntryLine *) elem;
  free (entry_line->parts);
}

Bool is_empty (EntryLinesList *entry_list)
{
  return entry_list->lines->size == 0;
}

EntryLine *
add_to_entry_lines_list (EntryLinesList *entry_list, Symbol *symbol,
                         LineParts *line_part)
{
  EntryLine entry_line;
  entry_line.symbol = symbol;
  entry_line.parts = line_part;
  /* the push create a copy of the entry_line */
  return push (entry_list->lines, &entry_line);
}

EntryLine *get_entry_line (EntryLinesList *entry_list, int i)
{
  return get (entry_list->lines, i);
}

void print_entry_list (EntryLinesList *entry_list, FILE *stream)
{
  print_vector (entry_list->lines, print_entry_line, stream, "\n", "\n");
}

void print_entry_line (const void *elem, FILE *stream)
{
  EntryLine *entry_line = (EntryLine *) elem;
  Symbol_Data *symbol_data = entry_line->symbol->data;
  fprintf (stream, "%s\t%04u", entry_line->symbol->word, symbol_data->val);
}

void free_entry_lines_list (EntryLinesList *entry_list)
{
  free_vector (entry_list->lines);
  free (entry_list);
}