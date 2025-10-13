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

vector<string> tokenizar(const string& linea) {  //Funcion para separar la linea de entrada en tokens
    vector<string> tokens;
    string token = "";
    
    for (size_t i = 0; i < linea.length(); i++) {
        if (linea[i] == ' ' || linea[i] == '\t') {
            if (!token.empty()) {
                tokens.push_back(token);
                token = "";
            }
        } else {
            token += linea[i];
        }
    }
    
    if (!token.empty()) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void builtin_cd(const vector<string>& args) {	//Funcion para el comando "cd" para cambiar de directorio
    if (args.size() == 0) {
        const char* home = getenv("HOME");
        if (home == NULL) {
            cerr << "cd: no se pudo obtener el directorio HOME" << endl;
            return;
        }
        if (chdir(home) == -1) {
            cerr << "cd: " << strerror(errno) << endl;
        }
    } else {
        if (chdir(args[0].c_str()) == -1) {
            cerr << "cd: " << args[0] << ": " << strerror(errno) << endl;
        }
    }
}




int main(){


    return 0;
}