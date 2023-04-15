#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

string trim(const string &s)
{
    string::const_iterator left = s.begin();
    while (left != s.end() && std::isspace(*left))
    {
        ++left;
    }

    string::const_reverse_iterator right = s.rbegin();
    while (right != s.rend() && std::isspace(*right))
    {
        ++right;
    }

    return string(left, right.base());
}

vector<string> tokenize(const string &input)
{
    vector<string> tokens;
    istringstream iss(input);
    string token;
    while (iss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

char **createArgv(const vector<string> &tokens)
{
    char **args = new char *[tokens.size() + 1];
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        args[i] = new char[tokens[i].size() + 1];
        strcpy(args[i], tokens[i].c_str());
    }
    args[tokens.size()] = nullptr;
    return args;
}

void exitShell()
{
    exit(EXIT_SUCCESS);
}

void printPID()
{
    cout << "Current PID: " << getpid() << endl;
}

void printHelp()
{
    cout << "Available commands:" << endl;
    cout << "  exit  - Terminate the shell" << endl;
    cout << "  print - Print the current PID" << endl;
    cout << "  help  - Display this help information" << endl;
}

void execute_command(const string &input)
{
    if (input.empty() || input.length() > 100)
    {
        cerr << "Invalid input" << endl;
        return;
    }

    vector<string> tokens = tokenize(input);

    if (tokens.empty())
    {
        return;
    }

    // check if command is an internal command
    if (tokens[0] == "exit")
    {
        exitShell();
        return;
    }
    else if (tokens[0] == "print")
    {
        printPID();
        return;
    }
    else if (tokens[0] == "help")
    {
        printHelp();
        return;
    }

    // command is an external command, so fork a child process
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // child process
        char **args = createArgv(tokens);
        if (execvp(args[0], args) == -1)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        // parent process
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    string input;

    while (true)
    {
        cout << "$ ";
        getline(cin, input);

        // Truncate the input if it is too long
        if (input.length() > 100)
        {
            input = input.substr(0, 100);
            cout << "Input truncated to 100 characters" << endl;
            cout << input << endl;
        }

        // Split the input into token groups based on the "&" character
        istringstream iss(input);
        string token_group;

        while (getline(iss, token_group, '&'))
        {
            // Remove leading and trailing whitespace from the token group
            token_group = trim(token_group);

            if (!token_group.empty())
            {
                // Execute each token group in a separate child process
                pid_t pid = fork();

                if (pid == 0)
                {
                    // Child process
                    execute_command(token_group);
                    exit(EXIT_SUCCESS);
                }
                else if (pid > 0)
                {
                    // Parent process
                    // Do nothing
                }
                else
                {
                    // Failed to fork
                    cout << "Failed to fork" << endl;
                }
            }
        }

        // Print the prompt for the next command
        if (!input.empty())
        {
            cout << "$ ";
        }
    }

    return 0;
}
