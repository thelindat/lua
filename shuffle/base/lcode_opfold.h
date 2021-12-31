/* true if opcode is foldable (that is, it is arithmetic or bitwise) */
#define luaop_fold(o) (OP_ADD <= (o) && (o) <= OP_SHR)

/*
** Unfolded foldop:
** #define luaop_fold(o) (                                                 \
**   OP_ADD == (o) || OP_SUB == (o) || OP_MUL == (o) || OP_MOD == (o) ||   \
**   OP_POW == (o) || OP_DIV == (o) || OP_IDIV == (o) || OP_BAND == (o) || \
**   OP_BOR == (o) || OP_BXOR == (o) || OP_SHL == (o) || OP_SHR == (o)     \
** )
*/
