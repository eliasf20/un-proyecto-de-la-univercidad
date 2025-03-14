#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Para strcasecmp

#define MAX_PRODUCTOS 100
#define ARCHIVO_DAT "productos.dat"
#define ARCHIVO_TASAS "tasas.dat"

// Estructura para tasas
struct Tasas {
    double tasaUSD;
    double tasaCOP;
    double tasaEURO;
};

// Estructura para productos
struct Producto {
    char codigo[20];
    char nombre[50];
    int cantidadUnd; // Cantidad en unidades
    double cantidadGr; // Cantidad en gramos
    double precioBs;
};

// Prototipos de funciones
void cargarTasas(struct Tasas *tasas);
void guardarTasas(struct Tasas tasas);
void cargarDatos(struct Producto productos[], int *numProductos);
void guardarDatos(struct Producto productos[], int numProductos);
void agregarProducto(struct Producto productos[], int *numProductos);
void mostrarProductosConTasas(struct Producto productos[], int numProductos, struct Tasas tasas);
void mostrarProductos(struct Producto productos[], int numProductos);
void gestionarTasas(struct Tasas *tasas);
void actualizarProducto(struct Producto productos[], int numProductos);
void borrarProducto(struct Producto productos[], int *numProductos);
void menuBusqueda(struct Producto productos[], int numProductos);
void mostrarMenu();

// Funciones de búsqueda
int buscarPorCodigo(struct Producto productos[], int numProductos, const char *codigo);
int buscarPorNombre(struct Producto productos[], int numProductos, const char *nombre);
void buscarPorCantidad(struct Producto productos[], int numProductos, double cantidad, int tipoBusqueda);
void buscarPersonalizada(struct Producto productos[], int numProductos, const char *nombre);

// Implementación de funciones

void cargarTasas(struct Tasas *tasas) {
    FILE *archivo = fopen(ARCHIVO_TASAS, "rb");
    if (archivo != NULL) {
        fread(tasas, sizeof(struct Tasas), 1, archivo);
        fclose(archivo);
    } else {
        tasas->tasaUSD = 0;
        tasas->tasaCOP = 0;
        tasas->tasaEURO = 0;
    }
}

void guardarTasas(struct Tasas tasas) {
    FILE *archivo = fopen(ARCHIVO_TASAS, "wb");
    fwrite(&tasas, sizeof(struct Tasas), 1, archivo);
    fclose(archivo);
}

void cargarDatos(struct Producto productos[], int *numProductos) {
    FILE *archivo = fopen(ARCHIVO_DAT, "rb");
    if (archivo == NULL) {
        printf("No se encontró el archivo, se creará uno nuevo.\n");
        *numProductos = 0; // Inicializar a 0 si no hay archivo
        return;
    }
    
    *numProductos = fread(productos, sizeof(struct Producto), MAX_PRODUCTOS, archivo);
    fclose(archivo);

    // Contar solo productos válidos (con nombre no vacío)
    int count = 0;
    for (int i = 0; i < *numProductos; i++) {
        if (strlen(productos[i].nombre) > 0) {
            count++;
        }
    }
    *numProductos = count; // Actualizar el número de productos válidos
}

void guardarDatos(struct Producto productos[], int numProductos) {
    FILE *archivo = fopen(ARCHIVO_DAT, "wb");
    fwrite(productos, sizeof(struct Producto), numProductos, archivo);
    fclose(archivo);
}

