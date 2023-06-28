#ifndef IODEF_H
#define IODEF_H


#ifndef SLVFNAME_LEN_MAX
#define SLVFNAME_LEN_MAX 64
#endif

#ifndef SHELL_PATH_LEN
#define SHELL_PATH_LEN 256
#endif

#ifndef MQUEUE_NAME_LEN
#define MQUEUE_NAME_LEN 512
#endif

#ifndef SHELL_IO_LEN
#define SHELL_IO_LEN 256000
#endif

#ifndef MQ_FILE_FMT
#define MQ_FILE_FMT "%d.btymq"
#endif

#define STDIN_FD fileno(stdin)
#define STDOUT_FD fileno(stdout)
#define STDERR_FD fileno(stderr)

#define GET_MQ_NAME(BUF, PID) snprintf(BUF, MQUEUE_NAME_LEN, MQ_FILE_FMT, PID)


#define PIPE_READ(PIPE, ...)                                                   \
  do {                                                                         \
    close(PIPE[1]);                                                            \
    read(PIPE[0], __VA_ARGS__);                                                \
    close(PIPE[0]);                                                            \
  } while (0)
#define PIPE_WRITE(PIPE, ...)                                                  \
  do {                                                                         \
    close(PIPE[0]);                                                            \
    write(PIPE[1], __VA_ARGS__);                                               \
    close(PIPE[1]);                                                            \
  } while (0)

#define READ_AND_CLOSE(FD, ...)                                                \
  do {                                                                         \
    read(FD, __VA_ARGS__);                                                     \
    close(FD);                                                                 \
  } while (0)
#define WRITE_AND_CLOSE(FD, ...)                                               \
  do {                                                                         \
    write(FD, __VA_ARGS__);                                                    \
    close(FD);                                                                 \
  } while (0)

#endif