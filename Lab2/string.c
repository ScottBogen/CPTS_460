/*
int strlen(char *s) : return length of string
int strcmp(char *s1, char *s2): return 0 if same; nonzero if not
int strcpy(char *dest, char *src) : copies string src to dest
int strncpy(char *dest, char *src, int n) : copies only n bytes
int strncmp(char *s1, char *s2, int n) : compares n bytes
int memcpy(char *dest, char *src, int n) : copies b bytes from src to dest
int memset(char *dest, char c, int n): fills dest with n CONSTANT bytes of c
*/

#include <stdio.h>


int mstrlen(char *s);
int mstrcmp(char *s1, char *s2);
int mstrcpy(char *dest, char *src);
int mstrncpy(char *dest, char *src, int n);
int mstrncmp(char *s1, char *s2, int n);
int mmemcpy(char *dest, char *src, int n);
int mmemset(char *dest, char c, int n);

int main() {

  char s[20] = "Computer";
  char d[20];
  mmemset(d,0,20);
  mmemcpy(d, s, 4);
  printf("%s\n",d);

  return 0;
}

int mstrlen(char *s) {
  int sum = 0;
  while(*s++) {
    sum++;
  }
  return sum;
}
int mstrcmp(char *s1, char *s2) {
  if (mstrlen(s1) != mstrlen(s2)) { return 1; }
  for (int i = 0; i < mstrlen(s1); i++) {
    if (s1[i] != s2[i]) { return 1; }
  }
  return 0;
}
int mstrcpy(char *dest, char *src) {
  int i;
  for (i=0; src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  dest[i+=1]= '\0';

  return 1;
}
int mstrncmp(char *s1, char *s2, int n) {
  for (int i = 0; i*sizeof(char) < n; i++) {
    if (s1[i] != s2[i]) { return 1; }
  }
  return 0;
}
int mstrncpy(char *dest, char *src, int n) {
  int i;
  for (i=0; src[i] != '\0' && i*sizeof(char)<n; i++) {
    dest[i] = src[i];
  }
  dest[i+=1]= '\0';

  return 1;
}
int mmemset(char *dest, char c, int n) {
  for (int i = 0; i*sizeof(char) < n && dest[i] != '\0'; i++) {
    dest[i] = c;
  }
}

int mmemcpy(char *dest, char *src, int n) {
  int i;
  for (i=0; i*sizeof(char)<n; i++) {
    dest[i] = src[i];
  }
  dest[i+=1]= '\0';

  return 1;
}
