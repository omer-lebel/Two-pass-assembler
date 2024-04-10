
#include "codeSeg.h"

/* ---------------------- helper function declaration ---------------------- */

/* Helper functions for Op_Analyze */
void init_operand (Operand *operand, Opcode opcode, Operand_Type type, char *sym_buffer);
void display_operand_info (Operand *operand, FILE *stream);

/* Helper functions for Op_List */
void *init_line_in_op_list (void *elem);
void free_line_in_op_list (void *elem);

/* Helper functions for printing code segment */
void add_op_line_to_code_segment (Op_Analyze *op, int *memInx, int len, FILE *stream);
void print_operand_machine_word (Operand *operand, int *memInx, int len, FILE *stream);

/* ====================================================================
 *                              op analyze
 * ==================================================================== */

void init_op_analyze (Op_Analyze *op, Opcode opcode, char *scr_sym_buffer,
                      char *target_sym_buffer)
{
  op->opcode = opcode;
  init_operand (&op->src, opcode, SRC, scr_sym_buffer);
  init_operand (&op->target, opcode, TARGET, target_sym_buffer);
}

/**
 * @brief Initializes an operand structure with the provided parameters.
 *
 * @param operand   Pointer to the operand structure to initialize.
 * @param opcode    Opcode of the operation.
 * @param type      Type of the operand (SRC or TARGET).
 * @param sym_buffer Buffer to hold the name of the symbol used in the
 *                   operand in case it is unresolved.
 */
void init_operand (Operand *operand, Opcode opcode, Operand_Type type,
                   char *sym_buffer)
{
  operand->type = type;
  operand->add_mode = NONE_ADD;
  operand->param_types = param_types[opcode][type];
  operand->info.symInx.symbol = sym_buffer;
  operand->info.symInx.found = TRUE;
}

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

void print_op_analyze(Op_Analyze *op, FILE *stream)
{
  fprintf (stream, "%04d\t", op->address);
  fprintf (stream, "<op: %s>\t", op_names[op->opcode]);
  fprintf (stream, "<opcode: %d>\t", op->opcode);
  display_operand_info (&(op->src), stream);
  display_operand_info (&(op->target), stream);
}

/**
 * @brief Displays the information of an operand in a human-readable format.
 *
 * @param operand   Pointer to the operand structure to display.
 * @param stream    File stream to which the information will be written.
 */
void display_operand_info (Operand *operand, FILE *stream)
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

/* ====================================================================
 *                                op line
 * ==================================================================== */

Op_List *new_op_list (void)
{
  return create_vector (sizeof (Op_Line), init_line_in_op_list,
                        free_line_in_op_list);
}

/**
 * @brief Helper function of the Op_list that init an operation line when it
 * added to the operation list, by cloning its fields.
 *
 * @param elem  Pointer to the operation line element to add.
 * @return      Pointer to the added operation line.
 */
void *init_line_in_op_list (void *elem)
{
  Op_Line *op_line = (Op_Line *) elem;
  Op_Analyze *tmp_op;
  LineParts *tmp_part;

  /* deep copy of op analyze */
  tmp_op = malloc (sizeof (Op_Analyze));
  if (!tmp_op) {
    return NULL;
  }
  *tmp_op = *op_line->analyze;
  op_line->analyze = tmp_op;

  /* deep copy of line part */
  tmp_part = malloc (sizeof (LineParts));
  if (!tmp_part) {
    free (tmp_op);
    return NULL;
  }
  *tmp_part = *op_line->parts;
  op_line->parts = tmp_part;
  op_line->parts->prefix = NULL;
  op_line->parts->token = NULL;
  op_line->parts->postfix = NULL;

  return op_line;
}

/**
 * @brief Frees memory allocated for an operation line in the operation list.
 *
 * @param elem Pointer to the operation line element to free.
 */
void free_line_in_op_list (void *elem)
{
  Op_Line *op_line = (Op_Line *) elem;
  free (op_line->analyze);
  free (op_line->parts);
}


Op_Line *add_to_op_list (Op_List *op_list, Op_Analyze *op_analyze,
                         LineParts *line_part){
  Op_Line op_line;
  op_line.analyze = op_analyze;
  op_line.analyze->address += IC_START;
  op_line.parts = line_part;
  return push (op_list, &op_line);
}

