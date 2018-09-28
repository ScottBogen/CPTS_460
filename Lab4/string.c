int strlen(char *s);
int strcmp(char *s1, char *s2);
int strcpy(char *dest, char *src);
int strncpy(char *dest, char *src, int n);
int strncmp(char *s1, char *s2, int n);
int memcpy(char *dest, char *src, int n);
int memset(char *dest, char c, int n);
int atoi(char *str);
int geti();

// geti() returns an integer from KBD
// HINT: gets(char *s); then return atoi(s);

int geti() {
  char str[32];
  kgets(str);
  printf("\n");
  return atoi(str);
}

int atoi(char *str)
{
    int sum = 0, i = 0;

    for (int i = 0; str[i] != '\0'; ++i)
        sum = sum*10 + str[i] - '0';

    return sum;
}

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
