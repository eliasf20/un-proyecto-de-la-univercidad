#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CLIENTES 100
#define MAX_COMPRAS 100

enum TipoCliente {
    NORMAL,
    FRECUENTE,
    REGULAR,
    PREMIUM,
    VIP,
    MINORISTA,
    MAYORISTA
};

typedef struct {
    int id_cliente;
    int id;
    int dia, mes, anio;
    float monto;
    char fecha[11];
    int cancelada;
} Compra;

typedef struct {
    int puntos;
    char fecha_nacimiento[11];
    enum TipoCliente tipo;
    Compra compras[MAX_COMPRAS];
    int id;
    char nombre[50];
    char telefono[15];
    char correo[50];
    char direccion[100];
    int puntos_fidelidad;
    Compra historial[MAX_COMPRAS];
    int num_compras;
} Cliente;

Cliente clientes[MAX_CLIENTES];
int num_clientes = 0;

// Función para validar el formato de la fecha (DD/MM/AAAA)
bool validarFormatoFecha(const char* fecha) {
    int dia, mes, anio;
    if (strlen(fecha) != 10 || fecha[2] != '/' || fecha[5] != '/') {
        return false; // Formato incorrecto
    }
    if (sscanf(fecha, "%d/%d/%d", &dia, &mes, &anio) != 3) {
        return false; // No se pudieron extraer día, mes y año
    }
    // Validar rangos de día, mes y año
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1900 || anio > 2100) {
        return false; // Valores fuera de rango
    }
    return true; // Fecha válida
}

// Función para validar el formato del teléfono
bool validarTelefono(const char* telefono) {
	int i ;
    int longitud = strlen(telefono);
    if (longitud != 10) return false; // El teléfono debe tener 10 dígitos
    for ( i = 0; i < longitud; i++) {
        if (!isdigit(telefono[i])) return false; // Todos los caracteres deben ser dígitos
    }
    return true; // Teléfono válido
}

// Función para validar el formato del correo electrónico
bool validarCorreo(const char* correo) {
    return (strchr(correo, '@') != NULL && strchr(correo, '.') != NULL); // Debe contener '@' y '.'
}

// Función para buscar un cliente por su ID
int buscarCliente(int id) {
	int i ;
    for ( i = 0; i < num_clientes; i++) {
        if (clientes[i].id == id) {
            return i; // Retorna el índice del cliente
        }
    }
    return -1; // Cliente no encontrado
}

// Función para actualizar la segmentación de los clientes
int actualizarSegmentacion() {
	int i ;
	int j;
    int actualizaciones = 0;
    for ( i = 0; i < num_clientes; i++) {
        float total_compras = 0;
        enum TipoCliente tipo_anterior = clientes[i].tipo;

        for ( j = 0; j < clientes[i].num_compras; j++) {
            total_compras += clientes[i].compras[j].monto;
        }

        if (total_compras < 1000) {
            clientes[i].tipo = NORMAL;
        } else if (total_compras < 5000) {
            clientes[i].tipo = FRECUENTE;
        } else {
            clientes[i].tipo = VIP;
        }

        if (clientes[i].tipo != tipo_anterior) {
            actualizaciones++;
        }
    }
    return actualizaciones;
}

// Función para registrar un cliente
int registrarCliente(const char* nombre, const char* telefono, const char* correo, const char* fecha_nacimiento) {
    if (num_clientes >= MAX_CLIENTES) {
        printf("Error: Límite de clientes alcanzado.\n");
        return 0;
    }

    if (!validarTelefono(telefono)) {
        printf("Error: Número de teléfono inválido.\n");
        return 0;
    }

    if (!validarCorreo(correo)) {
        printf("Error: Correo electrónico inválido.\n");
        return 0;
    }

    if (!validarFormatoFecha(fecha_nacimiento)) {
        printf("Error: Formato de fecha inválido. Use DD/MM/AAAA.\n");
        return 0;
    }

    Cliente nuevoCliente;
    nuevoCliente.id = num_clientes + 1;
    strcpy(nuevoCliente.nombre, nombre);
    strcpy(nuevoCliente.telefono, telefono);
    strcpy(nuevoCliente.correo, correo);
    strcpy(nuevoCliente.fecha_nacimiento, fecha_nacimiento);
    nuevoCliente.tipo = MINORISTA;
    nuevoCliente.puntos_fidelidad = 0;
    nuevoCliente.num_compras = 0;

    clientes[num_clientes] = nuevoCliente;
    num_clientes++;

    printf("Cliente registrado con éxito. ID asignado: %d\n", nuevoCliente.id);
    return nuevoCliente.id;
}

