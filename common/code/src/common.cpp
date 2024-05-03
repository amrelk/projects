#include "errors.hpp"

namespace ev24 {
void main();
}

extern "C"
{

void _close(void) {}
void _lseek(void) {}
void _read(void) {}
void _write(void) {}
void _getpid(void) {}
void _kill(void) {}

int main() {
    ev24::main();
    ev24::runtime_panic("Main terminated!");
}
}
