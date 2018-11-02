int exec(char *cmdline) // cmdline=VA in Uspace
{
  // cmdline = cmd arg1 .... argn
  // load /bin/cmd to Umode iamge; passing cmdline to new image
  // let caller return to VA(0) in Umode, with cmdline pointed by R0
}
