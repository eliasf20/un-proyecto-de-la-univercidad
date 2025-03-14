#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LENGTH 26
#define MAX_USER_LENGTH 50
#define MAX_PASS_LENGTH 50

typedef struct {
    char username[MAX_USER_LENGTH];
    char password[MAX_PASS_LENGTH];
} Admin;

typedef struct {
    char username[MAX_USER_LENGTH];
    char id[MAX_ID_LENGTH];
} Employee;

int validarAdmin(const char *usuario, const char *contrasena) {
    Admin admin;
    FILE *file = fopen("usuarios.dat", "rb");
    if (!file) {
        printf("Error al abrir el archivo de usuarios.\n");
        return 0;
    }
    
    while (fread(&admin, sizeof(Admin), 1, file)) {
        if (strcmp(admin.username, usuario) == 0 && strcmp(admin.password, contrasena) == 0) {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

int validarEmpleado(const char *usuario, const char *id) {
    Employee employee;
    FILE *file = fopen("usuarios.dat", "rb");
    if (!file) {
        printf("Error al abrir el archivo de usuarios.\n");
        return 0;
    }
    
    while (fread(&employee, sizeof(Employee), 1, file)) {
        if (strcmp(employee.username, usuario) == 0 && strcmp(employee.id, id) == 0) {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

void menuAdministrativo() {
    printf("\n>>>>>>>>>>>>>>0000000<<<<<<<<<<<<<<\n");
    printf("MENU DE OPERACIONES ADMINISTRATIVAS\n");
    printf(">>>>>>>>>>>>>>0000000<<<<<<<<<<<<<<\n");
    printf("1) VENTAS\n");
    printf("2) PRODUCTOS\n");
    printf("3) TRABAJADORES\n");
    printf("4) SALIR\n");
    printf("===================================\n");
}

void menuEmpleado() {
    printf("\n>>>>>>>>>>>>>>0000000<<<<<<<<<<<<<<\n");
    printf("MENU DE OPERACIONES EMPLEADOS\n");
    printf(">>>>>>>>>>>>>>0000000<<<<<<<<<<<<<<\n");
    printf("1) FACTURA\n");
    printf("2) CONTROL DE CAJA\n");
    printf("3) REGISTRO DE CLIENTES\n");
    printf("4) SALIR\n");
    printf("===================================\n");
}

void ejecutarPrograma(const char *ruta) {
    int resultado = system(ruta);
    if (resultado == 0) {
        printf("El programa se ejecutó correctamente.\n");
    } else {
        printf("Hubo un error al ejecutar el programa.\n");
    }
}

int main() {
    int tipoUsuario;
    char usuario[MAX_USER_LENGTH], contrasena[MAX_PASS_LENGTH], id[MAX_ID_LENGTH];

    printf("Ingrese tipo de usuario (1 - Empleado, 2 - Administrador): ");
    scanf("%d", &tipoUsuario);

    if (tipoUsuario == 2) {
        printf("===================================\n");
        printf("INGRESE DATOS \nUSUARIO: ");
        scanf("%s", usuario);
        printf("CONTRASEÑA: ");
        scanf("%s", contrasena);
        printf("===================================\n");

        if (validarAdmin(usuario, contrasena)) {
            int opcion;
            do {
                menuAdministrativo();
                printf("Seleccione una opción: ");
                scanf("%d", &opcion);
                
                switch (opcion) {
                    case 1:
                        ejecutarPrograma("\"FUNCION VENTAS V4.exe\"");
                        break;
                    case 2:
                        ejecutarPrograma("\"FUNCION PRODUCTO V6.exe\"");
                        break;
                    case 3:
                        ejecutarPrograma("\"FUNCION TRABAJADORES V3.exe\"");
                        break;
                    case 4:
                        printf("Saliendo del menú administrativo...\n");
                        break;
                    default:
                        printf("Opción no válida.\n");
                        break;
                }
            } while (opcion != 4);
        } else {
            printf("Acceso denegado: usuario o contraseña incorrectos.\n");
        }
    } else if (tipoUsuario == 1) {
        printf("===================================\n");
        printf("INGRESE DATOS \nUSUARIO: ");
        scanf("%s", usuario);
        printf("ID (máximo %d números): ", MAX_ID_LENGTH);
        scanf("%s", id);
        printf("===================================\n");

        if (validarEmpleado(usuario, id)) {
            int opcion;
            do {
                menuEmpleado();
                printf("Seleccione una opción: ");
                scanf("%d", &opcion);

                switch (opcion) {
                    case 1:
                        ejecutarPrograma("\"D:\\elias\\programas\\PROYECTO\\FUNCION FACTURA V2.5.exe\"");
                        break;
                    case 2:
                        ejecutarPrograma("\"D:\\elias\\programas\\PROYECTO\\FUNCION CAJA.exe\"");
                        break;
                    case 3:
                        ejecutarPrograma("\"D:\\elias\\programas\\PROYECTO\\funcion clientes proyectov2 (1).exe\"");
                        break;
                    case 4:
                        printf("Saliendo del menú de empleado...\n");
                        break;
                    default:
                        printf("Opción no válida.\n");
                        break;
                }
            } while (opcion != 4);
        } else {
            printf("Acceso denegado: usuario o ID incorrectos.\n");
        }
    } else {
        printf("Tipo de usuario no válido.\n");
    }

    return 0;
}