void agregarProducto(struct Producto productos[], int *numProductos) {
    if (*numProductos >= MAX_PRODUCTOS) {
        printf("No se pueden agregar más productos. Límite alcanzado.\n");
        return;
    }

    struct Producto nuevoProducto;
    sprintf(nuevoProducto.codigo, "<%03d>", *numProductos + 1); // Código en formato <001>

    int tipoAlmacenamiento;
    printf("TIPO DE ALMACENAMIENTO\n");
    printf("1) UNIDAD\n");
    printf("2) CANTIDAD (Kg,g;ETC)\n");
    printf(">>> ");
    scanf("%d", &tipoAlmacenamiento);

    printf("NOMBRE DE PRODUCTO\n>>> ");
    scanf(" %[^\n]", nuevoProducto.nombre);
    
    printf("PRECIO Bs\n>>> ");
    scanf("%lf", &nuevoProducto.precioBs);

    if (tipoAlmacenamiento == 1) {
        printf("CANTIDAD (UND)\n>>> ");
        scanf("%d", &nuevoProducto.cantidadUnd);
        nuevoProducto.cantidadGr = 0; // No aplica
    } else {
        printf("CANTIDAD (g)\n>>> ");
        scanf("%lf", &nuevoProducto.cantidadGr);
        nuevoProducto.cantidadUnd = 0; // No aplica
    }

    productos[*numProductos] = nuevoProducto;
    (*numProductos)++;
    guardarDatos(productos, *numProductos);

    printf("========================\n");
    printf("PRODUCTO AGREGADO\n");
    printf("========================\n");
    printf("CODIGO: %s\nNOMBRE: %s\nCANTIDAD(g): %.2f\nCANTIDAD(UND): %d\nPRECIO(BS): %.2f\n",
           nuevoProducto.codigo, nuevoProducto.nombre, nuevoProducto.cantidadGr, nuevoProducto.cantidadUnd, nuevoProducto.precioBs);
    printf("=======================\n");
}

void mostrarProductosConTasas(struct Producto productos[], int numProductos, struct Tasas tasas) {
    printf("==================================================================================================\n");
    printf("| CODIGO   | NOMBRE           | PRECIO(Bs) | PRECIO(USD) | PRECIO(COP) | PRECIO(EURO) |\n");
    printf("==================================================================================================\n");
    
    for (int i = 0; i < numProductos; i++) {
        double precioUSD = tasas.tasaUSD ? productos[i].precioBs / tasas.tasaUSD : 0;
        double precioCOP = tasas.tasaCOP ? productos[i].precioBs / tasas.tasaCOP : 0;
        double precioEURO = tasas.tasaEURO ? productos[i].precioBs / tasas.tasaEURO : 0;

        printf("| %-8s | %-15s | %-10.2f | %-10.2f | %-10.2f | %-10.2f |\n",
               productos[i].codigo, productos[i].nombre, 
               productos[i].precioBs, precioUSD, precioCOP, precioEURO);
    }
    
    printf("==================================================================================================\n");
}

void mostrarProductos(struct Producto productos[], int numProductos) {
    printf("==========================================================================================\n");
    printf("| %-8s | %-25s | %-12s | %-12s | %-10s |\n", 
           "CODIGO", "NOMBRE", "CANTIDAD UND", "CANTIDAD(g)", "PRECIO(Bs)");
    printf("==========================================================================================\n");
    
    for (int i = 0; i < numProductos; i++) {
        if (strlen(productos[i].nombre) > 0) {
            printf("| %-8s | %-25s | %-12d | %-12.2f | %-10.2f |\n",
                   productos[i].codigo, 
                   productos[i].nombre, 
                   productos[i].cantidadUnd, 
                   productos[i].cantidadGr, 
                   productos[i].precioBs);
        }
    }
    
    printf("==========================================================================================\n");
}

void gestionarTasas(struct Tasas *tasas) {
    printf("==========================\n");
    printf("INGRESE TASAS\n");
    printf("--------------------------\n");
    printf("TASA EN USD\n>>> ");
    scanf("%lf", &tasas->tasaUSD);
    printf("TASA EN COP\n>>> ");
    scanf("%lf", &tasas->tasaCOP);
    printf("TASA EN EUROS\n>>> ");
    scanf("%lf", &tasas->tasaEURO);
    guardarTasas(*tasas);
    printf("=========================\n");
    printf("TASAS GUARDADAS\n");
    printf("=========================\n");
}

void actualizarProducto(struct Producto productos[], int numProductos) {
    char codigo[20];
    printf("INGRESE CODIGO DE PRODUCTO PARA ACTUALIZAR\n>>> ");
    scanf("%s", codigo);

    for (int i = 0; i < numProductos; i++) {
        if (strcmp(productos[i].codigo, codigo) == 0) {
            printf("========================\n");
            printf("INF. DE PRODUCTO\n");
            printf("========================\n");
            printf("CODIGO: %s\nNOMBRE: %s\nCANTIDAD(g): %.2f\nCANTIDAD(UND): %d\nPRECIO(BS): %.2f\n",
                   productos[i].codigo, productos[i].nombre, productos[i].cantidadGr, productos[i].cantidadUnd, productos[i].precioBs);
            printf("=======================\n");

            printf("ACT.CANTIDAD (g o UND)\n>>> ");
            if (productos[i].cantidadGr > 0) {
                scanf("%lf", &productos[i].cantidadGr);
            } else {
                scanf("%d", &productos[i].cantidadUnd);
            }

            printf("ACT.PRECIO\n>>> ");
            scanf("%lf", &productos[i].precioBs);

            printf("========================\n");
            printf("PRODUCTO ACTUALIZADO\n");
            printf("========================\n");
            guardarDatos(productos, numProductos);
            return;
        }
    }
    printf("Producto no encontrado.\n");
}

