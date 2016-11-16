#include <stdlib.h> /* NULL */

#include <uninorm.h>
#include <unicase.h>

#include <lua.h>
#include <lauxlib.h>

#include "compat-5.3.h"

static const uninorm_t uninorms[] = {UNINORM_NFD, UNINORM_NFC, UNINORM_NFKD, UNINORM_NFKC};
static const char *const uninormnames[] = {"NFD", "NFC", "NFKD", "NFKC", NULL};


static int lunistring_normalize(lua_State *L) {
	uninorm_t nf = uninorms[luaL_checkoption(L, 1, NULL, uninormnames)];
	size_t n;
	const uint8_t *s = (const uint8_t*)luaL_checklstring(L, 2, &n);
	luaL_Buffer b;
	size_t lengthp = n; /* starting guess of equal length */
	uint8_t *resultbuf, *tmp;

	luaL_buffinit(L, &b);

	while (1) {
		resultbuf = (uint8_t*)luaL_prepbuffsize(&b, lengthp);
		if (!(tmp = u8_normalize(nf, s, n, resultbuf, &lengthp))) {
			return luaL_fileresult(L, 0, NULL);
		}
		if (tmp != resultbuf) {
			free(tmp);
		} else {
			break;
		}
	}

	luaL_pushresultsize(&b, lengthp);
	return 1;
}


static int lunistring_casefold(lua_State *L) {
	size_t n;
	const uint8_t *s = (const uint8_t*)luaL_checklstring(L, 1, &n);
	const char *iso639_language = luaL_optstring(L, 2, NULL);
	uninorm_t nf = uninorms[luaL_checkoption(L, 3, NULL, uninormnames)];
	luaL_Buffer b;
	size_t lengthp = n; /* starting guess of equal length */
	uint8_t *resultbuf, *tmp;

	luaL_buffinit(L, &b);

	while (1) {
		resultbuf = (uint8_t*)luaL_prepbuffsize(&b, lengthp);
		if (!(tmp = u8_casefold(s, n, iso639_language, nf, resultbuf, &lengthp))) {
			return luaL_fileresult(L, 0, NULL);
		}
		if (tmp != resultbuf) {
			free(tmp);
		} else {
			break;
		}
	}

	luaL_pushresultsize(&b, lengthp);
	return 1;
}


int luaopen_unistring(lua_State *L) {
	static const luaL_Reg lib[] = {
		{"normalize", lunistring_normalize},
		{"casefold", lunistring_casefold},
		{NULL, NULL}
	};
	luaL_newlib(L, lib);
	return 1;
}
