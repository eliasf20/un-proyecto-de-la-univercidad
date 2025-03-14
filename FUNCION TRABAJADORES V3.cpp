#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <stdbool.h>

#define MAX_EMPLEADOS 100
#define NOMBRE_LEN 50
#define APELLIDO_LEN 50
#define CEDULA_LEN 9
#define TELEFONO_LEN 15
#define DIRECCION_LEN 100
#define DIAS_LEN 20
#define HORAS_LEN 11
#define ARCHIVO_DAT "trabajadores.dat"

typedef enum {
    CAJERO = 1,
    ADMINISTRADOR
} Rol;

typedef struct {
    char dias[DIAS_LEN];
    char horas[HORAS_LEN];
} Horario;

typedef struct {
    char nombre[NOMBRE_LEN];
    char apellido[APELLIDO_LEN];
    char cedula[CEDULA_LEN];
    char telefono[TELEFONO_LEN];
    char direccion[DIRECCION_LEN];
    Horario horario;
    int numero_empleado;
    Rol rol;
    float salario;
    int horas_trabajadas;
    float bonificaciones;
    float deducciones;
    float evaluacion_desempeno;
} Empleado;

Empleado empleados[MAX_EMPLEADOS];
int total_empleados = 0;

// Prototipos de funciones
void limpiar_buffer();
int validar_cedula(const char *cedula);
int validar_telefono(const char *telefono);
int validar_formato_hora(const char *hora);
int validar_rango_horas(const char *hora_entrada, const char *hora_salida);
int validar_dias(const char *dias);
void normalizar_nombre(char *str);
void cargar_datos();
void guardar_datos();
int buscar_por_numero(int numero);
int buscar_por_cedula(const char *cedula);
void pausar();
void registrar_empleado();
void mostrar_empleados();
void gestion_nomina();
void evaluar_desempeno();

int main() {
    setlocale(LC_ALL, "es_ES.UTF-8");  // Configurar locale para UTF-8
    cargar_datos();
    int opcion;
    
    do {
        printf("\n=== Sistema de Gestion ===\n");
        printf("1. Registrar empleado\n");
        printf("2. Listar empleados\n");
        printf("3. Calcular nomina\n");
        printf("4. Evaluar desempeño\n");
        printf("5. Salir\n");
        printf("Seleccione opción: ");
        scanf("%d", &opcion);
        limpiar_buffer();

        switch(opcion) {
            case 1: registrar_empleado(); break;
            case 2: mostrar_empleados(); break;
            case 3: gestion_nomina(); break;
            case 4: evaluar_desempeno(); break;
            case 5: printf("\nSaliendo del sistema...\n"); break;
            default: printf("\nOpción inválida\n");
        }
        pausar();
    } while(opcion != 5);

    guardar_datos();
    return 0;
}

