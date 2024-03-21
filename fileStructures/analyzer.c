/*
 Created by OMER on 3/6/2024.
*/


#include "analyzer.h"

#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

/************************* op analyze ***************************/

void init_operand (Operand *operand, Opcode opcode, Operand_Type type,
                   char *sym_buffer)
{
  operand->type = type;
  operand->add_mode = NONE_ADD;
  operand->param_types = param_types[opcode][type];
  operand->info.symInx.symbol = sym_buffer;
  operand->info.symInx.found = TRUE;
}

/* in h */
void init_op_analyze (Op_Analyze *op, Opcode opcode, char *scr_sym_buffer,
                      char *target_sym_buffer)
{
  op->opcode = opcode;
  init_operand (&op->src, opcode, SRC, scr_sym_buffer);
  init_operand (&op->target, opcode, TARGET, target_sym_buffer);
}

void print_data (int *arr, unsigned len)
{
  unsigned int i;
  printf (CYN "data: " RESET "%d", arr[0]);
  for (i = 1; i < len; ++i) {
    printf (", %d", arr[i]);
  }
  printf ("\n");
}

void print_operand (Operand *operand, FILE *stream)
{
  switch (operand->add_mode) {
    case IMM_ADD:
      fprintf (stream, "<imm: %d>\t", operand->info.imm);
      break;
    case DIRECT_ADD:
      fprintf (stream, "<symbol: %s>\t",
               ((Symbol *) operand->info.symInx.symbol)->word);
      break;
    case INDEX_ADD:
      fprintf (stream, "<index: %s[%d]>\t",
               ((Symbol *) operand->info.symInx.symbol)->word,
               operand->info.symInx.offset);
      break;
    case REG_ADD:
      fprintf (stream, "<reg: r%d>\t", operand->info.reg_num);
      break;
    case NONE_ADD:
      fprintf (stream, "           \t");
      break;
  }
}

void print_op_analyze(Op_Analyze *op, FILE *stream)
{
  fprintf (stream, "%04d\t", op->address);
  fprintf (stream, "<op: %s>\t", op_names[op->opcode]);
  fprintf (stream, "<opcode: %d>\t", op->opcode);
  print_operand (&(op->src), stream);
  print_operand (&(op->target), stream);
}

void print_op_line (const void *op_line, FILE *stream)
{
  Op_Analyze *op = ((Op_Line *) op_line)->analyze;
  print_op_analyze(op, stream);
}

/* in h*/
int calc_op_size (Op_Analyze *op)
{
  int res = 1; /*for the first word */
  Addressing_Mode mode;

  /*special case of 2 registers that share the same word */
  if (op->src.add_mode == REG_ADD && op->target.add_mode == REG_ADD) {
    res += 1;
  }
  else {
    /*word for src operand */
    mode = op->src.add_mode;
    if (mode != NONE_ADD) {
      res += (mode == INDEX_ADD ? 2 : 1);
    }
    /*word for target operand */
    mode = op->target.add_mode;
    if (mode != NONE_ADD) {
      res += (mode == INDEX_ADD ? 2 : 1);
    }
  }
  return res;
}

/************************* line info ***************************/


void print_line_info (LineInfo *line, char *file_name)
{
  printf ("%s:%-2lu ", file_name, line->parts->num);

  switch (line->type_l) {
    case str_l:
      printf (CYN "string: " RESET "%s\n", line->info.str.content);
      break;
    case data_l:
      print_data (line->info.data.arr, line->info.data.len);
      break;
    case ext_l:
      printf (CYN "extern: " RESET "%s\n",
              ((Symbol *) line->info.ext_ent.name)->word);
      break;
    case ent_l:
      printf (CYN "entry: " RESET "%s\n",
              ((Symbol *) line->info.ext_ent.name)->word);
      break;
    case op_l:
      print_op_line(line->info.op, stdout);
      break;
    case def_l:
      printf (CYN "define:" RESET " %s=%d\n",
              ((Symbol *) line->info.define.name)->word,
              line->info.define.val);
      break;
  }
}

/************************* op list ***************************/

void *add_line_to_op_list (void *elem)
{
  Op_Line *op_line = (Op_Line *) elem;
  Op_Analyze *tmp_op;
  LinePart *tmp_part;

  tmp_op = malloc (sizeof (Op_Analyze));
  if (!tmp_op) {
    return NULL;
  }

  tmp_part = malloc (sizeof (LinePart));
  if (!tmp_part) {
    free (tmp_op);
    return NULL;
  }

  *tmp_part = *op_line->line_part;
  *tmp_op = *op_line->analyze;
  op_line->analyze = tmp_op;
  op_line->line_part = tmp_part;

  return op_line;
}

void free_line_in_op_list (void *elem)
{
  Op_Line *op_line = (Op_Line *) elem;
  free (op_line->analyze);
  free (op_line->line_part);
}

/* in h*/
Op_List *new_op_list (void)
{
  return create_vector (sizeof (Op_Line), add_line_to_op_list,
                        free_line_in_op_list);
}


Op_Line *add_to_op_list (Op_List *op_list, Op_Analyze *op_analyze,
                         LinePart *line_part){
  Op_Line op_line;
  op_line.analyze = op_analyze;
  op_line.line_part = line_part;
  return push (op_list, &op_line);
}

void show_op_list(Op_List *op_list, FILE *stream){
  fprintf (stream, "------------------ op list ------------------\n");
  print_vector (op_list, print_op_line, stream, "\n", "\n");
}

void free_op_list (Op_List *op_list){
  free_vector (op_list);
}


/************************* entry list ***************************/

void *add_line_to_entry_list (void *elem)
{
  Entry_line *entry_line = (Entry_line *) elem;
  LinePart *tmp_part;

  if (entry_line->part) { /*symbol is unresolved yet */
    tmp_part = malloc (sizeof (LinePart));
    if (!tmp_part) {
      return NULL;
    }
    *tmp_part = *entry_line->part;
    entry_line->part = tmp_part;
  }
  return entry_line;
}

void print_entry_line (const void *elem, FILE *stream)
{
  Entry_line *entry_line = (Entry_line *) elem;
  Symbol_Data *symbol_data = entry_line->symbol->data;
  fprintf (stream, "%s\t%04u", entry_line->symbol->word, symbol_data->val);
}

void free_line_in_entry_list (void *elem)
{
  Entry_line *entry_line = (Entry_line *) elem;
  free (entry_line->part);
}

/* in h*/
Entry_List *new_entry_list (void)
{
  return create_vector (sizeof (Entry_line), add_line_to_entry_list,
                        free_line_in_entry_list);
}

Entry_line *add_to_entry_list (Entry_List *entry_list, Symbol *symbol,
                               LinePart *line_part){
  Entry_line entry_line;
  entry_line.symbol = symbol;
  entry_line.part = line_part;
  return push (entry_list, &entry_line);
}

void print_entry_list(Entry_List *entry_list, FILE *stream){
  print_vector (entry_list, print_entry_line, stream, "\n", "\n");
}

void free_entry_list (Entry_List *entry_list){
  free_vector (entry_list);
}