// Función para registrar una compra
int registrarCompra(int id_cliente, float monto, const char* fecha) {
    int indice = buscarCliente(id_cliente);
    if (indice == -1) {
        printf("Error: Cliente no encontrado.\n");
        return 0;
    }

    if (!validarFormatoFecha(fecha)) {
        printf("Error: Formato de fecha inválido. Use DD/MM/AAAA.\n");
        return 0;
    }

    if (monto <= 0) {
        printf("Error: El monto de la compra debe ser positivo.\n");
        return 0;
    }

    if (clientes[indice].num_compras >= MAX_COMPRAS) {
        printf("Error: Límite de compras alcanzado.\n");
        return 0;
    }

    Compra nuevaCompra;
    nuevaCompra.id = clientes[indice].num_compras + 1;
    nuevaCompra.id_cliente = id_cliente;
    nuevaCompra.monto = monto;
    strcpy(nuevaCompra.fecha, fecha);

    // Guardar la compra en el arreglo de compras
    clientes[indice].compras[clientes[indice].num_compras] = nuevaCompra;

    // Guardar la compra en el historial
    clientes[indice].historial[clientes[indice].num_compras] = nuevaCompra;

    // Incrementar el número de compras
    clientes[indice].num_compras++;

    // Calcular puntos de fidelidad
    int puntos_ganados = (int)(monto / 10);
    clientes[indice].puntos_fidelidad += puntos_ganados;

    printf("Compra registrada con éxito. ID de compra: %d\n", nuevaCompra.id);
    printf("Puntos de fidelidad ganados: %d\n", puntos_ganados);

    return nuevaCompra.id;
}

// Función para cancelar una compra por su ID
bool cancelarCompraPorId(Cliente* cliente, int id_compra) {
	int i;
    if (cliente == NULL) {
        return false;
    }

    for ( i = 0; i < cliente->num_compras; i++) {
        if (cliente->compras[i].id == id_compra) {
            cliente->compras[i].cancelada = 1;
            return true;
        }
    }
    return false; // Compra no encontrada
}

// Función para generar un reporte de compras de un cliente
int generarReporteCompras(const Cliente* cliente) {
	int i ;
    if (cliente == NULL) {
        printf("Error: Cliente no válido.\n");
        return 0;
    }

    printf("Reporte de Compras del Cliente\n");
    printf("==============================\n");
    printf("ID del Cliente: %d\n", cliente->id);
    printf("Nombre del Cliente: %s\n", cliente->nombre);
    printf("Teléfono: %s\n", cliente->telefono);
    printf("Correo: %s\n", cliente->correo);
    printf("Dirección: %s\n", cliente->direccion);
    printf("Fecha de Nacimiento: %s\n", cliente->fecha_nacimiento);
    printf("Tipo de Cliente: %d\n", cliente->tipo);
    printf("Puntos de Fidelidad: %d\n\n", cliente->puntos_fidelidad);

    printf("Listado de Compras:\n");
    printf("-------------------\n");

    if (cliente->num_compras == 0) {
        printf("Este cliente no tiene compras registradas.\n");
    } else {
        printf("ID\tFecha\t\tMonto\t\tPuntos Ganados\n");
        for ( i = 0; i < cliente->num_compras; i++) {
            const Compra* compra = &(cliente->historial[i]);
            int puntos_ganados = (int)(compra->monto / 100);
            printf("%d\t%s\t%.2f\t\t%d\n", 
                   compra->id, 
                   compra->fecha,
                   compra->monto, 
                   puntos_ganados);
        }
    }

    printf("\nTotal de Compras: %d\n", cliente->num_compras);

    float monto_total = 0;
    for (i = 0; i < cliente->num_compras; i++) {
        monto_total += cliente->historial[i].monto;
    }
    printf("Monto Total de Compras: %.2f\n", monto_total);

    return 1;
}

