#include "common.hpp"
#include "main.h"

namespace ev24 {
[[noreturn]]
int main() {
  while (true) {
    HAL_Delay(10);
  }
}


void init_panic([[maybe_unused]] const char *x) {while(true){}}

void runtime_panic([[maybe_unused]] const char *x) {while(true){}}

}
