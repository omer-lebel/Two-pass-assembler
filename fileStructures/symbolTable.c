/*
 Created by OMER on 1/24/2024.
*/


#include "symbolTable.h"


LinkedList* init_symbol_table(void){
  return createList (init_symbol, print_symbol, free);
}

void *init_symbol (const void *data)
{
  Symbol *symbol_data = (Symbol *) data;
  Symbol *new_symbol = (Symbol *) malloc (sizeof (Symbol));
  if (!new_symbol) {
    return NULL; /* memory error */
  }
  new_symbol->address = symbol_data->address;
  new_symbol->are = symbol_data->are;
  new_symbol->type = symbol_data->type;
  new_symbol->isEntry = symbol_data->isEntry;
  new_symbol->val = symbol_data->val;
  return new_symbol;
}

void print_symbol (const char *word, const void *data, FILE *pf)
{
  Symbol *symbol_data = (Symbol *) data;

  fprintf (pf, " %-15s  %-5lu ", word, symbol_data->address);
  switch (symbol_data->type) {
    case DATA:
      fprintf (pf, " %-15s", "directive");
      break;
    case OPERATION:
      fprintf (pf, " %-15s", "operation");
      break;
    case DEFINE:
      fprintf (pf, " %-15s", "define");
      break;
    default:
      fprintf (pf, " %-15s", "unknown");
  }

  fprintf (pf, " %-15s", (symbol_data->are == EXTERNAL ?
                          "external" : "not external"));

  if (symbol_data->type == DEFINE){
    fprintf (pf, " %-5d", symbol_data->val);
  }
  fprintf (pf, "\n");

}

exit_code add_symbol (LinkedList *symbol_table, const char *label,
                      SymbolType type, size_t address, ARE are, int val)
{
  Symbol symbol_data;
  Node *new_symbol;

  symbol_data.address = address;
  symbol_data.type = type;
  symbol_data.are = are;
  symbol_data.isEntry = FALSE;
  symbol_data.val = val;

  new_symbol = createNode (symbol_table, label, &symbol_data);
  if (!new_symbol) {
    return MEMORY_ERROR; /* memory error */
  }
  appendToTail (symbol_table, new_symbol);
  return SUCCESS;
}

void update_data_symbol_addresses(LinkedList *symbol_table, size_t IC)
{
  Symbol *symbol;
  Node *node = symbol_table->head;
  while (node)
  {
    symbol = (Symbol*) node->data;
    if (symbol->type != DEFINE && symbol->are != EXTERNAL){
      symbol->address += INIT_IC;
      if (symbol->type == DATA){
        symbol->address += IC;
      }
    }

    node = node->next;
  }
}