/*
 * Licensed under a two-clause BSD-style license.
 * See LICENSE for details.
 */

#include "git-compat-util.h"
#include "sliding_window.h"
#include "line_buffer.h"
#include "strbuf.h"

static int input_error(struct line_buffer *file)
{
	if (!buffer_ferror(file))
		return error("delta preimage ends early");
	return error("cannot read delta preimage: %s", strerror(errno));
}

static int skip_or_whine(struct line_buffer *file, off_t gap)
{
	const off_t nread = buffer_skip_bytes(file, gap);
	return nread == gap ? 0 : input_error(file);
}

static int read_to_fill_or_whine(struct line_buffer *file,
				struct strbuf *buf, size_t width)
{
	buffer_read_binary(file, buf, width - buf->len);
	return buf->len == width ? 0 : input_error(file);
}

static int check_overflow(off_t a, size_t b)
{
	if (b > maximum_signed_value_of_type(off_t))
		return error("unrepresentable length in delta: "
				"%"PRIuMAX" > OFF_MAX", (uintmax_t) b);
	if (signed_add_overflows(a, (off_t) b))
		return error("unrepresentable offset in delta: "
				"%"PRIuMAX" + %"PRIuMAX" > OFF_MAX",
				(uintmax_t) a, (uintmax_t) b);
	return 0;
}

int move_window(struct sliding_view *view, off_t off, size_t width)
{
	off_t file_offset;
	assert(view);
	assert(view->width <= view->buf.len);
	assert(!check_overflow(view->off, view->buf.len));

	if (check_overflow(off, width))
		return -1;
	if (off < view->off || off + width < view->off + view->width)
		return error("invalid delta: window slides left");

	file_offset = view->off + view->buf.len;
	if (off < file_offset) {
		/* Move the overlapping region into place. */
		strbuf_remove(&view->buf, 0, off - view->off);
	} else {
		/* Seek ahead to skip the gap. */
		if (skip_or_whine(view->file, off - file_offset))
			return -1;
		strbuf_setlen(&view->buf, 0);
	}

	if (view->buf.len > width)
		; /* Already read. */
	else if (read_to_fill_or_whine(view->file, &view->buf, width))
		return -1;

	view->off = off;
	view->width = width;
	return 0;
}
