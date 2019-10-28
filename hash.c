#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include "hash.h"
#include "lista.h"
#include <string.h>

#define SIN_ELEMENTOS 0
#define ERROR -1
#define EXITO 0

struct hash{
	lista_t** index;
	hash_destruir_dato_t destructor;
	size_t cantidad_elementos;	
	size_t capacidad;
};

typedef struct elemento{
	char* clave;
	void* elemento;
}elemento_t;


void inicializar_listas(hash_t* hash){

	if(!hash)
		return;

	for (int i = 0; i < hash->capacidad; ++i)
		hash->index[i] = lista_crear();
}
/*
 * Crea el hash reservando la memoria necesaria para el.
 * Destruir_elemento es un destructor que se utilizará para liberar
 * los elementos que se eliminen del hash.
 * Capacidad indica la capacidad minima inicial con la que se crea el hash.
 * Devuelve un puntero al hash creado o NULL en caso de no poder crearlo.
 */
hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad){

	hash_t* hash = malloc(sizeof(hash_t));
	if(!hash)
		return NULL;

	hash->cantidad_elementos = SIN_ELEMENTOS;
	hash->index = malloc(sizeof(lista_t) * capacidad);
	if(!hash->index){
		free(hash);
		return NULL;
	}
	hash->capacidad = capacidad;
	inicializar_listas(hash);
	hash->destructor = destruir_elemento;
	return hash;
}

int determinar_posicion_hash(const char* clave){

	if(!clave)
		return ERROR;

	int tamanio_clave = strlen(clave);
	int numero = 256;
	int resultado = 0;

	for(int i = 0; i < tamanio_clave; i++)
		resultado += clave[i] * numero;

	return resultado;
}


elemento_t* crear_elemento(const char* clave, void* elemento){

	if(!clave)
		return NULL;

	elemento_t* elem = malloc(sizeof(elemento_t));
	if(!elemento)
		return NULL;

	elem->clave = strdup(clave);
	elem->elemento = elemento;

	return elem;
}
/*
 * Inserta un elemento reservando la memoria necesaria para el mismo.
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento){

	if(!hash || !clave)
		return ERROR;

	if(hash_contiene(hash, clave)){
		return ERROR;
		hash_quitar(hash, clave);
	}
		

	
	elemento_t* elemento_a_insertar = crear_elemento(clave, elemento);
	if(!elemento_a_insertar)
		return ERROR;

	int pos_hash = determinar_posicion_hash(clave) % hash->capacidad;
	int retorno = lista_insertar(hash->index[pos_hash], elemento_a_insertar);

	

	return retorno;
}

/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave){

}

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe.
 */
void* hash_obtener(hash_t* hash, const char* clave){

	if(!hash || !clave)
		return NULL;

	int pos = determinar_posicion_hash(clave) % hash->capacidad;
	
	lista_iterador_t* iter = lista_iterador_crear(hash->index[pos]);
	if(!iter)
		return NULL;

	bool encontro = false;
	elemento_t* elem; 
	while(lista_iterador_tiene_siguiente(iter) && !encontro){

		elem = lista_iterador_siguiente(iter);
		if(strcmp(elem->clave, clave) == 0)
			encontro = true;
	}

	if(encontro)
		return elem->elemento;
	else
		return NULL;
}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario.
 */
bool hash_contiene(hash_t* hash, const char* clave){

	if(!hash || !clave)
		return false;

	int pos = determinar_posicion_hash(clave) % hash->capacidad;
	
	lista_iterador_t* iter = lista_iterador_crear(hash->index[pos]);
	if(!iter)
		return false;
	bool encontro = false;

	while(lista_iterador_tiene_siguiente(iter) && !encontro){
		elemento_t* elem = lista_iterador_siguiente(iter);
		
		if(strcmp(elem->clave, clave) == 0)
			encontro = true;
	}

	return encontro;
}

/*
 * Devuelve la cantidad de elementos almacenados en el hash.
 */
size_t hash_cantidad(hash_t* hash){

	if(!hash)
		return 0;

	return hash->cantidad_elementos;
}

/*
 * Destruye el hash liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * hash.
 */
void hash_destruir(hash_t* hash);