// Función para actualizar la información de contacto de un cliente
int actualizarContactoCliente(int id_cliente) {
    int indice = buscarCliente(id_cliente);
    if (indice == -1) {
        printf("Cliente no encontrado.\n");
        return 0;
    }

    printf("Actualizar información de contacto para %s:\n", clientes[indice].nombre);

    char nuevoTelefono[20];
    do {
        printf("Nuevo teléfono (dejar en blanco para no cambiar): ");
        scanf(" %[^\n]", nuevoTelefono);
        if (strlen(nuevoTelefono) > 0 && !validarTelefono(nuevoTelefono)) {
            printf("Número de teléfono inválido. Intente de nuevo.\n");
        }
    } while (strlen(nuevoTelefono) > 0 && !validarTelefono(nuevoTelefono));

    if (strlen(nuevoTelefono) > 0) {
        strcpy(clientes[indice].telefono, nuevoTelefono);
    }

    char nuevoCorreo[50];
    do {
        printf("Nuevo correo electrónico (dejar en blanco para no cambiar): ");
        scanf(" %[^\n]", nuevoCorreo);
        if (strlen(nuevoCorreo) > 0 && !validarCorreo(nuevoCorreo)) {
            printf("Correo electrónico inválido. Intente de nuevo.\n");
        }
    } while (strlen(nuevoCorreo) > 0 && !validarCorreo(nuevoCorreo));

    if (strlen(nuevoCorreo) > 0) {
        strcpy(clientes[indice].correo, nuevoCorreo);
    }

    printf("Información de contacto actualizada con éxito.\n");
    return 1;
}

// Función para aplicar un descuento a una compra
float aplicarDescuento(int id_cliente, float monto_compra) {
    int indice = buscarCliente(id_cliente);
    if (indice == -1) {
        printf("Error: Cliente no encontrado.\n");
        return monto_compra;
    }

    float descuento = 0.0;
    switch (clientes[indice].tipo) {
        case PREMIUM:
            descuento = monto_compra * 0.15;
            break;
        case FRECUENTE:
            descuento = monto_compra * 0.10;
            break;
        case MINORISTA:
            descuento = monto_compra * 0.05;
            break;
    }

    float monto_final = monto_compra - descuento;
    return monto_final;
}

// Función para listar todos los clientes
int listarClientes() {
	int i ;
    if (num_clientes == 0) {
        printf("No hay clientes registrados.\n");
        return 0;
    }

    printf("Listado de Clientes:\n");
    printf("-------------------\n");
    for ( i = 0; i < num_clientes; i++) {
        printf("ID: %d\n", clientes[i].id);
        printf("Nombre: %s\n", clientes[i].nombre);
        printf("Teléfono: %s\n", clientes[i].telefono);
        printf("Correo: %s\n", clientes[i].correo);
        printf("Tipo de Cliente: %d\n", clientes[i].tipo);
        printf("Puntos de Fidelidad: %d\n", clientes[i].puntos_fidelidad);
        printf("-------------------\n");
    }

    return 1;
}

