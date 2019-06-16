#include <stdio.h>
#include <unistd.h>

int gl;

int add(int x, int y){
  return x + y;
}

int mul(int x, int y){
  return x * y;
}

int hoge(int x, int y){
  gl = 5;
  return add(x, y);
}

int main(void){
  while(1){
    printf("gl : %d\n", gl);
    printf("result : %d\n", hoge(2, 4));
    sleep(2);
  }
  return 0;
}