void limpiar_buffer() {
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

int validar_cedula(const char *cedula) {
    int len = strlen(cedula);
    if(len < 7 || len > 8) return 0;
    for(int i = 0; i < len; i++) {
        if(!isdigit(cedula[i])) return 0;
    }
    return 1;
}

int validar_telefono(const char *telefono) {
    if(strlen(telefono) < 10 || strlen(telefono) > 11) return 0;
    for(int i = 0; telefono[i]; i++) {
        if(!isdigit(telefono[i])) return 0;
    }
    return 1;
}

int validar_formato_hora(const char *hora) {
    if(strlen(hora) != 5) return 0;
    if(!isdigit(hora[0]) || !isdigit(hora[1]) || 
       hora[2] != ':' ||
       !isdigit(hora[3]) || !isdigit(hora[4])) return 0;
    
    int horas = atoi(hora);
    int minutos = atoi(hora + 3);
    
    return (horas >= 0 && horas <= 23) && 
           (minutos >= 0 && minutos <= 59);
}

int validar_rango_horas(const char *hora_entrada, const char *hora_salida) {
    int h_entrada = atoi(hora_entrada);
    int m_entrada = atoi(hora_entrada + 3);
    int h_salida = atoi(hora_salida);
    int m_salida = atoi(hora_salida + 3);
    
    return (h_salida > h_entrada) || 
          (h_salida == h_entrada && m_salida > m_entrada);
}

int validar_dias(const char *dias) {
    for(int i = 0; dias[i]; i++) {
        char c = toupper(dias[i]);
        if(c == 'L' || c == 'M' || c == 'X' || 
           c == 'J' || c == 'V' || c == 'S' || 
           c == 'D' || c == ',' || c == ' ') continue;
        return 0;
    }
    return 1;
}

void normalizar_nombre(char *str) {
    if(strlen(str) == 0) return;
    str[0] = toupper(str[0]);
    for(int i = 1; str[i]; i++) {
        str[i] = tolower(str[i]);
        if((str[i-1] == ' ' || str[i-1] == '-' || str[i-1] == '\'') && isalpha(str[i])) {
            str[i] = toupper(str[i]);
        }
    }
}

void cargar_datos() {
    FILE *archivo = fopen(ARCHIVO_DAT, "rb");
    if(!archivo) return;

    fseek(archivo, 0, SEEK_END);
    long bytes = ftell(archivo);
    rewind(archivo);

    total_empleados = bytes / sizeof(Empleado);
    
    if(total_empleados > MAX_EMPLEADOS) {
        printf("\nAdvertencia: El archivo contiene más empleados que el máximo permitido\n");
        total_empleados = MAX_EMPLEADOS;
    }
    
    size_t leidos = fread(empleados, sizeof(Empleado), total_empleados, archivo);
    if(leidos != (size_t)total_empleados) {
        printf("\nError al leer algunos registros\n");
    }
    
    fclose(archivo);
}

void guardar_datos() {
    FILE *archivo = fopen(ARCHIVO_DAT, "wb");
    if(!archivo) {
        printf("\nError al crear archivo de datos!\n");
        return;
    }
    
    size_t escritos = fwrite(empleados, sizeof(Empleado), total_empleados, archivo);
    if(escritos != (size_t)total_empleados) {
        printf("\nError al guardar algunos registros\n");
    }
    
    fclose(archivo);
}

int buscar_por_numero(int numero) {
    for(int i = 0; i < total_empleados; i++) {
        if(empleados[i].numero_empleado == numero) {
            return i;
        }
    }
    return -1;
}

int buscar_por_cedula(const char *cedula) {
    for(int i = 0; i < total_empleados; i++) {
        if(strcmp(empleados[i].cedula, cedula) == 0) {
            return i;
        }
    }
    return -1;
}

void pausar() {
    printf("\nPresione Enter para continuar...");
    limpiar_buffer();
    getchar();
}

void registrar_empleado() {
    if(total_empleados >= MAX_EMPLEADOS) {
        printf("\n¡Capacidad maxima alcanzada!\n");
        return;
    }

    Empleado nuevo;
    memset(&nuevo, 0, sizeof(Empleado));

    printf("\n--- Registro de Nuevo Empleado ---\n");

    // Nombre
    printf("Nombre: ");
    fgets(nuevo.nombre, NOMBRE_LEN, stdin);
    nuevo.nombre[strcspn(nuevo.nombre, "\n")] = 0;
    normalizar_nombre(nuevo.nombre);

    // Apellido
    printf("Apellido: ");
    fgets(nuevo.apellido, APELLIDO_LEN, stdin);
    nuevo.apellido[strcspn(nuevo.apellido, "\n")] = 0;
    normalizar_nombre(nuevo.apellido);

    // Cédula
    char cedula_temp[CEDULA_LEN];
    do {
        printf("Cedula (7 u 8 dígitos): ");
        fgets(cedula_temp, CEDULA_LEN, stdin);
        cedula_temp[strcspn(cedula_temp, "\n")] = 0;
        
        if(!validar_cedula(cedula_temp)) {
            printf("Cedula inválida. Debe contener 7 u 8 dígitos\n");
        } else if(buscar_por_cedula(cedula_temp) != -1) {
            printf("¡Esta cedula ya está registrada!\n");
        } else {
            strcpy(nuevo.cedula, cedula_temp);
            break;
        }
    } while(1);

    // Teléfono
    do {
        printf("Telefono (ej: 04141234567): ");
        fgets(nuevo.telefono, TELEFONO_LEN, stdin);
        nuevo.telefono[strcspn(nuevo.telefono, "\n")] = 0;
    } while(!validar_telefono(nuevo.telefono));

    // Dirección
    printf("Direccion: ");
    fgets(nuevo.direccion, DIRECCION_LEN, stdin);
    nuevo.direccion[strcspn(nuevo.direccion, "\n")] = 0;

    // Horario
    char dias_temp[DIAS_LEN];
    char hora_entrada[6], hora_salida[6];
    
    do {
        printf("\nDias laborales (L: Lunes, M: Martes, X: Miércoles, J: Jueves, V: Viernes, S: Sábado, D: Domingo)\n");
        printf("Ejemplo: L,M,X,J,V\n> ");
        fgets(dias_temp, DIAS_LEN, stdin);
        dias_temp[strcspn(dias_temp, "\n")] = 0;
    } while(!validar_dias(dias_temp));

    do {
        printf("\nHora de entrada (HH:MM): ");
        fgets(hora_entrada, 6, stdin);
        limpiar_buffer();
        
        printf("Hora de salida (HH:MM): ");
        fgets(hora_salida, 6, stdin);
        limpiar_buffer();
        
    } while(!validar_formato_hora(hora_entrada) || 
           !validar_formato_hora(hora_salida) ||
           !validar_rango_horas(hora_entrada, hora_salida));

    snprintf(nuevo.horario.dias, DIAS_LEN, "%s", dias_temp);
    snprintf(nuevo.horario.horas, HORAS_LEN, "%s-%s", hora_entrada, hora_salida);

    // Número de empleado
    while(1) {
        printf("Numero de empleado: ");
        char entrada[20];
        fgets(entrada, sizeof(entrada), stdin);
        if(sscanf(entrada, "%d", &nuevo.numero_empleado) == 1 && 
           buscar_por_numero(nuevo.numero_empleado) == -1) {
            break;
        }
        printf("Numero invalido o ya existe\n");
    }

    // Rol
    int rol;
    do {
        printf("Rol (1: Cajero, 2: Administrador): ");
        scanf("%d", &rol);
        limpiar_buffer();
    } while(rol < 1 || rol > 2);
    nuevo.rol = (Rol)rol;

    // Salario
    while(1) {
        printf("Salario base: ");
        char entrada[20];
        fgets(entrada, sizeof(entrada), stdin);
        if(sscanf(entrada, "%f", &nuevo.salario) == 1 && nuevo.salario > 0) break;
        printf("Salario invalido\n");
    }

    // Inicializar otros campos
    nuevo.horas_trabajadas = 0;
    nuevo.bonificaciones = 0;
    nuevo.deducciones = 0;
    nuevo.evaluacion_desempeno = 0;

    empleados[total_empleados++] = nuevo;
    guardar_datos();
    printf("\n¡Empleado registrado exitosamente!\n");
}

void mostrar_empleados() {
    if(total_empleados == 0) {
        printf("\nNo hay empleados registrados\n");
        return;
    }

    printf("\n--- Listado de Empleados (%d) ---\n", total_empleados);
    for(int i = 0; i < total_empleados; i++) {
        printf("\n[%d] %s %s", empleados[i].numero_empleado, 
               empleados[i].nombre, empleados[i].apellido);
        printf("\nCedula: %s", empleados[i].cedula);
        printf("\nTelefono: %s", empleados[i].telefono);
        printf("\nDireccion: %s", empleados[i].direccion);
        printf("\nDias laborales: %s", empleados[i].horario.dias);
        printf("\nHorario: %s", empleados[i].horario.horas);
        printf("\nRol: %s", empleados[i].rol == CAJERO ? "Cajero" : "Administrador");
        printf("\nSalario: $%.2f", empleados[i].salario);
        printf("\nEvaluacion: %.1f/10\n", empleados[i].evaluacion_desempeno);
    }
}

void gestion_nomina() {
    if(total_empleados == 0) {
        printf("\nNo hay empleados registrados\n");
        return;
    }

    int numero;
    printf("\nIngrese número de empleado: ");
    scanf("%d", &numero);
    limpiar_buffer();
    
    int indice = buscar_por_numero(numero);
    if(indice == -1) {
        printf("Empleado no encontrado\n");
        return;
    }

    Empleado *emp = &empleados[indice];
    printf("\n--- Nómina de %s %s ---\n", emp->nombre, emp->apellido);
    
    float total = emp->salario + emp->bonificaciones - emp->deducciones;
    printf("Salario base: $%.2f\n", emp->salario);
    printf("Bonificaciones: $%.2f\n", emp->bonificaciones);
    printf("Deducciones: $%.2f\n", emp->deducciones);
    printf("Total a pagar: $%.2f\n", total);
}

void evaluar_desempeno() {
    if(total_empleados == 0) {
        printf("\nNo hay empleados registrados\n");
        return;
    }

    int numero;
    float evaluacion;
    printf("\nIngrese número de empleado: ");
    scanf("%d", &numero);
    limpiar_buffer();
    
    int indice = buscar_por_numero(numero);
    if(indice == -1) {
        printf("Empleado no encontrado\n");
        return;
    }

    do {
        printf("Ingrese evaluacion (0.0-10.0): ");
        scanf("%f", &evaluacion);
        limpiar_buffer();
        
        if(evaluacion < 0 || evaluacion > 10) {
            printf("Valor fuera de rango. ");
        }
    } while(evaluacion < 0 || evaluacion > 10);

    empleados[indice].evaluacion_desempeno = evaluacion;
    guardar_datos();
    printf("\nEvaluacion actualizada exitosamente!\n");
}