/*
** Mask with 1 in all fast-access methods. A 1 in any of these bits
** in the flag of a (meta)table means the metatable does not have the
** corresponding metamethod field. (Bit 7 of the flag is used for
** 'isrealasize'.)
*/
#define maskflags (~(~0u << (5 + 1)))

/* 1<<p means tagmethod(p) is not present */
#define tagmethod(p) cast_byte(1u << (p))
