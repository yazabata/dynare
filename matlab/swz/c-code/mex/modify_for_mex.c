
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdarg.h>
#include <string.h>

#if defined(MATLAB_MEX_FILE) || defined(OCTAVE_MEX_FILE)
#include <dynmex.h>
#endif

int constant_seed;

/*
int
swz_printf(char *msg, ...)
{
  int ret;
  va_list ap;
  va_start(ap, msg);

#if defined(MATLAB_MEX_FILE)
  ret = mexPrintf(msg, ap);
#elif defined(OCTAVE_MEX_FILE)
  mexPrintf(msg, ap);
  ret = 1;
#else
  ret = vprintf(msg, ap);
#endif

  va_end(ap);
  return ret;
}

int
swz_fprintf_stdout(char *msg, ...)
{
  int ret;
  va_list ap;
  va_start(ap, msg);

#if defined(MATLAB_MEX_FILE)
  ret = mexPrintf(msg, ap);
  mexEvalString("pause(.001);");
  //  mexEvalString("drawnow;");
#elif defined(OCTAVE_MEX_FILE)
  mexPrintf(msg, ap);
  ret = 1;
#else
  ret = vprintf(msg, ap);
#endif

  va_end(ap);
  return ret;
}
*/
void
swz_fprintf_err(char *str, ...)
{
  char *whole_str=(char*)NULL;
  char *msg_truncated = ".....MSG TRUNCATED\n";
  int num_args = 0;
  va_list ap;

  va_start(ap, str);

#if defined(MATLAB_MEX_FILE) || defined(OCTAVE_MEX_FILE)
  mexPrintf(str, ap);
  /*  mexEvalString("drawnow;");*/
#else
  vfprintf(stderr, str, ap);
#endif

  va_end(ap);
}

void
swzExit(int status)
{
#if defined(MATLAB_MEX_FILE) || defined(OCTAVE_MEX_FILE)
  mexErrMsgTxt("Error in mexfile.\n");
#else
  exit(status);
#endif
}
