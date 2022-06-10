#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "stop.h"

#define len(_arr) ((int)((&_arr)[1] - _arr))

static const char * const programs[] = { "/block_test" };

void panic(const char *msg)
{
	fprintf(stderr, "%s: %s (errno = %d)\n", msg, strerror(errno), errno);
	exit(-1);
}

void mount_fs()
{
	printf("Mounting filesystems\n");
	// If /sys is not created, make it read-only (mode = 444)
	if (mkdir("/sys", 0x124) && errno != EEXIST)
		panic("mkdir");
	if (mount("none", "/sys", "sysfs", 0, ""))
		panic("mount");
}

int main()
{
	printf("Custom initramfs - Blocking and Unblocking Process:\n");
	mount_fs();

	printf("Forking to run %d programs\n", len(programs));

	pid_t pid_A = fork();
	if (pid_A == -1) {
		perror("fork");
		return -1;
	} else if (pid_A) {
		pid_t pid_B = fork();
		if (pid_B == -1) {
			perror("fork");
			return -1;
		} else if (pid_B) {
			const struct timespec ts = {.tv_sec = 2, .tv_nsec = 0 };
			nanosleep(&ts, NULL);

			while(1) {
				stop_process(pid_A);
				nanosleep(&ts, NULL);
				continue_process();
				nanosleep(&ts, NULL);

				stop_process(pid_B);
				nanosleep(&ts, NULL);
				continue_process();
				nanosleep(&ts, NULL);
			}
		} else {
			for (int i = 0;;i++) {
				printf("2\n");
				const struct timespec ts = {.tv_sec = 0, .tv_nsec = 5e8 };
				
				nanosleep(&ts, NULL);
			}
		}
	} else {
		for (;;) {
			printf("1\n");
			const struct timespec ts = {.tv_sec = 0, .tv_nsec = 5e8 };
			nanosleep(&ts, NULL);
		}
	}
	printf("init finished\n");
	for (;;)
		sleep(1000);
	return 0;
}

