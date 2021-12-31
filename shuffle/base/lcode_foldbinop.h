/* true if operation is foldable (that is, it is arithmetic or bitwise) */
#define foldbinop(op) (OPR_ADD <= (op) && (op) <= OPR_SHR)

/*
** Unfolded-foldbinop
** #define foldbinop(op) (                                                       \
**   OPR_ADD == (op) || OPR_SUB == (op) || OPR_MUL == (op) || OPR_MOD == (op) || \
**   OPR_POW == (op) || OPR_DIV == (op) || OPR_IDIV == (op) ||                   \
**   OPR_BAND == (op) || OPR_BOR == (op) || OPR_BXOR == (op) ||                  \
**   OPR_SHL == (op) || OPR_SHR == (op)                                          \
** )
*/
