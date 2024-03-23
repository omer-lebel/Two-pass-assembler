/*
 Created by OMER on 1/24/2024.
*/
#include "symbolTable.h"

Symbol_Table *new_symbol_table (void)
{
  Symbol_Table *table = malloc (sizeof (Symbol_Table));
  if (!table) {
    return NULL;
  }
  table->database = createList (init_symbol_data, print_symbol, free);
  if (!table->database) {
    free (table);
    return NULL;
  }
  table->extern_count = 0;
  table->unresolved_usage_count = 0;
  table->unresolved_entry_count = 0;

  return table;
}

void *init_symbol_data (const void *data)
{
  Symbol_Data *new_data = malloc (sizeof (Symbol_Data));
  if (!new_data) {
    return NULL;
  }
  memcpy (new_data, data, sizeof (Symbol_Data));
  return new_data;
}

Symbol *add_symbol (Symbol_Table *table, const char *label,
                    SymbolType type, int val, EntryFlag isEntry)
{
  Symbol_Data symbol_data;
  Symbol *new_symbol;

  symbol_data.type = type;
  symbol_data.val = val;
  symbol_data.isEntry = isEntry;

  new_symbol = createNode (table->database, label, &symbol_data);
  if (!new_symbol) {
    return NULL;
  }
  appendSorted(table->database, new_symbol);
  return new_symbol;
}

Symbol *find_symbol (Symbol_Table *table, const char *name)
{
  return findNode (table->database, name);
}

void print_symbol (const char *word, const void *data, FILE *stream)
{
  Symbol_Data *symbol_data = (Symbol_Data *) data;

  fprintf (stream, " %-15s  %-5d ", word, symbol_data->val);
  switch (symbol_data->type) {
    case DATA:
      fprintf (stream, " %-15s", "directive");
      break;
    case CODE:
      fprintf (stream, " %-15s", "code");
      break;
    case DEFINE:
      fprintf (stream, " %-15s", "define");
      break;
    case EXTERN:
      fprintf (stream, " %-15s", "external");
      break;
    case UNRESOLVED_USAGE:
      fprintf (stream, " %-15s", "unresolved usage");
      break;
    case UNRESOLVED_ENTRY:
      fprintf (stream, " %-15s", "unresolved entry");
      break;
    case UNRESOLVED_ENTRY_USAGE:
      fprintf (stream, " %-15s", "unresolved entry usage");
      break;
  }

  fprintf (stream, "\t\t%-15s", (symbol_data->isEntry ?
                                 "entry" : "not entry"));

  fprintf (stream, "\n");

}

void show_symbol_table (Symbol_Table *table, FILE *stream)
{
  fprintf (stream, "\n----------------- symbol table -----------------\n");
  fprintf (stream, "extern: %d\n", table->extern_count);
  fprintf (stream, "unresolved usage: %d\n", table->unresolved_usage_count);
  fprintf (stream, "unresolved entry: %d\n", table->unresolved_entry_count);
  printList (table->database, stream);
}

void free_symbol_table (Symbol_Table *table)
{
  freeList (table->database);
  free (table);
}

void update_data_symbol_addresses (Symbol_Table *symbol_table, int IC)
{
  Symbol_Data *symbol;
  Node *node = symbol_table->database->head;
  while (node) {
    symbol = (Symbol_Data *) node->data;
    if (symbol->type == CODE) {
      symbol->val += IC_START;
    }
    if (symbol->type == DATA) {
      symbol->val += IC_START + IC;
    }
    node = node->next;
  }
}