/*
 Created by OMER on 1/24/2024.
*/


#include "symbolTable.h"

LinkedList *symbols_table = NULL;

exit_code init_symbol_table(){
  symbols_table = createList (init_symbol, print_symbol, free);
  if (!symbols_table) {
    return FAILURE; /* memory error */
  }
  else{
    return SUCCESS;
  }
}

void *init_symbol (const void *data)
{
  Symbol *symbol_data = (Symbol *) data;
  Symbol *new_symbol = (Symbol *) malloc (sizeof (Symbol));
  if (!new_symbol) {
    return NULL; /* memory error */
  }
  new_symbol->address = symbol_data->address;
  new_symbol->isExtern = symbol_data->isExtern;
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
    case DIRECTIVE:
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

  fprintf (pf, " %-15s", (symbol_data->isExtern == EXTERNAL ?
                          "external" : "not external"));

  if (symbol_data->type == DEFINE){
    fprintf (pf, " %-5d", symbol_data->val);
  }
  fprintf (pf, "\n");

}

exit_code add_symbol (const char *label, SymbolType type, size_t address, int
isExtern, int val)
{
  Symbol symbol_data;
  Node *new_symbol;

  symbol_data.address = address;
  symbol_data.type = type;
  symbol_data.isExtern = isExtern;
  symbol_data.isEntry = FALSE;
  symbol_data.val = val;

  new_symbol = createNode (symbols_table, label, &symbol_data);
  if (!new_symbol) {
    return FAILURE; /* memory error */
  }
  appendToTail (symbols_table, new_symbol);
  return SUCCESS;
}