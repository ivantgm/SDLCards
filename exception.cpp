#include "exception.hpp"
#include <iostream>

namespace SDLCards {

//-----------------------------------------------------------------------------
Exception::Exception(const string& msg, const string& error) {
    this->msg = msg;
    this->error = error;
}

//-----------------------------------------------------------------------------
void Exception::show(void) const {
    cout << msg << ": " << error << endl;
}

} // namespace SDLCards
