/* ORDER TM: Used to simplify logic in ltm.c: luaT_trybinTM */
#define tmfast(o) (TM_INDEX <= (o) && (o) <= TM_EQ)

/*
** Unfolded tmfast:
** #define tmfast(o) (                                        \
**   TM_INDEX == (o) || TM_NEWINDEX == (o) || TM_GC == (o) || \
**   TM_MODE == (o) || TM_LEN == (o) || TM_EQ == (o)          \
** )
*/
