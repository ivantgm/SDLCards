#include "thread.hpp"
#include <SDL2/SDL.h>

namespace SDLCards {

//-----------------------------------------------------------------------------
Thread::Thread(const string& name, bool auto_free) {
    this->name = name;
    this->auto_free = auto_free;
}

//-----------------------------------------------------------------------------
void Thread::execute(void) {
    auto thread_function = [](void *obj) {
        Thread *thread = (Thread*)obj;
        int result = thread->on_execute(); 
        thread->on_terminate();       
        if(thread->auto_free) {
            delete thread;
        }         
        return result;
    };
    SDL_Thread* thread;
    thread = SDL_CreateThread(thread_function, name.c_str(), (void*)this);
}

//-----------------------------------------------------------------------------
int Thread::on_execute(void) {    
    return 0;
}

//-----------------------------------------------------------------------------
void Thread::on_terminate(void) {
    // não faz nada
}

} // namespace SDLCards