// Función para gestionar clientes (menú principal)
int gestionClientes() {
    int opcion;
    do {
        printf("\n--- Sistema de Gestión de Clientes ---\n");
        printf("1. Registrar nuevo cliente\n");
        printf("2. Registrar compra\n");
        printf("3. Cancelar compra\n");
        printf("4. Generar reporte de compras de un cliente\n");
        printf("5. Actualizar información de contacto de un cliente\n");
        printf("6. Aplicar descuento\n");
        printf("7. Listar clientes\n");
        printf("8. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                char nombre[50], telefono[15], correo[50], fecha_nacimiento[11];
                printf("Ingrese el nombre del cliente: ");
                scanf(" %[^\n]", nombre);
                printf("Ingrese el teléfono del cliente: ");
                scanf(" %[^\n]", telefono);
                printf("Ingrese el correo electrónico del cliente: ");
                scanf(" %[^\n]", correo);
                printf("Ingrese la fecha de nacimiento del cliente (DD/MM/AAAA): ");
                scanf(" %[^\n]", fecha_nacimiento);

                if (registrarCliente(nombre, telefono, correo, fecha_nacimiento)) {
                    printf("Cliente registrado con éxito.\n");
                }
                break;
            }
            case 2: {
                int id_cliente;
                float monto;
                char fecha[11];
                printf("Ingrese el ID del cliente: ");
                scanf("%d", &id_cliente);
                printf("Ingrese el monto de la compra: ");
                scanf("%f", &monto);
                printf("Ingrese la fecha de la compra (DD/MM/AAAA): ");
                scanf("%s", fecha);

                if (registrarCompra(id_cliente, monto, fecha)) {
                    printf("Compra registrada con éxito.\n");
                }
                break;
            }
            case 3: {
                int id_cliente, id_compra;
                printf("Ingrese el ID del cliente: ");
                scanf("%d", &id_cliente);
                printf("Ingrese el ID de la compra a cancelar: ");
                scanf("%d", &id_compra);

                int indice_cliente = buscarCliente(id_cliente);
                if (indice_cliente == -1) {
                    printf("Cliente no encontrado.\n");
                    break;
                }

                if (cancelarCompraPorId(&clientes[indice_cliente], id_compra)) {
                    printf("Compra cancelada con éxito.\n");
                } else {
                    printf("No se pudo cancelar la compra.\n");
                }
                break;
            }
            case 4: {
                int id_cliente;
                printf("Ingrese el ID del cliente: ");
                scanf("%d", &id_cliente);

                int indice_cliente = buscarCliente(id_cliente);
                if (indice_cliente == -1) {
                    printf("Cliente no encontrado.\n");
                    break;
                }

                if (generarReporteCompras(&clientes[indice_cliente])) {
                    printf("Reporte generado con éxito.\n");
                } else {
                    printf("No se pudo generar el reporte.\n");
                }
                break;
            }
            case 5: {
                int id_cliente;
                printf("Ingrese el ID del cliente: ");
                scanf("%d", &id_cliente);

                if (actualizarContactoCliente(id_cliente)) {
                    printf("Información de contacto actualizada con éxito.\n");
                } else {
                    printf("No se pudo actualizar la información de contacto.\n");
                }
                break;
            }
            case 6: {
                int id_cliente;
                float monto_compra;
                printf("Ingrese el ID del cliente: ");
                scanf("%d", &id_cliente);
                printf("Ingrese el monto de la compra: ");
                scanf("%f", &monto_compra);

                float monto_final = aplicarDescuento(id_cliente, monto_compra);
                if (monto_final < monto_compra) {
                    printf("Descuento aplicado con éxito. Monto final: %.2f\n", monto_final);
                } else {
                    printf("No se pudo aplicar el descuento o no hubo descuento aplicable.\n");
                }
                break;
            }
            case 7: {
                if (listarClientes()) {
                    printf("Clientes listados con éxito.\n");
                } else {
                    printf("No se pudo listar los clientes.\n");
                }
                break;
            }
            case 8:
                printf("Gracias por usar el Sistema de Gestión de Clientes. ¡Hasta luego!\n");
                break;
            default:
                printf("Opción no válida. Por favor, intente de nuevo.\n");
        }
    } while (opcion != 8);

    return 1;
}

// Función principal
int main() {
    int actualizaciones = actualizarSegmentacion();
    int id_cliente = registrarCliente("Juan Pérez", "1234567890", "juan@example.com", "01/01/1990");
    if (id_cliente > 0) {
        int id_compra = registrarCompra(id_cliente, 1000.0, "15/04/2023");
        if (id_compra > 0) {
            printf("Compra registrada exitosamente.\n");
        }
    }

    return gestionClientes();
}
