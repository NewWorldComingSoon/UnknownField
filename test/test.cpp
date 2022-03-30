#include "../sdk/UnknownFieldSDK.h"
#include <Windows.h>

class UnknownFieldProtection(MyClassX) {
public:
  MyClassX();
  ~MyClassX();
private:
  UCHAR name[300];
  DWORD mp;
  DWORD maxmp;
  DWORD hp;
  DWORD maxhp;
  unsigned char level;
};

class MyClassX2 {
public:
  MyClassX2();
  ~MyClassX2();

private:
  char name2[300];
  int mp2;
  int maxmp2;
  int hp2;
  int maxhp2;
  unsigned char level2;
};

class MyClassX3 {
public:
  MyClassX3();
  ~MyClassX3();

private:
  char name3[300];
  int mp3;
  int maxmp3;
  int hp3;
  int maxhp3;
  unsigned char level3;
};

int myfunct(int a, int b) {
	int c;
    c = a + b;
    return c;
}
