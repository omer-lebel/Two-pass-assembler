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
  new_symbol->val = symbol_data->val;
  new_symbol->type = symbol_data->type;
  new_symbol->isEntry = symbol_data->isEntry;
  return new_symbol;
}

void print_symbol (const char *word, const void *data, FILE *pf)
{
  Symbol *symbol_data = (Symbol *) data;

  fprintf (pf, " %-15s  %-5d ", word, symbol_data->val);
  switch (symbol_data->type) {
    case DATA:
      fprintf (pf, " %-15s", "directive");
      break;
    case CODE:
      fprintf (pf, " %-15s", "operation");
      break;
    case DEFINE:
      fprintf (pf, " %-15s", "define");
      break;
    case EXTERN:
      fprintf (pf, " %-15s", "external");
      break;
    case UNRESOLVED:
      fprintf (pf, " %-15s", "unresolved");
      break;
  }

  fprintf (pf, " %-15s", (symbol_data->isEntry ?
                          "entry" : "not entry"));

  fprintf (pf, "\n");

}

Node* add_symbol (LinkedList *symbol_table, const char *label,
                      SymbolType type, int val, Bool isEntry)
{
  Symbol symbol_data;
  Node *new_symbol;

  symbol_data.type = type;
  symbol_data.val = val;
  symbol_data.isEntry = isEntry;

  new_symbol = createNode (symbol_table, label, &symbol_data);
  if (!new_symbol) {
    return NULL; /* memory error */
  }
  appendToTail(symbol_table, new_symbol); /* todo sorted? */
  return new_symbol;
}

void update_data_symbol_addresses(LinkedList *symbol_table, int IC)
{
  Symbol *symbol;
  Node *node = symbol_table->head;
  while (node)
  {
    symbol = (Symbol*) node->data;
    if (symbol->type == CODE){
      symbol->val += IC_START;
      if (symbol->type == DATA){
        symbol->val += IC_START + IC;
      }
    }

    node = node->next;
  }
}