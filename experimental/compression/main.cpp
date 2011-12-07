
#include<assert.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<zlib.h>

int main()
{
  gzFile f = gzopen("proba.gz", "w");
  assert(f);
  char* msg = "Hello super world!";
  gzwrite(f, msg, strlen(msg));
  gzclose(f);
  return 0;
}
