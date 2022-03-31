# UnknownField
UnknownField is a tool based clang that obfuscating the order of fields to protect your C/C++ game or code.

## Before
![image](images/UnknownField_before.png)

## After
![image](images/UnknownField_after.png)

## Usage
```bash
UnknownField-cli.exe
Usage: UnknownField-cli.exe [options] <source0> [... <sourceN>]

Optional arguments:
-g                          - Enable global obfucation
```

## Example commands:
```bash
UnknownField-cli.exe test.cpp
UnknownField-cli.exe test.cpp -g
```

## Example SDK:
```C++
#include "sdk/UnknownFieldSDK.h"
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
```

## TODO
- Obfuscating the order of virtual functions.
- GUI Tool.

## Note
This project is currently still a demo.
