#include <stdio.h>
#include "../sdk/UnknownFieldSDK.h"

static unsigned int gv = 0;
class C1 {
public:
  C1() { gv = 1; }
};

class UnknownFieldProtection(C2) {
public:
  C1 c;
  unsigned int gv_ = gv;
};

int main() { 
	C2 c2;
	printf("c2.gv=%d\n", c2.gv_);
	return 0; 
}
