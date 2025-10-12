//Implementacion de librerias que usaremos
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

vector<string> historial;
map<string, string> aliases;


void mostrar_prompt() {  
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << "mini-shell [" << cwd << "]> ";
    } else {
        cout << "mini-shell> ";
    }
    cout.flush();
}


int main(){


    return 0;
}