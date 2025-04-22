#ifndef __GAME_THREAD__
#define __GAME_THREAD__

#include <string>

namespace SDLCards {

using namespace std;

class Thread {    
public:
    Thread(const string& name, bool auto_free);
    void execute(void);
protected:
    virtual int on_execute(void);
    virtual void on_terminate(void);
private:
    string name;
    bool auto_free;
};

} // namespace SDLCards

#endif
