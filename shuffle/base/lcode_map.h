/* Mapping binary operators to other enumerators */
#define opcode_to_tms(op) cast(TMS, ((op) - OP_ADD) + TM_ADD)
#define binopr_to_tms(opr) cast(TMS, ((opr) - OPR_ADD) + TM_ADD)
#define binopr_to_luaop(opr) (((opr) - OPR_ADD) + LUA_OPADD)
#define unopr_to_luaop(op) (((op) - OPR_MINUS) + LUA_OPUNM)
#define binopr_to_kopcode(opr) cast(OpCode, ((opr) - OPR_ADD) + OP_ADDK)
#define binopr_to_opcode(opr) cast(OpCode, ((opr) - OPR_ADD) + OP_ADD)
#define unopr_to_opcode(op) cast(OpCode, ((op) - OPR_MINUS) + OP_UNM)

#define cmpbinopr_to_opcode(opr) cast(OpCode, ((opr) - OPR_EQ) + OP_EQ)
#define ncmpbinopr_to_opcode(opr) cast(OpCode, ((opr) - OPR_NE) + OP_EQ)
#define cmpbinopr_to_iopcode(op) cast(OpCode, ((op) - OP_LT) + OP_LTI)
