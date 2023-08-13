#include <byteswap.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define SDP_READ_REGISTER	0x0101

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be16_to_cpu(val) bswap_16(val)
#define be32_to_cpu(val) bswap_32(val)
#define be64_to_cpu(val) bswap_64(val)
#define cpu_to_be16(val) bswap_16(val)
#define cpu_to_be32(val) bswap_32(val)
#define cpu_to_be64(val) bswap_64(val)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define be16_to_cpu(val) (val)
#define be32_to_cpu(val) (val)
#define be64_to_cpu(val) (val)
#define cpu_to_be16(val) (val)
#define cpu_to_be32(val) (val)
#define cpu_to_be64(val) (val)
#else
#error "Unknown byte order"
#endif

struct sdp_request {
	uint8_t req;
	uint16_t cmd;
	uint32_t addr;
	uint8_t format;
	uint32_t cnt;
	uint32_t data;
	uint8_t rsvd;
} __attribute__ ((__packed__));

struct sdp_readl_response {
	uint8_t req;
	uint32_t val;
} __attribute__ ((__packed__));

void usage(void)
{
	printf("Usage: sdpread <hidraw device> <addr>\n");
}

int main(int argc, char *argv[])
{
	struct sdp_request cmd = {};
	struct sdp_readl_response resp = {};
	int ret, fd;

	if (argc != 3) {
		usage();
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Unable to open %s (%s)\n", argv[1], strerror(errno));
		return 1;
	}

	cmd.req = 0x1;
	cmd.cmd = cpu_to_be16(0x0101);
	cmd.addr = cpu_to_be32(strtol(argv[2], NULL, 0));
	cmd.cnt = cpu_to_be32(sizeof(uint32_t));

	ret = write(fd, &cmd, sizeof(cmd));
	if (ret != sizeof(cmd)) {
		fprintf(stderr, "Unable to submit command (%s), wrong device?\n",
			strerror(errno));
		return 1;
	}

	ret = read(fd, &resp, sizeof(resp));
	if (ret != sizeof(resp)) {
		fprintf(stderr, "Error reading response (%d)\n", ret);
		return 1;
	}

	/* don't care, read next packet */

	ret = read(fd, &resp, sizeof(resp));
	if (ret != sizeof(resp)) {
		fprintf(stderr, "Error reading response (%d)\n", ret);
		return 1;
	}

	/* dump value */
	printf("%08x\n", resp.val);

	return 0;
}