void borrarProducto(struct Producto productos[], int *numProductos) {
    char codigo[20];
    printf("INTRODUCIR CODIGO PARA BORRAR PRODUCTO\n>>> ");
    scanf("%s", codigo);

    for (int i = 0; i < *numProductos; i++) {
        if (strcmp(productos[i].codigo, codigo) == 0) {
            printf("========================\n");
            printf("INF. DE PRODUCTO\n");
            printf("========================\n");
            printf("CODIGO: %s\nNOMBRE: %s\nCANTIDAD(g): %.2f\nCANTIDAD(UND): %d\nPRECIO(BS): %.2f\n",
                   productos[i].codigo, productos[i].nombre, productos[i].cantidadGr, productos[i].cantidadUnd, productos[i].precioBs);
            printf("=======================\n");

            char confirmacion;
            printf("BORRAR PRODUCTO?\n  <Y>        <N>\n");
            printf(">>> ");
            scanf(" %c", &confirmacion);
            if (confirmacion == 'Y' || confirmacion == 'y') {
                for (int j = i; j < *numProductos - 1; j++) {
                    productos[j] = productos[j + 1];
                }
                (*numProductos)--;
                guardarDatos(productos, *numProductos);
                printf("=======================\n");
                printf("PRODUCTO BORRADO\n");
                printf("=======================\n");
            }
            return;
        }
    }
    printf("Producto no encontrado.\n");
}

// Implementación de funciones de búsqueda

int buscarPorCodigo(struct Producto productos[], int numProductos, const char *codigo) {
    for (int i = 0; i < numProductos; i++) {
        if (strcmp(productos[i].codigo, codigo) == 0) {
            return i;
        }
    }
    return -1; // No encontrado
}

