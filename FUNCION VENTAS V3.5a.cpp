#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_PRODUCTOS 100
#define MAX_VENTAS 1000
#define ARCHIVO_VENTAS "ventas.dat"
#define ARCHIVO_PRODUCTOS "productos.dat"

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
    time_t fecha;
    char tipo; // 'V' = Venta, 'D' = Devolución
    char metodo_pago[20];
    double total;
} Transaccion;

Producto productos[MAX_PRODUCTOS];
Transaccion ventas[MAX_VENTAS];
int num_productos = 0;
int num_ventas = 0;

// -------------------- FUNCIONES BASE --------------------
void cargar_datos() {
    FILE *file = fopen(ARCHIVO_PRODUCTOS, "rb");
    if(file) {
        num_productos = fread(productos, sizeof(Producto), MAX_PRODUCTOS, file);
        fclose(file);
    }
    
    file = fopen(ARCHIVO_VENTAS, "rb");
    if(file) {
        num_ventas = fread(ventas, sizeof(Transaccion), MAX_VENTAS, file);
        fclose(file);
    }
}

void guardar_datos() {
    FILE *file = fopen(ARCHIVO_PRODUCTOS, "wb");
    if(file) {
        fwrite(productos, sizeof(Producto), num_productos, file);
        fclose(file);
    }
    
    file = fopen(ARCHIVO_VENTAS, "wb");
    if(file) {
        fwrite(ventas, sizeof(Transaccion), num_ventas, file);
        fclose(file);
    }
}

// -------------------- MÓDULO DEVOLUCIONES --------------------
void procesar_devolucion() {
    Transaccion devolucion;
    devolucion.id = num_ventas + 1;
    devolucion.fecha = time(NULL);
    devolucion.tipo = 'D';
    
    printf("\n=== PROCESAR DEVOLUCIÓN ===\n");
    printf("Codigo producto: ");
    scanf("%s", devolucion.codigo_producto);
    
    int encontrado = -1;
    for(int i = 0; i < num_productos; i++) {
        if(strcmp(productos[i].codigo, devolucion.codigo_producto) == 0) {
            encontrado = i;
            break;
        }
    }
    
    if(encontrado == -1) {
        printf("Producto no existe!\n");
        return;
    }
    
    printf("Cantidad a devolver: ");
    scanf("%d", &devolucion.cantidad);
    
    // Actualizar solo cantidadUnd (asume devolución por unidades)
    productos[encontrado].cantidadUnd += devolucion.cantidad;
    devolucion.total = productos[encontrado].precioBs * devolucion.cantidad * -1;
    
    ventas[num_ventas++] = devolucion;
    guardar_datos();
    printf("Devolucion registrada!\n");
}

// -------------------- REPORTES --------------------
void generar_reporte_ventas() {
    printf("\n=== REPORTE DE VENTAS ===\n");
    printf("Periodo (dias): ");
    int dias;
    scanf("%d", &dias);
    
    time_t ahora = time(NULL);
    printf("\nTransacciones ultimos %d dias:\n", dias);
    printf("%-5s %-10s %-8s %-8s %-10s\n", "ID", "Tipo", "Cantidad", "Metodo", "Total");
    
    for(int i = 0; i < num_ventas; i++) {
        if(difftime(ahora, ventas[i].fecha) <= (dias * 86400)) {
            printf("%-5d %-10c %-8d %-8s $%-9.2f\n",
                   ventas[i].id,
                   ventas[i].tipo,
                   ventas[i].cantidad,
                   ventas[i].metodo_pago,
                   ventas[i].total);
        }
    }
}

// -------------------- MENÚ PRINCIPAL --------------------
int main() {
    cargar_datos();
    
    int opcion;
    do {
        printf("\n=== SISTEMA DE VENTAS ===");
        printf("\n1. Generar Factura (Ejecutar sistema externo)");
        printf("\n2. Procesar devolucion");
        printf("\n3. Reporte de ventas");
        printf("\n4. Salir");
        printf("\nSeleccione: ");
        scanf("%d", &opcion);
        
        switch(opcion) {
            case 1: {
                int resultado = system("\"FUNCION FACTURA V2.5.EXE\"");
                
                if(resultado != 0) {
                    printf("\nError: Sistema de facturacion no disponible\n");
                    printf("Verifique que el archivo 'FUNCION FACTURA V2.EXE'\n");
                    printf("esta en el mismo directorio\n");
                }
                break;
            }
            case 2: 
                procesar_devolucion();
                break;
            case 3: 
                generar_reporte_ventas();
                break;
            case 4: 
                printf("Saliendo...\n"); 
                break;
            default: 
                printf("Opcion invalida!\n");
        }
    } while(opcion != 4);
    
    guardar_datos();
    return 0;
}