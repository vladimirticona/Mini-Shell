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


void builtin_pwd() {	//Funcion para el comando "pwd" para mostrar el directorio actual
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << cwd << endl;
    } else {
        cerr << "pwd: error al obtener el directorio actual" << endl;
    }
}

void builtin_help() {  //Lo que se imprimira cuando se escriba "help" o "ayuda"
    cout << "\n=== MINI-SHELL - AYUDA ===" << endl;
    cout << "\nComandos internos:" << endl;
    cout << "  cd                    - Cambiar directorio" << endl;
    cout << "  pwd                   - Mostrar directorio actual" << endl;
    cout << "  help                  - Mostrar ayuda" << endl;
    cout << "  history               - Mostrar historial de comandos" << endl;
    cout << "  meminfo               - Mostrar estadísticas de memoria del proceso" << endl;
    cout << "  alias [nombre='comando']  - Crear alias" << endl;
    cout << "  unalias [nombre]      - Eliminar un alias" << endl;
    cout << "  salir                 - Salir de la shell" << endl;
    cout << "\nCaracterísticas:" << endl;
    cout << "  comando > archivo     - Redirigir salida estándar a archivo" << endl;
    cout << "  /ruta/absoluta/cmd    - Ejecutar comando con ruta absoluta" << endl;
    cout << "  comando               - Buscar comando en /bin/" << endl;
    cout << "\nEjemplos:" << endl;
    cout << "  ls > listado.txt" << endl;
    cout << "  alias ll='ls'" << endl;
    cout << "  /usr/bin/whoami" << endl;
    cout << "  cat /etc/hostname" << endl;
    cout << "========================\n" << endl;
}



int main(){


    return 0;
}