int buscarPorNombre(struct Producto productos[], int numProductos, const char *nombre) {
    for (int i = 0; i < numProductos; i++) {
        if (strcasecmp(productos[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1; // No encontrado
}

void buscarPorCantidad(struct Producto productos[], int numProductos, double cantidad, int tipoBusqueda) {
    printf("================================================================================\n");
    printf("| %-8s | %-15s | %-12s | %-12s | %-10s |\n", 
           "CODIGO", "NOMBRE", "CANTIDAD UND", "CANTIDAD(g)", "PRECIO(Bs)");
    printf("================================================================================\n");
    
    for (int i = 0; i < numProductos; i++) {
        if ((tipoBusqueda == 1 && productos[i].cantidadUnd == cantidad) ||
            (tipoBusqueda == 2 && productos[i].cantidadGr == cantidad)) {
            printf("| %-8s | %-15s | %-12d | %-12.2f | %-10.2f |\n",
                   productos[i].codigo, 
                   productos[i].nombre, 
                   productos[i].cantidadUnd, 
                   productos[i].cantidadGr, 
                   productos[i].precioBs);
        }
    }
    printf("================================================================================\n");
}

void buscarPersonalizada(struct Producto productos[], int numProductos, const char *nombre) {
    printf("================================================================================\n");
    printf("| %-8s | %-15s | %-12s | %-12s | %-10s |\n", 
           "CODIGO", "NOMBRE", "CANTIDAD UND", "CANTIDAD(g)", "PRECIO(Bs)");
    printf("================================================================================\n");
    
    for (int i = 0; i < numProductos; i++) {
        if (strstr(productos[i].nombre, nombre) != NULL) {
            printf("| %-8s | %-15s | %-12d | %-12.2f | %-10.2f |\n",
                   productos[i].codigo, 
                   productos[i].nombre, 
                   productos[i].cantidadUnd, 
                   productos[i].cantidadGr, 
                   productos[i].precioBs);
        }
    }
    printf("================================================================================\n");
}

void menuBusqueda(struct Producto productos[], int numProductos) {
    int opcion;
    do {
        printf("\n=============================\n");
        printf("       MENÚ DE BÚSQUEDA      \n");
        printf("=============================\n");
        printf("1. Buscar por código\n");
        printf("2. Buscar por nombre\n");
        printf("3. Buscar por cantidad (unidades)\n");
        printf("4. Buscar por cantidad (gramos)\n");
        printf("5. Búsqueda personalizada (por nombre)\n");
        printf("6. Volver al menú principal\n");
        printf("=============================\n");
        printf("Elige una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                char codigo[20];
                printf("Ingrese el código a buscar: ");
                scanf("%s", codigo);
                int idx = buscarPorCodigo(productos, numProductos, codigo);
                if (idx != -1) {
                    printf("\nProducto encontrado:\n");
                    printf("CODIGO: %s\nNOMBRE: %s\nCANTIDAD(g): %.2f\nCANTIDAD(UND): %d\nPRECIO(BS): %.2f\n",
                           productos[idx].codigo, productos[idx].nombre, productos[idx].cantidadGr, productos[idx].cantidadUnd, productos[idx].precioBs);
                } else {
                    printf("Producto no encontrado.\n");
                }
                break;
            }
            case 2: {
                char nombre[50];
                printf("Ingrese el nombre a buscar: ");
                scanf(" %[^\n]", nombre);
                int idx = buscarPorNombre(productos, numProductos, nombre);
                if (idx != -1) {
                    printf("\nProducto encontrado:\n");
                    printf("CODIGO: %s\nNOMBRE: %s\nCANTIDAD(g): %.2f\nCANTIDAD(UND): %d\nPRECIO(BS): %.2f\n",
                           productos[idx].codigo, productos[idx].nombre, productos[idx].cantidadGr, productos[idx].cantidadUnd, productos[idx].precioBs);
                } else {
                    printf("Producto no encontrado.\n");
                }
                break;
            }
            case 3: {
                int cantidad;
                printf("Ingrese la cantidad en unidades a buscar: ");
                scanf("%d", &cantidad);
                buscarPorCantidad(productos, numProductos, cantidad, 1);
                break;
            }
            case 4: {
                double cantidad;
                printf("Ingrese la cantidad en gramos a buscar: ");
                scanf("%lf", &cantidad);
                buscarPorCantidad(productos, numProductos, cantidad, 2);
                break;
            }
            case 5: {
                char nombre[50];
                printf("Ingrese parte del nombre a buscar: ");
                scanf(" %[^\n]", nombre);
                buscarPersonalizada(productos, numProductos, nombre);
                break;
            }
            case 6: {
                printf("Volviendo al menú principal...\n");
                break;
            }
            default: {
                printf("Opción no válida. Intente de nuevo.\n");
            }
        }
    } while (opcion != 6);
}

void mostrarMenu() {
    printf("==================================================================================================\n");
    printf("                                  --- Menú Principal ---                                          \n");
    printf("==================================================================================================\n");
    printf("1) Registrar Tasa\n");
    printf("2) Actualizar Tasa\n");
    printf("3) Agregar Producto\n");
    printf("4) Actualizar Producto\n");
    printf("5) Borrar Producto\n");
    printf("6) Ver Lista de Productos\n");
    printf("7) Mostrar Lista con Tasas\n");
    printf("8) Buscar Producto\n");
    printf("0) Salir\n");
    printf("==================================================================================================\n");
    printf("Elige una opción: ");
}

int main() {
    struct Producto productos[MAX_PRODUCTOS];
    struct Tasas tasas;
    int numProductos = 0;

    cargarTasas(&tasas);
    cargarDatos(productos, &numProductos);

    int opcion;
    do {
        mostrarMenu();
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
            case 2:
                gestionarTasas(&tasas);
                break;
            case 3:
                agregarProducto(productos, &numProductos);
                break;
            case 4:
                actualizarProducto(productos, numProductos);
                break;
            case 5:
                borrarProducto(productos, &numProductos);
                break;
            case 6:
                mostrarProductos(productos, numProductos);
                break;
            case 7:
                mostrarProductosConTasas(productos, numProductos, tasas);
                break;
            case 8:
                menuBusqueda(productos, numProductos);
                break;
            case 0:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 0);

    return 0;
}