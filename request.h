#ifndef __REQUEST_H__

struct threadStat {
    int thread_id;
    int count;
    int thread_static;
    int thread_dynamic;
};

void requestHandle(int fd, struct threadStat *thread_stat, struct timeval dispatch_time, struct timeval arrival_time);

#endif
