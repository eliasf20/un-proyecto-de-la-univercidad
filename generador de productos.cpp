#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO_DAT "productos.dat"
#define MAX_PRODUCTOS 100

struct Producto {
    char codigo[20];
    char nombre[50];
    int cantidadUnd;
    double cantidadGr;
    double precioBs;
};

void generarInventario() {
    struct Producto productos[MAX_PRODUCTOS];
    int contador = 0;

    // Confitería (Unidades)
    char* confiteria[] = {"Chocolate Negro", "Chocolate con Leche", "Gomitas", "Paleta de Caramelo", 
                         "Bombones", "Tableta de Avellanas", "Turrón", "Galletas Rellenas", 
                         "Barra de Cereal", "Chicles", "Mentas", "Oreo", "KitKat", "Snickers",
                         "Twix", "M&M's", "Skittles", "Ferrero Rocher", "Lindt", "Toblerone",
                         "Caramelo Ácido", "Piruletas", "Gominolas", "Regaliz", "Chocolate Blanco"};
    for(int i = 0; i < 25; i++) {
        sprintf(productos[contador].codigo, "<%03d>", contador + 1);
        strcpy(productos[contador].nombre, confiteria[i]);
        productos[contador].cantidadUnd = 50 + rand() % 100;
        productos[contador].cantidadGr = 0;
        productos[contador].precioBs = 5.0 + (rand() % 2000)/100.0;
        contador++;
    }

    // Repostería (Gramos)
    char* reposteria[] = {"Harina de Trigo", "Azúcar Glass", "Mantequilla", "Levadura", 
                         "Chocolate para Fundir", "Colorante Alimenticio", "Esencia de Vainilla", 
                         "Moldes para Cupcakes", "Betún", "Fondant", "Merengue", "Almendras",
                         "Nueces", "Coco Rallado", "Canela en Polvo", "Cacao en Polvo", 
                         "Gelatina sin Sabor", "Claras de Huevo", "Masa Quebrada", "Masa Filo",
                         "Glaseado Real", "Pasta de Azúcar", "Dulce de Leche", "Crema Pastelera",
                         "Buttercream"};
    for(int i = 0; i < 25; i++) {
        sprintf(productos[contador].codigo, "<%03d>", contador + 1);
        strcpy(productos[contador].nombre, reposteria[i]);
        productos[contador].cantidadUnd = 0;
        productos[contador].cantidadGr = 1000 + rand() % 5000;
        productos[contador].precioBs = 8.0 + (rand() % 3000)/100.0;
        contador++;
    }

    // Comida Básica (Mezcla)
    char* comida[] = {"Arroz Blanco", "Lentejas", "Huevos (docena)", "Leche Entera", 
                     "Pechuga de Pollo", "Carne Molida", "Atún en Lata", "Aceite de Oliva",
                     "Sal Yodada", "Pasta Spaghetti", "Queso Parmesano", "Yogur Natural",
                     "Mantequilla de Maní", "Miel de Abeja", "Café Molido", "Avena en Hojuelas",
                     "Sardinas en Lata", "Jamón Serrano", "Tocino Ahumado", "Salmón Fresco",
                     "Filete de Res", "Ternera", "Cerdo", "Pavo", "Cordero"};
    for(int i = 0; i < 25; i++) {
        sprintf(productos[contador].codigo, "<%03d>", contador + 1);
        strcpy(productos[contador].nombre, comida[i]);
        if(i < 5) {
            productos[contador].cantidadUnd = 20 + rand() % 50;
            productos[contador].cantidadGr = 0;
        } else {
            productos[contador].cantidadUnd = 0;
            productos[contador].cantidadGr = 500 + rand() % 2500;
        }
        productos[contador].precioBs = 3.0 + (rand() % 5000)/100.0;
        contador++;
    }

    // Verduras (Mezcla)
    char* verduras[] = {"Zanahoria", "Brócoli", "Espinacas", "Tomate", "Cebolla",
                       "Papa", "Ajo", "Pimiento Rojo", "Calabacín", "Berenjena",
                       "Coliflor", "Lechuga", "Pepino", "Rábano", "Apio",
                       "Remolacha", "Champiñones", "Maíz Dulce", "Aguacate", "Calabaza",
                       "Espárragos", "Alcachofa", "Puerro", "Col Rizada", "Rúcula"};
    for(int i = 0; i < 25; i++) {
        sprintf(productos[contador].codigo, "<%03d>", contador + 1);
        strcpy(productos[contador].nombre, verduras[i]);
        if(i % 2 == 0) {
            productos[contador].cantidadUnd = 30 + rand() % 70;
            productos[contador].cantidadGr = 0;
        } else {
            productos[contador].cantidadUnd = 0;
            productos[contador].cantidadGr = 300 + rand() % 1200;
        }
        productos[contador].precioBs = 1.5 + (rand() % 2000)/100.0;
        contador++;
    }

    // Guardar en archivo
    FILE *archivo = fopen(ARCHIVO_DAT, "wb");
    if(archivo) {
        fwrite(productos, sizeof(struct Producto), MAX_PRODUCTOS, archivo);
        fclose(archivo);
        printf("Inventario generado con 100 productos en %s\n", ARCHIVO_DAT);
    } else {
        printf("Error al crear archivo!\n");
    }
}

int main() {
    generarInventario();
    return 0;
}