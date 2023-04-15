#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main()
{
  string input;

  while (true)
  {
    cout << "$ ";
    getline(cin, input);

    istringstream iss(input);
    string command;
    iss >> command;

    if (command == "exit")
    {
      exit(1);
    }
    else if (command == "print")
    {
      // TODO: Print the current PID
    }
    else if (command == "help")
    {
      // TODO: Print help information
    }
    else
    {
      // Execute an external command
      pid_t pid = fork();

      if (pid == 0)
      {
        // Child process
        // Convert the command string to a C string
        char *cmd = const_cast<char *>(command.c_str());

        // Convert the arguments to an array of C strings
        int i = 0;
        char *args[100];

        args[i++] = cmd;

        string arg;
        while (iss >> arg && i < 100)
        {
          args[i++] = const_cast<char *>(arg.c_str());
        }

        args[i] = nullptr;

        // Execute the command
        execvp(cmd, args);

        // If execvp returns, it must have failed
        cout << "Command not found: " << command << endl;
        exit(EXIT_FAILURE);
      }
      else if (pid > 0)
      {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
      }
      else
      {
        // Failed to fork
        cout << "Failed to fork" << endl;
      }
    }
  }

  return 0;
}