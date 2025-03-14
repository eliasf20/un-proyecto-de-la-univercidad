#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define ARCHIVO_VENTAS "ventas.dat"
#define ARCHIVO_CIERRES "caja.dat"
#define MAX_USUARIOS 5
#define MAX_REGISTROS 1000

// Estructuras compatibles con sistemas existentes
typedef struct {
    int id;
    char codigo_producto[20];
    int cantidad;
    time_t fecha;
    char tipo;
    char metodo_pago[20];
    double total;
    char trabajador[50];
    int num_caja;
} Transaccion;

typedef struct {
    time_t inicio;
    time_t fin;
    char trabajador[50];
    int num_caja;
    double total_ventas;
    double efectivo_inicial;
    double efectivo_final;
    double diferencia;
    char estado[20];
} RegistroCaja;

typedef struct {
    char usuario[50];
    char password[50];
    int permiso; // 1=Admin, 2=Cajero
} Usuario;

// Variables globales
RegistroCaja registros[MAX_REGISTROS];
int num_registros = 0;
Usuario usuarios[MAX_USUARIOS] = {
    {"admin", "admin123", 1},
    {"caja1", "caja123", 2},
    {"caja2", "caja456", 2}
};
Usuario usuario_actual;

// Prototipos de funciones
void mostrar_menu();
void movimientos_caja();
void conciliacion();
void reportes_caja();
void registrar_movimiento();
void autenticar_usuario();
void cargar_datos();
void guardar_datos();
double calcular_total_ventas(time_t inicio, time_t fin, int num_caja);
void generar_reporte_diario();

void autenticar_usuario() {
    char usuario[50], password[50];
    int intentos = 0;
    
    do {
        printf("\n=== AUTENTICACION ===");
        printf("\nUsuario: ");
        scanf("%s", usuario);
        printf("Password: ");
        scanf("%s", password);
        
        for(int i = 0; i < MAX_USUARIOS; i++) {
            if(strcmp(usuarios[i].usuario, usuario) == 0 && 
               strcmp(usuarios[i].password, password) == 0) {
                usuario_actual = usuarios[i];
                printf("\nBienvenido %s!\n", usuario_actual.usuario);
                return;
            }
        }
        printf("\nCredenciales incorrectas!\n");
        intentos++;
    } while(intentos < 3);
    
    printf("Demasiados intentos fallidos. Saliendo...\n");
    exit(0);
}

void cargar_datos() {
    FILE *f = fopen(ARCHIVO_CIERRES, "rb");
    if(f) {
        num_registros = fread(registros, sizeof(RegistroCaja), MAX_REGISTROS, f);
        fclose(f);
    }
}

void guardar_datos() {
    FILE *f = fopen(ARCHIVO_CIERRES, "wb");
    if(f) {
        fwrite(registros, sizeof(RegistroCaja), num_registros, f);
        fclose(f);
    }
}

void movimientos_caja() {
    if(usuario_actual.permiso != 2) {
        printf("Acceso denegado! Solo para cajeros\n");
        return;
    }
    
    RegistroCaja nuevo;
    time(&nuevo.inicio);
    strcpy(nuevo.trabajador, usuario_actual.usuario);
    
    printf("\n=== NUEVO TURNO DE CAJA ===");
    printf("\nNumero de caja: ");
    scanf("%d", &nuevo.num_caja);
    printf("Efectivo inicial: ");
    scanf("%lf", &nuevo.efectivo_inicial);
    
    printf("\nTurno iniciado: %s", ctime(&nuevo.inicio));
    
    // Registrar automáticamente el cierre al salir
    time(&nuevo.fin);
    nuevo.total_ventas = calcular_total_ventas(nuevo.inicio, nuevo.fin, nuevo.num_caja);
    
    printf("Efectivo final: ");
    scanf("%lf", &nuevo.efectivo_final);
    
    nuevo.diferencia = nuevo.efectivo_final - (nuevo.efectivo_inicial + nuevo.total_ventas);
    strcpy(nuevo.estado, (nuevo.diferencia == 0) ? "CONCILIADO" : "DESCUADRADO");
    
    registros[num_registros++] = nuevo;
    guardar_datos();
    
    printf("\nRegistro guardado exitosamente!\n");
}

