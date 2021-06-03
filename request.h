#ifndef __REQUEST_H__

struct threadStat {
    int thread_id;
    int count;
    int thread_static;
    int thread_dynamic;
};

void requestHandle(int fd, struct threadStat *thread_stat, long dispatch_interval, long arrival_time);

#endif
