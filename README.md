# brainseabar
난해한 brainseabar(Brain-Sea-Bar) 프로그래밍 언어

## 소개
Brainfuck 언어에 영향을 받아 작성자가 직접 만든, 스택 기반의 난해한 프로그래밍 언어입니다.   
생각보다 많은 개발자들이 부적절한 글꼴을 사용하는 것을 봤습니다. 이 언어는 그들에게 암묵적으로 적절한 글꼴 사용을 강제할 것입니다.   
이름의 기원은 fuck을 우리말로 바꿔서 쓴 것입니다.

## 알림
세상에 난해한 프로그래밍 언어는 무수히 많기 때문에 이 언어와 매우 비슷한 다른 언어가 존재할 확률이 높습니다.
하지만 작성자는 이 언어를 만드는 동안 Brainfuck 외의 어떠한 언어도 참고하지 않았습니다.   
튜링 완전한지 판별하는 과정은 거치지 않았습니다. 튜링 완전하지 않다면 보완하겠습니다.

## 구조
### 데이터
brainseabar은 모든 데이터를 부호 없는 8비트 정수형으로 다룹니다.

### 스택과 스택 포인터
brainseabar은 LIFO 구조인 스택을 사용하지만, item이 좌우로 쌓이는 구조이며, 스택 포인터가 스택의 최우측이나 최좌측을 가리키지는 않습니다.   
스택의 용량은 2MiB를 넘지 않습니다.   
현재 스택 위치는 ```sp```로 표현합니다.
```
    ┌────┬────┬────┬────┬────┲━━━━┱────┬────┬────┐
... │ .. │ .. │ .. │ .. │ .. ┃ .. ┃ .. │ .. │ .. │ ...
    └────┴────┴────┴────┴────┺━━━━┹────┴────┴────┘
                               sp
```

### 명령어
push 작업이 있으면, 그 전에 ```sp``` 위치를 포함하여 왼쪽의 item들을 왼쪽으로 한 칸씩 밀며, pop 작업이 있으면, 그 후에 ```sp``` 왼쪽의 item들을 오른쪽으로 한 칸씩 당깁니다.   
brainseabar의 명령어는 13개가 있으며, 내용은 아래와 같습니다.

```
1  // 1을 push
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ bb │ cc ┃  1 ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp

0  // pop
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ xx │ aa ┃ bb ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp

'  // sp를 왼쪽으로 한 칸 이동
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ xx │ aa ┃ bb ┃ cc │ dd │ ee │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp

"  // sp를 오른쪽으로 한 칸 이동
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ bb │ cc ┃ dd ┃ ee │ ff │ yy │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp

I  // sp 위치의 값을 push (복사 기능)
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ bb │ cc ┃ cc ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp

O  // sp 위치의 값을 그 왼쪽의 값과 바꾸기
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ aa │ cc ┃ bb ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp

l  // 두 번 pop하고, 그 두 값의 arithmetic add 값을 push
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━━━━━━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ xx │ aa ┃ bb PLUS cc ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━━━━━━━━━┹────┴────┴────┘
                sp                                             sp

|  // 두 번 pop하고, 그 두 값의 bitwise nand 값을 push
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━━━━━━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ xx │ aa ┃ bb NAND cc ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━━━━━━━━━┹────┴────┴────┘
                sp                                             sp

i  // 1바이트 정수를 입력받고, 그 값을 push
    ┌────┬────┲━━━━┱────┬────┬────┐            ┌────┬────┲━━━━┱────┬────┬────┐
... │ aa │ bb ┃ cc ┃ dd │ ee │ ff │ ... => ... │ bb │ cc ┃ ii ┃ dd │ ee │ ff │ ...
    └────┴────┺━━━━┹────┴────┴────┘            └────┴────┺━━━━┹────┴────┴────┘
                sp                                         sp


j  // sp 위치의 값을 ascii로 출력
J  // sp 위치의 값을 number로 출력
[  // sp 위치의 값이 0이면, 일치하는 ] 명령어의 위치까지 정방향으로 점프
]  // sp 위치의 값이 0이 아니면, 일치하는 [ 명령어의 위치까지 반대방향으로 점프
```
**주의: ```J``` 명령어는 추후에 삭제될 예정입니다. 사용에 주의하세요.**   
**해석기는 위 명령어 외의 모든 문자는 무시하지만, 코드의 가독성을 위해(?) 공백이나 새 줄 외에는 다른 문자 삽입을 자제하는 것이 좋습니다.**

