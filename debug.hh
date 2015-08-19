#ifndef __DEBUG_HH__
#define __DEBUG_HH__

#include <cassert>
#include <fstream>

class Debug {
public:
  Debug(const char *name) {
    _out.open(name);
    assert(_out.is_open());
  }
  ~Debug(void) {
    _out.close();
  }
  std::ofstream& out(void) { return _out; }
  
private:
  std::ofstream _out;
};

extern Debug debug;

#endif // __DEBUG_HH__
