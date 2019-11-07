#include <stdio.h>
#include <stdbool.h>
#include "hash.h"
#include "hash_iterador.h"
#include "pruebas.h"
#include <stdlib.h>
#include <string.h>
#define ERROR -1
#define EXITO 0
#define ANSI_COLOR_GREEN   "\x1b[1m\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED	"\x1b[1m\x1b[31m"

static int failure_count = 0; 		// Contador de pruebas fallidas
static int success_count = 0; 		// Contador de pruebas pasadas
extern char* strdup(const char*);


void destruir_string(void* elemento){
  
  if(elemento)
    free(elemento);
  
}

void assert_prueba(char* prueba, bool condicion){

	printf("Test: %s: ", prueba);

	if(condicion){
		success_count++;
		printf(ANSI_COLOR_GREEN "OK. \n" ANSI_COLOR_RESET);
	}
	else{
		failure_count++;
		printf(ANSI_COLOR_RED"FALLO.\n" ANSI_COLOR_RESET);
	}

}

void test_hash_nulos(){

	hash_t* hash = NULL;
	printf("\nTEST SOBRE HASH NULOS: \n\n");

	assert_prueba("Insertar en hash NULL deberia devolver error", hash_insertar(hash, "ABCD123BD", "PRUEBA") == ERROR);
	assert_prueba("Quitar en hash NULL deberia devolver error", hash_quitar(hash, "ABCD123BD"));
	assert_prueba("Obtener en hash NULL deberia devolver error", hash_obtener(hash, "ABCD123BD") == NULL);
	assert_prueba("Ver si contiene en hash NULL deberia devolver false", hash_contiene(hash, "ABCD123BD") == false);
	assert_prueba("Pedir cantidad en hash NULL deberia devolver 0", hash_cantidad(hash) == 0);

}


void test_insercion_borrado_busqueda(){

	printf("\nTEST INSERCION: \n\n");

	hash_t* hash = hash_crear(destruir_string, 5);

	assert_prueba("Inserto un elemento, deberia devolver EXITO", hash_insertar(hash, "ABCD123BD", strdup("PRUEBA 1")) == EXITO);
	assert_prueba("Dicho elemento ahora deberia estar en el hash", hash_contiene(hash, "ABCD123BD"));
	assert_prueba("Borro dicho elemento, deberia devolver EXITO", hash_quitar(hash, "ABCD123BD") == EXITO);
	assert_prueba("El elemento no deberia estar mas en el hash", !hash_contiene(hash, "ABCD123BD"));
	assert_prueba("Busco un elemento que no existe, contiene deberia devolver false", !hash_contiene(hash, "ASDF12345"));

	hash_insertar(hash, "ABCD123BD", strdup("PRUEBA 1"));
	hash_insertar(hash, "ABCD123BD", strdup("PRUEBA 2"));
	assert_prueba("Inserto un elemento con misma clave, deberia reemplazar la descripcion", strcmp((char*)hash_obtener(hash, "ABCD123BD"), "PRUEBA 2") == 0);

	char* patentes[15] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o"};
	char* descripciones[15] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

	int correctamente_insertados = 0;

	for (int i = 0; i < 15; ++i){
		if(hash_insertar(hash, patentes[i], strdup(descripciones[i])) == EXITO)
			correctamente_insertados++;
	}

	assert_prueba("Inserto 15 elementos, no deberia haber problemas", correctamente_insertados == 15);
	
	int coincidencias = 0;
	for(int i = 0; i < 15; i++){
		if(strcmp(hash_obtener(hash, patentes[i]), descripciones[i]) == 0)
			coincidencias++;
	}

	assert_prueba("Todos los elementos tienen su correcta descripcion", coincidencias == 15);
	assert_prueba("Cantidad deberia devolver 16", hash_cantidad(hash) == 16);


	hash_destruir(hash);


}

void test_iterador(){

	printf("\nTEST ITERADOR: \n\n");
	hash_t* hash = hash_crear(destruir_string, 5);
	hash_iterador_t* iter = hash_iterador_crear(hash);

	assert_prueba("En un hash vacio, tiene siguiente deberia devolver false", !hash_iterador_tiene_siguiente(iter));
	hash_iterador_destruir(iter);

	hash_insertar(hash, "A", strdup("PRUEBA 1"));
	
	hash_iterador_t* iter2 = hash_iterador_crear(hash);
	assert_prueba("Inserto un elemento, tiene siguiente ahora deberia devolver true", hash_iterador_tiene_siguiente(iter2));
	assert_prueba("Siguiente me deberia devolver el elemento insertado", strcmp((char*)hash_iterador_siguiente(iter2), "A")	== 0);
	assert_prueba("Siguiente ahora deberia devolver NULL", hash_iterador_siguiente(iter2) == NULL);
	assert_prueba("Ya no deberia haber siguiente", !hash_iterador_tiene_siguiente(iter2));
	hash_quitar(hash, "A");
	hash_iterador_destruir(iter2);
	char* patentes[15] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o"};
	char* descripciones[15] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

	for (int i = 0; i < 15; ++i)
		hash_insertar(hash, patentes[i], strdup(descripciones[i]));

	int cantidad_recorridos = 0;
	hash_iterador_t* iter3 = hash_iterador_crear(hash);

	while(hash_iterador_tiene_siguiente(iter3)){
		cantidad_recorridos++;
		hash_iterador_siguiente(iter3);
		
	}


	assert_prueba("Deberia recorrer todos los elementos despues de rehashear", cantidad_recorridos == 15);
	hash_iterador_destruir(iter3);
	hash_destruir(hash);

}

void print_count(){

	printf("\nOverall:\n");

	printf("Pruebas Corridas: %i\n", failure_count + success_count);
	printf("Pruebas Pasadas:" ANSI_COLOR_GREEN "%i \n" ANSI_COLOR_RESET , success_count);
	printf("Pruebas Fallidas:" ANSI_COLOR_RED "%i\n" ANSI_COLOR_RESET, failure_count);
}