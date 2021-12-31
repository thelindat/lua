#define luaop_to_tms(op) cast(TMS, ((op) - LUA_OPADD) + TM_ADD)