double calcular_total_ventas(time_t inicio, time_t fin, int num_caja) {
    double total = 0;
    FILE *f = fopen(ARCHIVO_VENTAS, "rb");
    Transaccion trans;
    
    if(f) {
        while(fread(&trans, sizeof(Transaccion), 1, f) == 1) {
            if(trans.num_caja == num_caja &&
               difftime(trans.fecha, inicio) >= 0 &&
               difftime(fin, trans.fecha) >= 0 &&
               trans.tipo == 'V') {
                total += trans.total;
            }
        }
        fclose(f);
    }
    return total;
}

void conciliacion() {
    if(usuario_actual.permiso != 1) {
        printf("Acceso denegado! Solo para administradores\n");
        return;
    }
    
    printf("\n=== CONCILIACION DE CAJA ===\n");
    
    for(int i = 0; i < num_registros; i++) {
        printf("\nRegistro %d:", i+1);
        printf("\nCaja: %d", registros[i].num_caja);
        printf("\nTrabajador: %s", registros[i].trabajador);
        printf("\nInicio: %s", ctime(&registros[i].inicio));
        printf("Fin: %s", ctime(&registros[i].fin));
        printf("Total ventas: $%.2f", registros[i].total_ventas);
        printf("\nDiferencia: $%.2f", registros[i].diferencia);
        printf("\nEstado: %s\n", registros[i].estado);
    }
}

void reportes_caja() {
    printf("\n=== REPORTES DE CAJA ===\n");
    printf("1. Reporte diario\n");
    printf("2. Reporte por cajero\n");
    printf("3. Reporte por caja\n");
    printf("Seleccione: ");
    
    int opcion;
    scanf("%d", &opcion);
    
    switch(opcion) {
        case 1: generar_reporte_diario(); break;
        // Implementar otros reportes similarmente
        default: printf("Opcion no implementada!\n");
    }
}

void generar_reporte_diario() {
    time_t fecha;
    printf("Ingrese fecha (dd/mm/aaaa): ");
    struct tm tm_fecha = {0};
    scanf("%d/%d/%d", &tm_fecha.tm_mday, &tm_fecha.tm_mon, &tm_fecha.tm_year);
    tm_fecha.tm_mon--;
    tm_fecha.tm_year -= 1900;
    fecha = mktime(&tm_fecha);
    
    printf("\n=== REPORTE DIARIO ===\n");
    printf("Fecha: %s", ctime(&fecha));
    printf("%-10s %-15s %-10s %-12s %-12s\n", 
           "Caja", "Trabajador", "Ventas", "Efectivo", "Diferencia");
    
    for(int i = 0; i < num_registros; i++) {
        if(difftime(registros[i].inicio, fecha) >= 0 &&
           difftime(registros[i].fin, fecha) <= 86400) {
            printf("%-10d %-15s $%-9.2f $%-10.2f $%-10.2f\n",
                   registros[i].num_caja,
                   registros[i].trabajador,
                   registros[i].total_ventas,
                   registros[i].efectivo_final,
                   registros[i].diferencia);
        }
    }
}

void mostrar_menu() {
    int opcion;
    do {
        printf("\n=== CONTROL DE CAJA ===\n");
        printf("1. Movimientos de caja\n");
        printf("2. Conciliacion\n");
        printf("3. Reportes\n");
        printf("4. Salir\n");
        printf("Seleccione: ");
        scanf("%d", &opcion);
        
        switch(opcion) {
            case 1: movimientos_caja(); break;
            case 2: conciliacion(); break;
            case 3: reportes_caja(); break;
            case 4: printf("Saliendo...\n"); break;
            default: printf("Opcion invalida!\n");
        }
    } while(opcion != 4);
}

int main() {
    autenticar_usuario();
    cargar_datos();
    mostrar_menu();
    guardar_datos();
    return 0;
}