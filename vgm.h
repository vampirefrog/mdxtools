#ifndef VGM_H_
#define VGM_H_

#include <stdio.h>
#include <stdint.h>

struct vgm_logger {
	FILE *f;
	int total_samples;
	int total_bytes;

	int wait_cummulative;
};

int vgm_logger_init(struct vgm_logger *log, const char *filename);
int vgm_logger_write_wait(struct vgm_logger *log, int wait);
int vgm_logger_write_ym2151(struct vgm_logger *log, uint8_t reg, uint8_t val);
int vgm_logger_write_okim6258(struct vgm_logger *log, uint8_t port, uint8_t val);
int vgm_logger_end(struct vgm_logger *log);

#endif /* VGM_H_ */
