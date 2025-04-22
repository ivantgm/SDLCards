#ifndef __CACHETA_APP__
#define __CACHETA_APP__

#include "app.hpp"

namespace SDLCards {

class Cacheta: public App {
public:
    Cacheta();
    ~Cacheta();
    void poll_event(SDL_Event *e);
private:
};

} // namespace SDLCards

#endif