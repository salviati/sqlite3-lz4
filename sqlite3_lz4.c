// sqlite3-lz4 - a lz4 extension for sqlite3.
// Copyright (c) 2013 Utkan Güngördü <utkan@freeconsole.org>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <sqlite3ext.h>
#include <lz4.h>
#include <lz4hc.h>
SQLITE_EXTENSION_INIT1

static inline void write32(char* restrict sp, int x) {
	unsigned char* restrict p = (unsigned char *)sp;
	p[0] = x >> 24 & 0xff;
	p[1] = x >> 16 & 0xff;
	p[2] = x >> 8  & 0xff;
	p[3] = x       & 0xff;
}

static inline int read32(const char* restrict sp) {
	unsigned char* restrict p = (unsigned char *)sp;
	return ((int)p[0] << 24) | ((int)p[1] << 16) | ((int)p[2] << 8) | (int)p[3];
}

static void _lz4compress(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
	if (argc != 1) {
		// error.
		return;
	}
	int in_len = sqlite3_value_bytes(argv[0]);
	const char *in = sqlite3_value_blob(argv[0]);

	int out_buf_len = LZ4_compressBound(in_len);
	char *out = sqlite3_malloc(4 + out_buf_len);
	int out_len = LZ4_compress(in, 4 + out, in_len);
	if (out_len < 0) {
		// error
		sqlite3_free(out);
		return;
	}

	write32(out, in_len);
	sqlite3_result_blob(ctx, out, 4 + out_len, sqlite3_free);
}


static void _lz4compresshc(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
	if (argc != 1) {
		// error.
		return;
	}
	int in_len = sqlite3_value_bytes(argv[0]);
	const char *in = sqlite3_value_blob(argv[0]);

	int out_buf_len = LZ4_compressBound(in_len);
	char *out = sqlite3_malloc(4 + out_buf_len);
	int out_len = LZ4_compressHC(in, 4 + out, in_len);
	if (out_len < 0) {
		// error
		sqlite3_free(out);
		return;
	}

	write32(out, in_len);
	sqlite3_result_blob(ctx, out, 4 + out_len, sqlite3_free);
}

static void _lz4uncompress(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
	if (argc != 1) {
		// error.
		return;
	}

	int in_len = sqlite3_value_bytes(argv[0]);
	if (in_len <= 4) {
		// return an empty buffer instead?
		return;
	}
	const char *in = sqlite3_value_blob(argv[0]);

	int out_len = read32(in);

	char *out = sqlite3_malloc(out_len);
	int nread = LZ4_uncompress(4 + in, out, out_len);
	if (nread < 0 || nread != in_len-4) {
		// error
		sqlite3_free(out);
		return;
	}

	sqlite3_result_blob(ctx, out, out_len, sqlite3_free);
}



int sqlite3_extension_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
	SQLITE_EXTENSION_INIT2(pApi)
	sqlite3_create_function(db, "lz4compress",   1, SQLITE_UTF8, 0, _lz4compress,   0, 0);
	sqlite3_create_function(db, "lz4compresshc", 1, SQLITE_UTF8, 0, _lz4compresshc, 0, 0);
	sqlite3_create_function(db, "lz4uncompress", 1, SQLITE_UTF8, 0, _lz4uncompress, 0, 0);

	return 0;
}
