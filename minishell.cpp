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


void builtin_history() { //Mostrar el historial de comandos ejecutados
    if (historial.empty()) {
        cout << "Historial vacío" << endl;
        return;
    }
    
    cout << "\n=== HISTORIAL DE COMANDOS ===" << endl;
    for (size_t i = 0; i < historial.size(); i++) {
        printf("%4zu  %s\n", i + 1, historial[i].c_str());
    }
    cout << "============================\n" << endl;
}

void builtin_meminfo() {   //Se mostrara las estadisticas 
    FILE* fp = fopen("/proc/self/status", "r");
    if (fp == NULL) {
        cerr << "meminfo: no se pudo abrir /proc/self/status" << endl;
        return;
    }
    
    char linea[256];
    cout << "\n=== ESTADÍSTICAS DE MEMORIA DEL PROCESO ===" << endl;
    
    while (fgets(linea, sizeof(linea), fp) != NULL) {
        if (strncmp(linea, "VmPeak:", 7) == 0 ||
            strncmp(linea, "VmSize:", 7) == 0 ||
            strncmp(linea, "VmRSS:", 6) == 0 ||
            strncmp(linea, "VmData:", 7) == 0 ||
            strncmp(linea, "VmStk:", 6) == 0 ||
            strncmp(linea, "VmExe:", 6) == 0) {
            cout << "  " << linea;
        }
    }
    
    cout << "\nDescripción:" << endl;
    cout << "  VmPeak: Pico máximo de memoria virtual usada" << endl;
    cout << "  VmSize: Tamaño actual de memoria virtual" << endl;
    cout << "  VmRSS:  Memoria física realmente en uso " << endl;
    cout << "  VmData: Memoria del heap (datos dinámicos)" << endl;
    cout << "  VmStk:  Memoria del stack" << endl;
    cout << "  VmExe:  Memoria del código ejecutable" << endl;
    cout << "==========================================\n" << endl;
    
    fclose(fp);
}


void builtin_alias(const vector<string>& args) { //Funcion para el comando "alias" para crear alias personalizados
    if (args.empty()) {
        if (aliases.empty()) {
            cout << "No hay alias definidos" << endl;
        } else {
            cout << "\nAlias definidos:" << endl;
            for (const auto& par : aliases) {
                cout << "  " << par.first << " -> " << par.second << endl;
            }
            cout << endl;
        }
        return;
    }
    
    string nombre, comando;
    
    if (args.size() >= 3 && args[1] == "=") {
        nombre = args[0];
        comando = args[2];
        
        for (size_t i = 3; i < args.size(); i++) {
            comando += " " + args[i];
        }
    } else {
        string argumento = args[0];
        size_t pos_igual = argumento.find('=');
        
        if (pos_igual == string::npos) {
            if (aliases.find(argumento) != aliases.end()) {
                cout << argumento << " -> " << aliases[argumento] << endl;
            } else {
                cerr << "alias: '" << argumento << "' no está definido" << endl;
            }
            return;
        }
        
        nombre = argumento.substr(0, pos_igual);
        comando = argumento.substr(pos_igual + 1);
    }
    
    if (comando.length() >= 2 && comando[0] == '\'' && comando[comando.length()-1] == '\'') {
        comando = comando.substr(1, comando.length() - 2);
    }
    
    if (nombre.empty() || comando.empty()) {
        cerr << "alias: formato incorrecto. Usa: alias nombre='comando'" << endl;
        return;
    }
    
    aliases[nombre] = comando;
    cout << "Alias creado: " << nombre << " -> " << comando << endl;
}

void builtin_unalias(const vector<string>& args) { //Para eliminar alias
    if (args.empty()) {
        cerr << "unalias: falta el nombre del alias" << endl;
        return;
    }
    
    string nombre = args[0];
    
    if (aliases.find(nombre) != aliases.end()) {
        aliases.erase(nombre);
        cout << "Alias '" << nombre << "' eliminado" << endl;
    } else {
        cerr << "unalias: '" << nombre << "' no está definido" << endl;
    }
}


bool es_ejecutable(const string& ruta) { //Verifica si una ruta corresponde a un archivo ejecutable
    struct stat st;
    if (stat(ruta.c_str(), &st) == 0) {
        return (st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH);
    }
    return false;
}

string resolver_ruta(const string& comando) { // Resuelve la ruta completa de un comando (absoluta o en /bin)
    if (comando[0] == '/') {
        if (es_ejecutable(comando)) {
            return comando;
        }
        return "";
    }
    
    string ruta_bin = "/bin/" + comando;
    if (es_ejecutable(ruta_bin)) {
        return ruta_bin;
    }
    
    return "";
}



int main(){


    return 0;
}