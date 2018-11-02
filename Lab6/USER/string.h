// same as string.c in kernel

/*
int strlen(char *s);
int strcmp(char *s1, char *s2);
int strcpy(char *dest, char *src);
int kstrcpy(char *dest, char *src);
int kstrcat(char *dest, char *src);
int atoi(char *s);
int geti();
int memcpy(char *dest, char *src, int size);
*/

//int kprintf(char *, ...);
int strlen(char *s)
{
  int i = 0;
  while(*s){
    i++; s++;
  }
  return i;
}

int strcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
    return 0;
  return(*s1 - *s2);

}

int strcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}
int kstrcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}
int kstrcat(char *dest, char *src)
{
  while(*dest)
    dest++;

  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}

int atoi(char *s)
{
  int v = 0;
  while (*s){
    v = 10*v + (*s - '0');
    s++;
  }
  //kprintf("v=%d\n", v);
  return v;
}

int geti()
{
  char s[16];
  ugetline(s);
  return atoi(s);
}

int memcpy(char *dest, char *src, int size)
{
  int i;
  for (i=0; i<size; i++){
    *dest++ = *src++;
  }
}
