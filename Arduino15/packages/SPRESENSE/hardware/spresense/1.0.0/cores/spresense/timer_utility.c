
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <common/up_arch.h>
#include <cxd56_clock.h>
#include <chip/cxd56_timer.h>
#include <arch/chip/timer.h>
#include "utility.h"

typedef struct {
    int fd;
    uint32_t base;
} timer_map_t;

static timer_map_t s_timer_map[] = {
    { -1, CXD56_TIMER0_BASE },
    { -1, CXD56_TIMER1_BASE },
};

static int get_timer_status(int fd, struct timer_status_s* status)
{
    assert(status);
    if (ioctl(fd, TCIOC_GETSTATUS, (unsigned long)((uintptr_t)status)) < 0) {
        printf("ERROR: Failed to get timer status (errno = %d)\n", errno);
        return ERROR;
    }
    return OK;
}

bool util_timer_is_running(int fd)
{
    struct timer_status_s status;
    if (get_timer_status(fd, &status))
        return false;

    return status.flags & TCFLAGS_ACTIVE;
}

uint32_t util_get_time_out(int fd)
{
    struct timer_status_s status;
    if (get_timer_status(fd, &status))
        return 0;

    return status.timeout;
}

uint32_t util_get_time_left(int fd)
{
    static uint32_t cpu_clk = 0;

    if (cpu_clk == 0)
        cpu_clk = cxd56_get_cpu_baseclk();

    uint32_t base = 0;
    arrayForEach(s_timer_map, i) {
        if (s_timer_map[i].fd == fd) {
            base = s_timer_map[i].base;
            break;
        }
    }

    if (base == 0) return 0;

    uint64_t remaining = (uint64_t)getreg32(base + CXD56_TIMER_VALUE);
    uint32_t timeleft = (uint32_t)(remaining * 1000000ULL / cpu_clk);

    return timeleft;
}

uint32_t util_get_time_collapsed(int fd)
{
    struct timer_status_s status;
    if (get_timer_status(fd, &status))
        return 0;

    return (status.timeout - status.timeleft);
}

int util_start_timer(int fd, unsigned long timeout /*us*/, tccb_t cb)
{
    struct timer_sethandler_s sethandler;

    int ret = ioctl(fd, TCIOC_SETTIMEOUT, timeout);
    if (ret < 0) {
        printf("ERROR: Failed to set timer timeout [%lu] (errno = %d)\n", timeout, errno);
        return ERROR;
    }

    sethandler.handler = cb;
    sethandler.arg     = NULL;

    ret = ioctl(fd, TCIOC_SETHANDLER, &sethandler);
    if (ret < 0) {
        printf("ERROR: Failed to set timer handler (errno = %d)\n", errno);
        return ERROR;
    }

    ret = ioctl(fd, TCIOC_START, 0);
    if (ret < 0) {
        printf("ERROR: Failed to start timer (errno = %d)\n", errno);
        return ERROR;
    }

    struct timer_status_s status;
    if (get_timer_status(fd, &status) == 0 && status.timeout != timeout)
        printf("start status.timeout = %d\n", status.timeout);

    return OK;
}

int util_stop_timer(int fd)
{
    if (util_timer_is_running(fd)) {
        int ret = ioctl(fd, TCIOC_STOP, 0);
        if (ret < 0) {
            printf("ERROR: Failed to stop timer (errno = %d)\n", errno);
            return ERROR;
        }
    }
    return OK;
}

int util_open_timer(const char* dev_name, int* fd)
{
    // timer init is done in boardctl
    *fd = open(dev_name, O_RDONLY);
    if (*fd < 0) {
        printf("ERROR: Failed to open %s: %d\n", dev_name, errno);
        return ERROR;
    }

    if (strcmp(dev_name, "/dev/timer0") == 0)
        s_timer_map[0].fd = *fd;
    else
        s_timer_map[1].fd = *fd;

    return OK;
}

int util_close_timer(int fd)
{
    return close(fd);
}
