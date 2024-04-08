#include "entryLines.h"

void *add_line_to_entry_list (void *elem);
void free_line_in_entry_list (void *elem);
void print_entry_line (const void *elem, FILE *stream);


Entry_List *new_entry_list (void)
{
  return create_vector (sizeof (Entry_line), add_line_to_entry_list,
                        free_line_in_entry_list);
}

void *add_line_to_entry_list (void *elem)
{
  Entry_line *entry_line = (Entry_line *) elem;
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

void free_line_in_entry_list (void *elem)
{
  Entry_line *entry_line = (Entry_line *) elem;
  free (entry_line->parts);
}

Entry_line *add_to_entry_list (Entry_List *entry_list, Symbol *symbol,
                               LineParts *line_part){
  Entry_line entry_line;
  entry_line.symbol = symbol;
  entry_line.parts = line_part;
  /* the push create a copy of the entry_line */
  return push (entry_list, &entry_line);
}

void print_entry_list(Entry_List *entry_list, FILE *stream){
  print_vector (entry_list, print_entry_line, stream, "\n", "\n");
}

void print_entry_line (const void *elem, FILE *stream)
{
  Entry_line *entry_line = (Entry_line *) elem;
  Symbol_Data *symbol_data = entry_line->symbol->data;
  fprintf (stream, "%s\t%04u", entry_line->symbol->word, symbol_data->val);
}

void free_entry_list (Entry_List *entry_list){
  free_vector (entry_list);
}