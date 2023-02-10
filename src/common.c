// common.c

void buffer_init(Buffer* buffer) {
  assert(buffer);
  buffer->data = NULL;
  buffer->size = 0;
}

void buffer_init_new(Buffer* buffer, u32 size) {
  assert(buffer);
  buffer->data = malloc(size);
  if (buffer->data) {
    buffer->size = size;
    return;
  }
  assert(!"buffer_init_new: failed to allocate memory");
}

void buffer_free(Buffer* buffer) {
  assert(buffer);
  if (buffer->data) {
    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
  }
}

u32 buffer_iterate(void* restrict dest, Buffer* source, u32 size, u32* iter) {
  assert("buffer_iterate: write outside buffer memory area" && (*iter + size) <= source->size);
  memcpy(dest, &source->data[*iter], size);

  *iter += size;

  return size;
}

Result file_read(const char* path, Buffer* buffer) {
	Result result = Ok;
	u32 num_bytes_read = 0;
  buffer_init(buffer);

	FILE* fp = fopen(path, "rb");
	if (!fp) {
		fprintf(stderr, "file_read: file `%s` does not exist.\n", path);
    return_defer(Err);
	}

	fseek(fp, 0, SEEK_END);
	u32 size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer->data = malloc(size * sizeof(u8));
	buffer->size = size;
	if (!buffer->data) {
		buffer->size = 0;
    return_defer(Err);
	}

	num_bytes_read = fread(buffer->data, 1, size, fp);
	if (num_bytes_read != size) {
		fprintf(stderr, "file_read: failed to read file `%s`.\n", path);
    return_defer(Err);
	}

defer:
  if (fp) {
	  fclose(fp);
  }
	return result;
}

Result file_write(const char* path, Buffer* buffer) {
  Result result = Ok;
  buffer_init(buffer);

  FILE* fp = fopen(path, "wb");
  if (!fp) {
    fprintf(stderr, "file_write: failed to open file `%s` for writing.\n", path);
    return_defer(Err);
  }
  i32 write_result = fwrite(buffer->data, buffer->size, 1, fp);
  fclose(fp);
  if (write_result != 1) {
    fprintf(stderr, "file_write: failed to write data to file `%s`.\n", path);
    return_defer(Err);
  }
defer:
  return result;
}

void log_error(const char* format, ...) {
  fprintf(stderr, "[error]: ");
  va_list argp;
  va_start(argp, format);
  vfprintf(stderr, format, argp);
  va_end(argp);
}

inline f32 lerp_f32(f32 v0, f32 v1, f32 t) {
  return (1.0f - t) * v0 + t * v1;
}
