#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define BILLION 1000000000ULL
#define CONV_SEC_TO_NS(x) ((x) * BILLION)

struct termios termios_stdin1, termios_stdout1;
struct termios termios_stdin2, termios_stdout2;

int ammo = 30;
int ammc = 30;

void app_sighandler(int arg) {
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_stdin1);
  tcsetattr(STDOUT_FILENO, TCSANOW, &termios_stdout1);
  _exit(0);
}

int main() {
  tcgetattr(STDIN_FILENO, &termios_stdin1);
  tcgetattr(STDOUT_FILENO, &termios_stdout1);
  termios_stdin2 = termios_stdin1;
  termios_stdout2 = termios_stdout1;

  int sigarr[] = {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGSEGV, SIGILL};
  int sigars = sizeof(sigarr)/sizeof(sigarr[0]);

  for (int i = 0; i < sigars; i++) {
    signal(sigarr[i], app_sighandler);
  }

  termios_stdin2.c_lflag &= ~(ICANON | ECHO);
  termios_stdout2.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &termios_stdin2);
  tcsetattr(STDOUT_FILENO, TCSANOW, &termios_stdout2);

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if (c == 'f') {
      if (ammo <= 0) {
        write(STDOUT_FILENO, "empty\n", 6);
      }
      else {
        write(STDOUT_FILENO, "pow!\n", 5);
      }
      if (ammo > 0) ammo--;
    }
    else if (c == 'r') {
      write(STDOUT_FILENO, "reloading...\n", 13);
      double seconds = 2.5;
      struct timespec req = {
        .tv_sec = (time_t)seconds,
        .tv_nsec = (long)(CONV_SEC_TO_NS(seconds - (time_t)seconds))
      };
      nanosleep(&req, NULL);
      write(STDOUT_FILENO, "done!\n", 6);
      ammo = ammc;
    }
  }

  return 0;
}
