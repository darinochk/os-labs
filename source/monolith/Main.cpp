#include <sys/wait.h>
#include <unistd.h>

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> parseCommand(const string& cmd) {
  vector<string> args;
  stringstream ss(cmd);
  string token;

  while (ss >> token) {
    args.push_back(token);
  }

  return args;
}

int main() {
  string command;
  time_t start_time = time(nullptr);
  while (true) {
    cout << "shell> ";

    getline(cin, command);

    if (command == "exit") {
      time_t end_time = time(nullptr);

      double elapsed_time = difftime(end_time, start_time);
      cout << "Program executed in " << elapsed_time << " seconds." << endl;
      break;
    }

    vector<string> args = parseCommand(command);

    if (args.empty()) {
      continue;
    }

    vector<char*> execArgs;
    for (const string& arg : args) {
      execArgs.push_back(const_cast<char*>(arg.c_str()));
    }
    execArgs.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
      perror("Fork failed");
      exit(1);
    } else if (pid == 0) {
      // дочерний процесс
      if (execvp(execArgs[0], execArgs.data()) == -1) {
        perror("Execvp failed");
        exit(1);
      }
    } else {
      // родительский процесс
      int status;
      waitpid(pid, &status, 0);
    }
  }

  return 0;
}
