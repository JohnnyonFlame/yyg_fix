#ifndef __YYG_FIX_H__
#define __YYG_FIX_H__

// Defines courtesy of https://github.com/nkrapivin/libLassebq
#define MASK_KIND_RVALUE 0x0ffffff
typedef enum RValueType {
	VALUE_REAL = 0,
	VALUE_STRING = 1,
	VALUE_ARRAY = 2,
	VALUE_PTR = 3,
	VALUE_VEC3 = 4,
	VALUE_UNDEFINED = 5,
	VALUE_OBJECT = 6,
	VALUE_INT32 = 7,
	VALUE_VEC4 = 8,
	VALUE_MATRIX = 9,
	VALUE_INT64 = 10,
	VALUE_ACCESSOR = 11,
	VALUE_JSNULL = 12,
	VALUE_BOOL = 13,
	VALUE_ITERATOR = 14,
	VALUE_REF = 15,
	VALUE_UNSET = MASK_KIND_RVALUE
} RValueType;

typedef struct yyg_ref {
	void* m_thing;
	int m_refCount;
	int m_size;
} yyg_ref;

typedef struct yyg_rval {
	/* ... */
	union {
		int v32;
		long long v64;
		double val;
		/* pointers */

		yyg_ref *str;
	} rvalue;

	int flags;
	RValueType kind;
} yyg_rval;

typedef struct yyg_vdecl_t {
	char *name;
	yyg_rval *rval;
	void *unk1;
	void *unk2;
} yyg_vdecl_t;

typedef void (*yyg_routine_t)(yyg_rval *ret, void *self, void *other, int argc, yyg_rval *args);

// Offsets from the yyg runner
const void (*yyg_define_builtin)(const char *id, yyg_routine_t func, int, int)     = (void(*))0x000258f8;
const void (*yyg_assure_str)(yyg_rval *var_table, int slot, void *other, int argc) = (void(*))0x00026584;

// const int          *yyg_builtin_cnt          = 0x0041b424;
// const void         *yyg_builtin_registry     = 0x0041b428;
const int          *yyg_builtin_var_count    = (int*)          0x0063f330;
const yyg_vdecl_t  *yyg_builtin_var_registry = (yyg_vdecl_t  *)0x0063f334;
const int          *yyg_var_count            = (int*)          0x0041b458;
const yyg_vdecl_t  *yyg_var_registry         = (yyg_vdecl_t  *)0x0041b45c;

#endif /* __YYG_FIX_H__ */