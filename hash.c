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

	hash->capacidad = capacidad;
	hash->cantidad_elementos = SIN_ELEMENTOS;
	hash->destructor = destruir_elemento;

	hash->index = malloc(sizeof(lista_t) * capacidad);
	if(!hash->index){
		free(hash);
		return NULL;
	}

	for (int i = 0; i < hash->capacidad; ++i)
		hash->index[i] = lista_crear();
	
	return hash;
}


int determinar_posicion_hash(const char* clave){

	if(!clave)
		return ERROR;

	int tamanio_clave = strlen(clave);
	int numero = 257;
	int resultado = 0;

	for(int i = 0; i < tamanio_clave; i++)
		resultado += clave[i] * numero;

	return resultado;
}

elemento_t* crear_elemento(char* clave, void* elemento){

	if(!clave)
		return NULL;

	elemento_t* elem = malloc(sizeof(elemento_t));
	if(!elem)
		return NULL;

	elem->clave = clave;
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

	elemento_t* elemento_a_insertar = crear_elemento((char*)clave, elemento);
	if(!elemento_a_insertar)
		return ERROR;

	int posicion_hash = determinar_posicion_hash(clave) % hash->capacidad;
	hash->cantidad_elementos++;

	return lista_insertar(hash->index[posicion_hash], elemento_a_insertar);
}

/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave){

	if(!hash || !clave)
		return ERROR;

	int posicion_hash = determinar_posicion_hash(clave) % hash->capacidad;

	lista_iterador_t* iter = lista_iterador_crear(hash->index[posicion_hash]);
	if(!iter)
		return ERROR;

	elemento_t* elem;
	int posicion_a_borrar = 0;
	bool encontro = false;

	while(lista_iterador_tiene_siguiente(iter) && !encontro){
		elem = lista_iterador_siguiente(iter);

		if(strcmp(elem->clave, clave ) == 0)
			encontro = true;
		else
			posicion_a_borrar++;
	}

	lista_iterador_destruir(iter);

	if(encontro){
		hash->destructor(elem->elemento);
		free(elem);
		hash->cantidad_elementos--;
		return lista_borrar_de_posicion(hash->index[posicion_hash], posicion_a_borrar);
	}
	else
		return ERROR;
}

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe.
 */
void* hash_obtener(hash_t* hash, const char* clave){

	if(!hash || !clave)
		return NULL;

	int posicion_hash = determinar_posicion_hash(clave) % hash->capacidad;
	lista_iterador_t* iter = lista_iterador_crear(hash->index[posicion_hash]);
	if(!iter)
		return NULL;

	bool encontro = false;
	elemento_t* elem;

	while(lista_iterador_tiene_siguiente(iter) && !encontro){

		elem = lista_iterador_siguiente(iter);
		if(strcmp(clave, elem->clave) == 0)
			encontro = true;
	}

	lista_iterador_destruir(iter);

	if(encontro)
		return elem;
	else
		return NULL;
}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario.
 */
bool hash_contiene(hash_t* hash, const char* clave){

	if(!hash || !clave)
		return ERROR;

	int posicion_hash = determinar_posicion_hash(clave) % hash->capacidad;
	lista_iterador_t* iter = lista_iterador_crear(hash->index[posicion_hash]);
	if(!iter)
		return NULL;

	bool encontro = false;
	elemento_t* elem;

	while(lista_iterador_tiene_siguiente(iter) && !encontro){

		elem = lista_iterador_siguiente(iter);
		if(strcmp(clave, elem->clave) == 0)
			encontro = true;
	}

	lista_iterador_destruir(iter);
	
	return encontro;
}

/*
 * Devuelve la cantidad de elementos almacenados en el hash.
 */
size_t hash_cantidad(hash_t* hash){

	if(!hash)
		return SIN_ELEMENTOS;

	return hash->cantidad_elementos;
}

void borrar_todos_los_elementos(hash_t* hash){

	if(!hash)
		return;

	elemento_t* elem;
	for(int i = 0; i < hash->capacidad; i++){

		while(!lista_vacia(hash->index[i])){

			lista_iterador_t* iter = lista_iterador_crear(hash->index[i]);
			elem = lista_iterador_siguiente(iter);

			if(elem){
				hash->destructor(elem->elemento);
				free(elem);
				hash->cantidad_elementos--;
				lista_borrar_de_posicion(hash->index[i], 0);
			}

			lista_iterador_destruir(iter);
		}
	}

}

/*
 * Destruye el hash liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * hash.
 */
void hash_destruir(hash_t* hash){

	if(!hash)
		return;

	if(hash_cantidad(hash) != 0)
		borrar_todos_los_elementos(hash);

	for(int i = 0; i < hash->capacidad; i++)
		lista_destruir(hash->index[i]);

	free(hash->index);
	free(hash);
}