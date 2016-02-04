#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string>
#include <iostream>
#include <vector>
#include <sstream>


void exec_cmd(std::string cmd, bool f_finish)
{
    int pfd[2];
    pipe(pfd);

    pid_t pch = fork();

    if( !pch ){     // child process
        printf("Child PID = %i.\n", getpid());
        printf("Command = %s.\n",cmd.c_str());

        std::stringstream sstr;
        sstr << cmd;
        std::string sub_cmd;

        std::vector<std::string> scmd;
        while ( std::getline(sstr, sub_cmd, ' ') ){
            printf("\tcommand = .%s.\t%i\n",sub_cmd.c_str(), sub_cmd.size());
            if (sub_cmd.size())
                scmd.push_back( sub_cmd );
        }

        close(STDOUT_FILENO);
        FILE* fd = fopen("result.out", "w");
        if( f_finish)
            dup2(fileno(fd), STDOUT_FILENO);
        else
            dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);
        close(pfd[1]);

        char **arg = new char *[scmd.size()+1];
        for (unsigned int i = 0; i < scmd.size(); i++)
            arg[i] = const_cast<char*>(scmd[i].c_str());
        arg[scmd.size()] = 0;

        execvp( arg[0], arg);
        fclose(fd);

        printf("child finished ...\n");
    } else {    // parent process
        dup2(pfd[0], STDIN_FILENO);
        close(pfd[0]);
        close(pfd[1]);

        printf("Parent PID = %i\n", getpid());
        wait(NULL);
        printf("Parent PID = %i finished\n", getpid());
    }
}


int main(int argc, char **argv){

    std::string inString;
    std::getline(std::cin, inString);


    std::vector<std::string> list_cmd;
    std::stringstream sstr(inString);
    std::string cmd;
    char delim = '|';

    bool f_start = true;
    while ( std::getline(sstr, cmd, delim) ){
        list_cmd.push_back(cmd);
    }

    for(int i = 0; i < list_cmd.size(); i++){
        bool f_finish = i == list_cmd.size()-1 ? true : false;
        std::string str = list_cmd[i];
        std::cout << "\nExecute command: " << str << "." << std::endl;
        std::cout << "f_finish = " << f_finish << std::endl;
        exec_cmd (str, f_finish);
    }

    printf("\nFinished process\n");

    return 0;
}