void display_op_list(Op_List *op_list, FILE *stream){
  fprintf (stream, "\n------------------ op list ------------------\n");
  print_vector (op_list, display_op_line, stream, "\n", "\n");
}

void display_op_line (const void *op_line, FILE *stream)
{
  Op_Analyze *op = ((Op_Line *) op_line)->analyze;
  print_op_analyze(op, stream);
}

void free_op_list (Op_List *op_list){
  free_vector (op_list);
}

/* ====================================================================
 *                             code segment
 * ==================================================================== */

void print_code_segment (Op_List *op_list, int memInx, int len, FILE *stream)
{
  Op_Line *op_line;
  int i = 0;
  for (; memInx < len; ++i) {
    op_line = (Op_Line *) get (op_list, i);
    add_op_line_to_code_segment (op_line->analyze, &memInx, len, stream);
  }
}

/**
 * @brief Adds an operation line to the code segment.
 *
 * @param op        Pointer to the operation to add to the code segment.
 * @param memInx    Pointer to the memory index.
 * @param len       Length of the code segment.
 * @param stream    File stream to which the code segment will be written.
 *                  (typicality .ob file)
 */
void add_op_line_to_code_segment (Op_Analyze *op, int *memInx, int len,
                                  FILE *stream)
{
  unsigned short int word, scr_code, target_code;
  /* first word */
  scr_code = (op->src.add_mode == NONE_ADD) ? 0 : op->src.add_mode;
  target_code = (op->target.add_mode == NONE_ADD) ? 0 : op->target.add_mode;
  word = first_word (op->opcode, scr_code, target_code);
  fprintf (stream, "%04d ", *memInx);
  print_special_base_word (word, (len - *memInx == 1), stream);
  (*memInx)++;

  /*  both operand are registers, so they share the second word */
  if ((op->src.add_mode == REG_ADD) && (op->target.add_mode == REG_ADD)) {
    word = registers_word (op->src.info.reg_num, op->target.info.reg_num);
    fprintf (stream, "%04d ", *memInx);
    print_special_base_word (word, (len - *memInx == 1), stream);
    (*memInx)++;
  }

  else {
    /* src word */
    if ((op->src.add_mode != NONE_ADD)) {
      print_operand_machine_word (&op->src, memInx, len, stream);
    }
    /* target word */
    if ((op->target.add_mode != NONE_ADD)) {
      print_operand_machine_word (&op->target, memInx, len, stream);
    }
  }
}

/**
 * @brief Prints the machine word representation of an operand.
 *
 * @param operand Pointer to the operand structure.
 * @param memInx Pointer to the memory index.
 * @param len Length of the code segment.
 * @param stream File stream to which the information will be written.
 *
 * @note doesn't take care of case that both src and target are registers
 */
void print_operand_machine_word (Operand *operand, int *memInx, int len, FILE *stream)
{
  unsigned short int word = 0;
  Symbol_Data *symbol;
  int are;

  switch (operand->add_mode) {
    case IMM_ADD:
      word = imm_word (operand->info.imm);
      break;
    case DIRECT_ADD:
      symbol = (Symbol_Data *) ((Symbol *) operand->info.symInx.symbol)->data;
      are = symbol->type == EXTERN ? external_b : relocatable_b;
      word = label_word (symbol->val, are);
      break;
    case INDEX_ADD:
      symbol = (Symbol_Data *) ((Symbol *) operand->info.symInx.symbol)->data;
      are = symbol->type == EXTERN ? external_b : relocatable_b;
      word = label_word (symbol->val, are);
      fprintf (stream, "%04d ", *memInx); /* print the label */
      print_special_base_word (word, (len - *memInx == 1), stream);
      (*memInx)++;
      word = imm_word (operand->info.symInx.offset); /* add the index */
      break;
    case REG_ADD:
      if (operand->type == SRC)
        word = registers_word (operand->info.reg_num, 0);
      else /* TARGET */
        word = registers_word (0, operand->info.reg_num);
      break;
    case NONE_ADD: /* never happened */
      return;
  }
  fprintf (stream, "%04d ", *memInx);
  print_special_base_word (word, (len - *memInx == 1), stream);
  (*memInx)++;
}