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

void ejecutar_comando(const vector<string>& tokens) { //Ejecuta comandos externos con soporte para redireccion de salida
    if (tokens.empty()) return;
    
    string archivo_salida = "";
    vector<string> args_comando;
    bool hay_redireccion = false;
    
    for (size_t i = 0; i < tokens.size(); i++) { // Detectar y procesar redireccion de salida
        if (tokens[i] == ">") {
            hay_redireccion = true;
            if (i + 1 < tokens.size()) {
                archivo_salida = tokens[i + 1];
                i++;
            } else {
                cerr << "Error: falta el nombre del archivo después de '>'" << endl;
                return;
            }
        } else {
            args_comando.push_back(tokens[i]);
        }
    }
    
    if (args_comando.empty()) return;
    // Resuelve la ruta ejecutable del comando
    string ruta_ejecutable = resolver_ruta(args_comando[0]);
    if (ruta_ejecutable.empty()) {
        cerr << "Error: comando '" << args_comando[0] << "' no encontrado" << endl;
        return;
    }
    // Preparar argumentos para execv
    char** argv = new char*[args_comando.size() + 1];
    for (size_t i = 0; i < args_comando.size(); i++) {
        argv[i] = strdup(args_comando[i].c_str());
    }
    argv[args_comando.size()] = NULL;
    
    pid_t pid = fork();
    // Manejo de errores al crear proceso hijo
    if (pid == -1) {
        cerr << "Error: no se pudo crear el proceso hijo" << endl;
        for (size_t i = 0; i < args_comando.size(); i++) {
            free(argv[i]);
        }
        delete[] argv;
        return;
    }
     // Codigo del proceso hijo
    if (pid == 0) {
        if (hay_redireccion) {
            int fd = open(archivo_salida.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                cerr << "Error: no se pudo abrir el archivo '" << archivo_salida << "'" << endl;
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd, STDOUT_FILENO) == -1) {
                cerr << "Error: fallo en la redirección" << endl;
                close(fd);
                exit(EXIT_FAILURE);
            }
            
            close(fd);
        }
        
        execv(ruta_ejecutable.c_str(), argv);
        
        cerr << "Error: no se pudo ejecutar '" << args_comando[0] << "'" << endl;
        exit(EXIT_FAILURE);
    } else { // Codigo del proceso padre
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Error: problema al esperar al proceso hijo" << endl;
        }
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                cerr << "El comando terminó con código de error: " << exit_code << endl;
            }
        } else if (WIFSIGNALED(status)) {
            cerr << "El comando fue terminado por una señal" << endl;
        }
    }
    
    for (size_t i = 0; i < args_comando.size(); i++) {
        free(argv[i]);
    }
    delete[] argv;
}


int main(){
    string linea;
    
    cout << "========================================" << endl;
    cout << "   Bienvenido a Mini-Shell" << endl;
    cout << "   Escribe 'help' para ver la ayuda" << endl;
    cout << "   Escribe 'salir' para terminar" << endl;
    cout << "========================================\n" << endl;
    
    while (true) {
        mostrar_prompt();
        
        if (!getline(cin, linea)) {
            cout << "\nSaliendo..." << endl;
            break;
        }
        
        if (linea.empty() || linea.find_first_not_of(" \t") == string::npos) {
            continue;
        }
        
        historial.push_back(linea);
        
        vector<string> tokens = tokenizar(linea);
        
        if (tokens.empty()) continue;
        
        string comando = tokens[0];
        vector<string> args(tokens.begin() + 1, tokens.end());
        
        if (aliases.find(comando) != aliases.end()) {
            string comando_expandido = aliases[comando];
            vector<string> tokens_expandidos = tokenizar(comando_expandido);
            tokens_expandidos.insert(tokens_expandidos.end(), args.begin(), args.end());
            tokens = tokens_expandidos;
            comando = tokens[0];
            args = vector<string>(tokens.begin() + 1, tokens.end());
        }
        
        // Verificar y ejecutar comandos internos
        if (comando == "salir" || comando == "exit" || comando == "quit") {
            cout << "Saliendo....." << endl;
            break;
        }
        else if (comando == "cd") {
            builtin_cd(args);
        }
        else if (comando == "pwd") {
            builtin_pwd();
        }
        else if (comando == "help" || comando == "ayuda") {
            builtin_help();
        }
        else if (comando == "history" || comando == "historial") {
            builtin_history();
        }
        else if (comando == "meminfo") {
            builtin_meminfo();
        }
        else if (comando == "alias") {
            builtin_alias(args);
        }
        else if (comando == "unalias") {
            builtin_unalias(args);
        }
        else { // Ejecuta comandos externos del sistema
            ejecutar_comando(tokens);
        }
    }

    return 0;
}