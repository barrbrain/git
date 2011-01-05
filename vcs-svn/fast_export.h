#ifndef FAST_EXPORT_H_
#define FAST_EXPORT_H_

struct strbuf;
struct line_buffer;

void fast_export_init(int fd);
void fast_export_deinit(void);
void fast_export_reset(void);

void fast_export_delete(const char *path);
void fast_export_modify(const char *path, uint32_t mode, const char *dataref);
void fast_export_begin_commit(uint32_t revision, uint32_t author, char *log,
			uint32_t uuid, uint32_t url, unsigned long timestamp);
void fast_export_end_commit(uint32_t revision);
void fast_export_data(uint32_t mode, uint32_t len, struct line_buffer *input);
void fast_export_delta(uint32_t mode, const char *path,
			uint32_t old_mode, const char *dataref,
			uint32_t len, struct line_buffer *input);

void fast_export_ls_rev(uint32_t rev, const char *path,
			uint32_t *mode_out, struct strbuf *dataref_out);
void fast_export_ls(const char *path,
			uint32_t *mode_out, struct strbuf *dataref_out);

#endif
