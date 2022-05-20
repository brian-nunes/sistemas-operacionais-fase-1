// init.c: roda ao inicializar o kernel (em modo usuário)

#include "hello_world.h"
#include "last_scno.h"
#include "stack.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void panic(const char *msg){
	fprintf(stderr, "%s: %s (errno = %d)\n", msg,
										strerror(errno), errno);
	exit(-1);
}
void mount_fs(){
	printf("Mounting filesystems\n");
	// If /sys is not created, make it read-only (mode = 444)
	if (mkdir("/sys", 0x124) && errno != EEXIST)
		panic("mkdir");
	if (mount("none", "/sys", "sysfs", 0, ""))
		panic("mount");
}

int main(){
	mount_fs(); // monta o sysfs em /sys

	hello_world(); // scno de hello_world = 400
	printf("last system call number = %lu\n", last_scno());

	stack_push(5); // scno de stack_push = 398
	printf("last system call number = %lu\n", last_scno());

	stack_pop(); // scno de stack_pop = 399
	printf("last system call number = %lu\n", last_scno());

	int fd = open("/sys/kernel/sys_last_nr/last_nr", O_RDONLY);
	// scno de open = 5
	printf("last system call number = %lu\n", last_scno());

	char filc[5];
	if (lseek(fd, 0, SEEK_SET)) {
		perror("can’t lseek");
	} else {
		int size = read(fd, filc, 4);
		if (size < 0) {
			perror("can’t read");
		} else {
			printf("last system call number from sys dir = %s\n",
															filc);

			// scno de read = 3
		}
	}

	printf("last system call number = %lu\n", last_scno());
	// scno de write = 4

	for(;;){
		sleep(1000); // continua executando o kernel
	}
	
	return 0;
}