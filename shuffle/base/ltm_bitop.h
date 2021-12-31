#define tmbitop(o) ((TM_BAND <= (o) && (o) <= TM_SHR) || (o) == TM_BNOT)

/*
** Unfolded timbitop:
** #define tmbitop(o)  (                                                       \
**   TM_BAND == (o) || TM_BOR == (o) || TM_BXOR == (o) || TM_BNOT == (o) ||    \
**   TM_SHL == (o) || TM_SHR == (o)                                            \
** )
*/
