#include "../include/threads.hpp"
#include "../include/fft.hpp"
#include "../include/audio.hpp"
#include <unordered_map>

void lock_mutex(SDL_mutex *m, bool *is_locked){
    *is_locked = true;
    SDL_LockMutex(m);
}

void unlock_mutex(SDL_mutex *m, bool *is_locked){
    *is_locked = false;
    SDL_UnlockMutex(m);
}


int FFT_THREAD(void *data){
    ThreadData *ptr = static_cast<ThreadData*>(data);
    FourierTransform *fptr = static_cast<FourierTransform*>(ptr->arg1);

    while(true){
        SDL_LockMutex(ptr->m);

        if(!ptr->is_ready && ptr->is_running){
            SDL_CondWait(ptr->c, ptr->m);
        }

        fptr->generate_visual();

        if(!ptr->is_running){
            break;
        }


        if(ptr->is_ready && ptr->is_running){
            SDL_CondWait(ptr->c, ptr->m);
        }

        SDL_UnlockMutex(ptr->m);
    }

    return 0;
}