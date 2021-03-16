#define _GNU_SOURCE
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#include "yyg_fix.h"

// "open" LD_PRELOAD hook. This is necessary to reshuffle the joystick slots
// because joysticks on YYG's Linux runner are just slotted by /dev/input/jsXXX order,
// and we might want to remap this joystick because we have 200 different controller layouts.
typedef int (*dlnopen)(const char *pathname, int flags, ...);
int open(const char *pathname, int flags, ...)
{
	va_list args;
    va_start(args, flags);

	//fprintf(stderr, "open(%s, %d, ...);\n", pathname, flags);
    static dlnopen or_open = NULL;
	if (!or_open && !(or_open = dlsym(RTLD_NEXT, "open"))) {
		fprintf(stderr, "Catastrophic failure: dlsym(..., ""open"") == NULL.\n");
		exit(1);
	}

	int ret;
	if (strncmp(pathname, "/dev/input/js", 13) == 0){
        // Skip all blacklisted joystick devices
        // GameMaker attemps to open joysticks very often, so lets process it very early on
        // TODO:: Investigate a proper way of configuring and parsing this list.
        int js_blacklist[] = {
            0,
            -1,
        };

        char path[64] = "/dev/input/jsXXX";
        char *end = NULL;
        int devno = strtol(&((const char*)pathname)[13], &end, 10);
        if (end && *end == '\0') {
            int idx = 0;
            int new_dev = devno;

            // Skip all the blacklisted joysticks.
            while (js_blacklist[idx] >= 0 && js_blacklist[idx] <= new_dev) {
                new_dev++;
                idx++;
            }

            // Open the redirected joystick
            snprintf(&path[13], sizeof(path)-13, "%d", new_dev);
            ret = or_open(path, flags);
        }
    } else { 
		if (flags & O_CREAT)
			ret = or_open(pathname, flags, va_arg(args, mode_t));
		else
			ret = or_open(pathname, flags);
	}

	va_end(args);
	return ret;
}

static void stub_psn_get_trophy_unlock_state(yyg_rval *ret, void *self, void *other, int argc, yyg_rval *args)
{
	return;
}

static void reimpl_bool(yyg_rval *ret, void *self, void *other, int argc, yyg_rval *args)
{
    ret->kind = VALUE_BOOL;
	switch(args[0].kind) {
	case VALUE_REAL:
		ret->rvalue.val = (args[0].rvalue.val > 0.5f) ? 1.0f : 0.0f;
		break;
	case VALUE_BOOL:
		ret->rvalue.val = args[0].rvalue.val;
		break;
	case VALUE_UNDEFINED:
		ret->rvalue.val = 0.0f;
		break;
	default:
		printf("reimpl_bool: Unimplemented kind %d\n", args[0].kind);
		exit(1);
	}
}

static void reimpl_int64(yyg_rval *ret, void *self, void *other, int argc, yyg_rval *args)
{
    ret->kind = VALUE_INT64;
	switch(args[0].kind) {
	case VALUE_REAL:
		ret->rvalue.v64 = args[0].rvalue.val;
		break;
	case VALUE_BOOL:
		ret->rvalue.v64 = args[0].rvalue.val;
		break;
	case VALUE_INT64:
		ret->rvalue.v64 = args[0].rvalue.v64;
		break;
	case VALUE_UNDEFINED:
		ret->rvalue.v64 = 0;
		break;
	default:
		printf("reimpl_int64: Unimplemented kind %d\n", args[0].kind);
		exit(1);
	}
}

static int get_var_slot_by_name(const int *off_count, const yyg_vdecl_t *off_registry, const char *var)
{
	int vars = *off_count;
	//printf("Found %d vars.\n", vars);
	
	if (vars > 0) {
		for (int i = 0; i < vars; i++) {
			if (strcmp(off_registry[i].name, var) == 0)
				return i;
		}
	}
	
	return -1;
}

static void reimpl_variable_global_exists(yyg_rval *ret, void *self, void *other, int argc, yyg_rval *args)
{
	yyg_assure_str(args, 0, other, argc);
	char *var_name = (char*)args[0].rvalue.str->m_thing;
	int var_slot = get_var_slot_by_name(yyg_var_count, yyg_var_registry, var_name);
	if (var_slot < 0)
		var_slot = get_var_slot_by_name(yyg_builtin_var_count, yyg_builtin_var_registry, var_name);

	printf("[r: %d b: %d] get_var_slot_by_name(\"%s\") => %d.\n", *yyg_var_count, *yyg_builtin_var_count, var_name, var_slot);

	ret->rvalue.v64 = (var_slot != -1) ? 1.0f : 0.0f;
	ret->kind = VALUE_BOOL;
	return;
}
#if 0
static void hook_ds_map_set(yyg_rval *ret, void *self, void *other, int argc, yyg_rval *args)
{
	yyg_routine_t or_ds_map_set = (yyg_routine_t)0x0005ac3c;
	printf("hook_ds_map_set(\"%s\", ...);\n", (const char*)args[1].rvalue.str->m_thing);

	or_ds_map_set(ret, self, other, argc, args);
}
#endif
void init_yyg_fix()
{
    yyg_define_builtin("psn_get_trophy_unlock_state", stub_psn_get_trophy_unlock_state, 1, 1);
	yyg_define_builtin("variable_global_exists", reimpl_variable_global_exists, 1, 1);
	yyg_define_builtin("bool", reimpl_bool, 1, 1);
	yyg_define_builtin("int64", reimpl_int64, 1, 1);
	//yyg_define_builtin("ds_map_set", hook_ds_map_set, 1, 1);             
}