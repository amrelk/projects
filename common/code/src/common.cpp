#include "errors.hpp"

namespace app {
void main();
}

extern "C"
{

/*void _close(void) {}
void _lseek(void) {}
void _read(void) {}
void _write(void) {}
void _getpid(void) {}
void _kill(void) {}*/

int main() {
    app::main();
    app::runtime_panic("Main terminated!");
}
}
