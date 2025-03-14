#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_PRODUCTOS 100
#define MAX_VENTAS 1000
#define MAX_TRABAJADORES 100
#define ARCHIVO_VENTAS "ventas.dat"
#define ARCHIVO_PRODUCTOS "productos.dat"
#define ARCHIVO_TRABAJADORES "trabajadores.dat"
#define ARCHIVO_REPORTE "REPORTE_VENTA.dat"

typedef struct {
    char codigo[20];
    char nombre[50];
    int cantidadUnd;
    double cantidadGr;
    double precioBs;
} Producto;

typedef struct {
    int id;
    char codigo_producto[20];
    int cantidad;
    double cantidad_gr;
    time_t fecha;
    char tipo;
    char metodo_pago[20];
    double total;
    char ci_trabajador[20];
    int num_caja;
} Transaccion;

typedef struct {
    char nombre[50];
    char apellido[50];
    char cedula[9];
    char telefono[15];
    char direccion[100];
    struct {
        char dias[20];
        char horas[11];
    } horario;
    int numero_empleado;
    int rol;
    float salario;
    int horas_trabajadas;
    float bonificaciones;
    float deducciones;
    float evaluacion_desempeno;
} Empleado;

Producto productos[MAX_PRODUCTOS];
Transaccion ventas[MAX_VENTAS];
Empleado trabajadores[MAX_TRABAJADORES];
int num_productos = 0;
int num_ventas = 0;
int num_trabajadores = 0;

// -------------------- FUNCIONES BASE --------------------
void cargar_datos() {
    FILE *file = fopen(ARCHIVO_PRODUCTOS, "rb");
    if (file) {
        num_productos = fread(productos, sizeof(Producto), MAX_PRODUCTOS, file);
        fclose(file);
    }

    file = fopen(ARCHIVO_VENTAS, "rb");
    if (file) {
        while (num_ventas < MAX_VENTAS && 
               fread(&ventas[num_ventas], sizeof(Transaccion), 1, file) == 1) {
            num_ventas++;
        }
        fclose(file);
    }
}

void guardar_datos() {
    FILE *file = fopen(ARCHIVO_PRODUCTOS, "wb");
    if (file) {
        fwrite(productos, sizeof(Producto), num_productos, file);
        fclose(file);
    }

    file = fopen(ARCHIVO_VENTAS, "wb");
    if (file) {
        fwrite(ventas, sizeof(Transaccion), num_ventas, file);
        fclose(file);
    }
}

// -------------------- AUTENTICACIÓN --------------------
void cargar_trabajadores() {
    FILE *file = fopen(ARCHIVO_TRABAJADORES, "rb");
    if (file) {
        num_trabajadores = fread(trabajadores, sizeof(Empleado), MAX_TRABAJADORES, file);
        fclose(file);
    }
}

bool validar_trabajador(const char *ci) {
    for (int i = 0; i < num_trabajadores; i++) {
        if (strcmp(trabajadores[i].cedula, ci) == 0) {
            return true;
        }
    }
    return false;
}

bool autenticar_trabajador() {
    char ci[20];
    int intentos = 0;

    cargar_trabajadores();

    do {
        printf("\n=== AUTENTICACIÓN ===");
        printf("\nCI del trabajador: ");
        scanf("%19s", ci);

        if (validar_trabajador(ci)) {
            printf("\n¡Bienvenido!\n");
            return true;
        }
        printf("\nDatos incorrectos!\n");
        intentos++;
    } while (intentos < 3);

    printf("Demasiados intentos fallidos!\n");
    return false;
}

