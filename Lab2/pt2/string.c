#include <stdio.h>


int strlen(char *s);
int strcmp(char *s1, char *s2);
int strcpy(char *dest, char *src);
int strncpy(char *dest, char *src, int n);
int strncmp(char *s1, char *s2, int n);
int memcpy(char *dest, char *src, int n);
int memset(char *dest, char c, int n);
/*
int main() {

  char s[20] = "Computer";
  char d[20];
  mmemset(d,0,20);
  mmemcpy(d, s, 4);
  printf("%s\n",d);

  return 0;
}
*/
int strlen(char *s) {
  int sum = 0;
  while(*s++) {
    sum++;
  }
  return sum;
}
int strcmp(char *s1, char *s2) {
  int i;
  if (strlen(s1) != strlen(s2)) { return 1; }
  for (i = 0; i < strlen(s1); i++) {
    if (s1[i] != s2[i]) { return 1; }
  }
  return 0;
}
int strcpy(char *dest, char *src) {
  int i;
  for (i=0; src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  dest[i+=1]= '\0';

  return 1;
}
int strncmp(char *s1, char *s2, int n) {
  int i;
  for (i= 0; i*sizeof(char) < n; i++) {
    if (s1[i] != s2[i]) { return 1; }
  }
  return 0;
}
int strncpy(char *dest, char *src, int n) {
  int i;
  for (i=0; src[i] != '\0' && i*sizeof(char)<n; i++) {
    dest[i] = src[i];
  }
  dest[i+=1]= '\0';

  return 1;
}
int memset(char *dest, char c, int n) {
  int i;
  for (i = 0; i*sizeof(char) < n && dest[i] != '\0'; i++) {
    dest[i] = c;
  }
}

int memcpy(char *dest, char *src, int n) {
  int i;
  for (i=0; i*sizeof(char)<n; i++) {
    dest[i] = src[i];
  }
  dest[i+=1]= '\0';

  return 1;
}
