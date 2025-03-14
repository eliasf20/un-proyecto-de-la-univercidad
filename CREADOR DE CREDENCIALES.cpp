#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USUARIOS 100
#define NOMBRE_ARCHIVO "usuarios.dat"

typedef struct {
    char nombre[50];
    char contrasena[50];
    int tipoUsuario; // 1 para empleado, 2 para administrador
} Usuario;

void agregarUsuario() {
    Usuario nuevoUsuario;
    FILE *archivo;

    printf("Ingrese el nombre de usuario: ");
    scanf("%s", nuevoUsuario.nombre);
    printf("Ingrese la contraseña: ");
    scanf("%s", nuevoUsuario.contrasena);
    
    printf("Ingrese tipo de usuario (1 - Empleado, 2 - Administrador): ");
    scanf("%d", &nuevoUsuario.tipoUsuario);

    archivo = fopen(NOMBRE_ARCHIVO, "ab"); // Abrir en modo append
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    fwrite(&nuevoUsuario, sizeof(Usuario), 1, archivo);
    fclose(archivo);

    printf("Usuario agregado exitosamente.\n");
}

void mostrarUsuarios() {
    Usuario usuario;
    FILE *archivo;

    archivo = fopen(NOMBRE_ARCHIVO, "rb"); // Abrir en modo lectura
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    printf("\nLista de Usuarios:\n");
    while (fread(&usuario, sizeof(Usuario), 1, archivo)) {
        printf("Nombre: %s, Tipo: %d\n", usuario.nombre, usuario.tipoUsuario);
    }

    fclose(archivo);
}

int main() {
    int opcion;

    do {
        printf("\n=============================\n");
        printf("  MENU DE USUARIOS\n");
        printf("=============================\n");
        printf("1) Agregar Usuario\n");
        printf("2) Mostrar Usuarios\n");
        printf("3) Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                agregarUsuario();
                break;
            case 2:
                mostrarUsuarios();
                break;
            case 3:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 3);

    return 0;
}