// -------------------- MÓDULO DEVOLUCIONES --------------------
void procesar_devolucion() {
    if (!autenticar_trabajador()) return;

    Transaccion devolucion;
    devolucion.id = num_ventas + 1;
    devolucion.fecha = time(NULL);
    devolucion.tipo = 'D';

    printf("\n=== PROCESAR DEVOLUCIÓN ===");
    printf("\nCI del trabajador: ");
    scanf("%19s", devolucion.ci_trabajador);
    printf("Número de caja: ");
    scanf("%d", &devolucion.num_caja);

    printf("Código del producto: ");
    scanf("%19s", devolucion.codigo_producto);

    int idx = -1;
    for (int i = 0; i < num_productos; i++) {
        if (strcmp(productos[i].codigo, devolucion.codigo_producto) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Producto no encontrado!\n");
        return;
    }

    printf("Cantidad a devolver (unidades): ");
    scanf("%d", &devolucion.cantidad);
    devolucion.cantidad_gr = 0;

    productos[idx].cantidadUnd += devolucion.cantidad;
    devolucion.total = productos[idx].precioBs * devolucion.cantidad * -1;

    printf("\nMétodo de pago original:\n1. Efectivo\n2. Tarjeta\n3. Transferencia\nOpción: ");
    int opcion_pago;
    scanf("%d", &opcion_pago);

    switch(opcion_pago) {
        case 1: strcpy(devolucion.metodo_pago, "Efectivo"); break;
        case 2: strcpy(devolucion.metodo_pago, "Tarjeta"); break;
        case 3: strcpy(devolucion.metodo_pago, "Transferencia"); break;
        default: strcpy(devolucion.metodo_pago, "Desconocido");
    }

    ventas[num_ventas++] = devolucion;
    guardar_datos();
    printf("Devolución registrada!\n");
}

// -------------------- GENERACIÓN DE REPORTES --------------------
void generar_reporte_ventas() {
    FILE *reporte = fopen(ARCHIVO_REPORTE, "wb");
    if (!reporte) {
        printf("Error al crear reporte!\n");
        return;
    }

    printf("\n=== REPORTE DE VENTAS ===");
    printf("\nPeriodo (días): ");
    int dias;
    scanf("%d", &dias);

    time_t ahora = time(NULL);
    double total_general = 0;
    int items_general = 0;

    printf("\n%-5s %-15s %-10s %-8s %-12s %-10s\n",
           "ID", "Trabajador", "Caja", "Método", "Total", "Fecha");

    for (int i = 0; i < num_ventas; i++) {
        if (difftime(ahora, ventas[i].fecha) <= (dias * 86400)) {
            char nombre_trabajador[50] = "Desconocido";
            for (int j = 0; j < num_trabajadores; j++) {
                if (strcmp(trabajadores[j].cedula, ventas[i].ci_trabajador) == 0) {
                    snprintf(nombre_trabajador, sizeof(nombre_trabajador), "%s %s", 
                            trabajadores[j].nombre, trabajadores[j].apellido);
                    break;
                }
            }

            fprintf(reporte, "%s|%d|%.2f|%d\n",
                    nombre_trabajador,
                    ventas[i].cantidad + (ventas[i].cantidad_gr > 0 ? 1 : 0),
                    ventas[i].total,
                    ventas[i].num_caja);

            printf("%-5d %-15s %-10d %-8s $%-9.2f %s",
                   ventas[i].id,
                   nombre_trabajador,
                   ventas[i].num_caja,
                   ventas[i].metodo_pago,
                   ventas[i].total,
                   ctime(&ventas[i].fecha));

            total_general += ventas[i].total;
            items_general += ventas[i].cantidad + (ventas[i].cantidad_gr > 0 ? 1 : 0);
        }
    }

    fprintf(reporte, "TOTAL|%d|%.2f|%d\n", items_general, total_general, 0);
    fclose(reporte);

    printf("\nReporte generado en %s\n", ARCHIVO_REPORTE);
}

// -------------------- SISTEMA EXTERNO DE FACTURACIÓN --------------------
void ejecutar_facturacion() {
    if (!autenticar_trabajador()) return;

    printf("\n=== SISTEMA DE FACTURACIÓN ===");
    int resultado = system("\"FUNCION FACTURA V2.5.EXE\"");

    if(resultado != 0) {
        printf("\nError: Sistema de facturación no disponible\n");
        printf("Verifique que el archivo EXE está en el directorio\n");
        return;
    }

    cargar_datos(); // Recargar datos actualizados
    printf("\nFactura procesada exitosamente!\n");
}

// -------------------- MENÚ PRINCIPAL --------------------
int main() {
    cargar_datos();
    cargar_trabajadores();

    int opcion;
    do {
        printf("\n=== SISTEMA INTEGRADO DE VENTAS ===");
        printf("\n1. Sistema de Facturación");
        printf("\n2. Procesar Devolución");
        printf("\n3. Generar Reporte");
        printf("\n4. Salir");
        printf("\nSeleccione: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: ejecutar_facturacion(); break;
            case 2: procesar_devolucion(); break;
            case 3: generar_reporte_ventas(); break;
            case 4: printf("Saliendo...\n"); break;
            default: printf("Opción inválida!\n");
        }
    } while (opcion != 4);

    guardar_datos();
    return 0;
}