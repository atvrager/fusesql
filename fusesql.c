#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <sqlite3.h>

struct sqlite_private_data {
  sqlite3* handle;
};

void* sqlite_init(struct fuse_conn_info* conn) {
  struct fuse_context* ctx = fuse_get_context();
  struct sqlite_private_data* priv = ctx->private_data;
  sqlite3_open("./test.db", &priv->handle);
  return priv;
}

void sqlite_destroy(void* priv_data) {
  struct sqlite_private_data* priv = (struct sqlite_private_data*)priv_data;
  sqlite3_close(priv->handle);
}

int sqlite_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info) {
  struct fuse_context* ctx = fuse_get_context();
  struct sqlite_private_data* priv = ctx->private_data;
  sqlite3_stmt* stmt;
  const char* tail;
  sqlite3_prepare_v2(priv->handle, "SELECT name FROM SQLITE_MASTER;", 255, &stmt, &tail);
  while (sqlite3_step(stmt) != SQLITE_DONE) {
    const char* str = (const char*)sqlite3_column_text(stmt, 0);
    filler(buf, str, NULL, 0);
  }
  sqlite3_finalize(stmt);
  return 0;
}

int sqlite_getattr(const char* path, struct stat *stbuf) {
  stbuf->st_nlink = 2;
  stbuf->st_mode = S_IFDIR | 0777;
  return 0;
}

static struct fuse_operations sqlite_ops = {
  .init = sqlite_init,
  .destroy = sqlite_destroy,
  .readdir = sqlite_readdir,
  .getattr = sqlite_getattr,
};

int main(int argc, char** argv) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  //fuse_opt_parse(&args, &param, fusesql_opts, fusesql_process_arg);
  struct sqlite_private_data priv = { 0 };
  return fuse_main(args.argc, args.argv, &sqlite_ops, (void*)&priv);
}