### 해석기의 강제 종료 조건
* 스택이 최대 용량을 초과하는 경우   
* 스택에 item이 적게 들어가 있어 명령어 수행이 불가능한 경우

### 주석
해석기는 해시 기호( # ) 사이의 모든 문자를 명령어로 해석하지 않습니다. 주석의 시작과 끝을 해시 기호로 명시해야 합니다.
```
# push 0 #
11|1l1l
```

### 간단한 예제
두 수를 입력받고 산술 덧셈 출력하기
```
iilJ0
```
두 수를 입력받고 논리 곱 출력하기
```
ii|I|J0
```
두 수를 입력받고 논리 합 출력하기
```
ii'I|"I||J0
```
숫자 0 출력하기
```
11|1l1lJ0
```
숫자 170 출력하기
```
1IlIl1lIlIl1lIlIl1lIlJ0
```
Hello World! 출력하기
```
1IlIlIl1lIlIlIlj0 1Il1lIlIlIl1lIlIl1lj 1Il1lIl1lljj 11l1llj 1IlIlIlIlIljO
1IlIl1lIlIl1lIl1lIl1lj0 j 11l1llj 1Il1lIlI|1llj 1IlIlIlI|1ll j0 1lj0 1IlIl1lIlj0
```

### 함수 작성 규약
* 매개변수는 ```sp``` 위치를 포함하여 왼쪽에 연속적으로 나열합니다.
* 함수 설명에 매개변수가 나열되어 있으면 그 순서대로 push되어 해석된다고 약속합니다.   
예를 들어 두 개의 매개변수를 받고, 그 이름을 각각 ```num1``` ```num2```로 한다면, ```num1```으로 넘길 인자를 먼저 push, 그 다음 ```num2```로 넘길 인자를 push, 그리고 함수를 호출합니다.
* 반환값 역시 ```sp``` 위치를 포함하여 왼쪽에 연속적으로 나열합니다.
* 반환값의 순서 역시 매개변수의 순서와 같이, 함수 설명에 나와 있는 순서대로 push되어 반환된다고 약속합니다.   
예를 들어 두 개의 값을 반환하고, 그 이름을 각각 ```quot``` ```rema```로 한다면, ```quot```으로 넘길 값을 먼저 push, 그 다음 ```rema```로 넘길 값을 push, 그리고 함수를 종료합니다.
* 함수가 종료되면 매개변수는 모두 pop 되어 있어야 합니다.

### 해석기 사용 방법
```bsbmain.h``` 파일을 ```include```하고, ```createBsbstack``` ```destroyBsbstack``` ```bsbExecute``` 함수를 ```extern``` 키워드를 사용하여 선언합니다.
``` C
#include <stdint.h>
#include "bsbmain.h"

// ...

extern struct bsbstack* createBsbstack ();
extern int destroyBsbstack (struct bsbstack* st);
extern int bsbExecute (char* filename, struct bsbstack* st);
```

```bsbExecute``` 함수를 호출하여 해석을 시작할 수 있으며, ```filename```에는 해석할 파일 이름이 들어갑니다.   
```createBsbstack``` 함수는 해석을 위한 새로운 스택을 만듭니다. ```destroyBsbstack``` 함수는 기존 스택을 제거합니다.   
```bsbExecute``` 함수는 현재 스택 ```st``` 상에서 ```filename```을 이름으로 하는 소스 코드를 실행합니다.
