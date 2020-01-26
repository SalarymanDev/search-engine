#ifndef LOCKGUARD_H
#define LOCKGUARD_H

#include <pthread.h>

namespace BBG {

    // Locking RAII
    class lockguard {
    private:
        pthread_mutex_t * m;
        
    public:

        lockguard(pthread_mutex_t *_m) : m(_m) {
            pthread_mutex_lock(m);
        }

        ~lockguard() {
            pthread_mutex_unlock(m);
        }
    };
}

#endif
