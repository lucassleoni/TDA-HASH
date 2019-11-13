#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include "hash.h"
#include "lista.h"
#include <string.h>
#include "hash_iterador.h"

#define SIN_ELEMENTOS 0
#define ERROR -1
#define EXITO 0
#define FACTOR_REHASH 3

struct hash{
	lista_t** index;
	hash_destruir_dato_t destructor;
	size_t cantidad_elementos;
	size_t capacidad;
	size_t factor_carga;
};

typedef struct elemento{
	char* clave;
	void* elemento;
}elemento_t;


// pre: 
// pos: devuelve TRUE si pudo inicializar todas las listas correctamente, FALSE en caso contrario
bool inicializar_listas(lista_t** index, size_t pos_inicial, size_t capacidad){

	if(!index)
		return false;

	size_t cantidad_inicializadas = 0;
	size_t i = pos_inicial;
	bool inicializa_correctamente = true;

	while(i < capacidad && inicializa_correctamente){

		index[i] = lista_crear();
		if(!index[i])
			inicializa_correctamente = false;
		else{
			i++;
			cantidad_inicializadas++;
		}
	}

	if(!inicializa_correctamente){
		for (size_t i = pos_inicial; i < cantidad_inicializadas + pos_inicial; i++)
			lista_destruir(index[i]);
	}

	return inicializa_correctamente;
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

	hash->capacidad = capacidad;
	hash->cantidad_elementos = SIN_ELEMENTOS;
	hash->destructor = destruir_elemento;
	hash->factor_carga = 0;

	hash->index = malloc(sizeof(void*) * capacidad);
	if(!hash->index){
		free(hash);
		return NULL;
	}

	if(!inicializar_listas(hash->index, 0, hash->capacidad)){
		free(hash->index);
		free(hash);
		return NULL;
	}

	return hash;
}

// pre: clave es distinto de NULL
// pos: devuelve un entero que representa la posicion a insertar en el hash
int determinar_posicion_hash(const char* clave){

	if(!clave)
		return ERROR;

	size_t tamanio_clave = strlen(clave);
	int numero = 257;
	int resultado = 0;

	for(size_t i = 0; i < tamanio_clave; i++)
		resultado += (int)clave[i] * numero;

	return resultado;
}

// pre: clave es distinto de NULL
// pos: devuelve un puntero a un elemento con dicha clave y elementos
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

// pre:
// pos: agranda el tamaño del arreglo de listas. vuelve a insertar todos los elementos. Devuelve 0 si se ejecuto correctamente, -1 caso contrario
int hash_rehashear(hash_t* hash){

	if(!hash)
		return ERROR;
	
	void* aux = realloc(hash->index, (2* hash->capacidad) * sizeof(void*));
	if(!aux)
		return ERROR;

	size_t cantidad_aux = hash->capacidad;
	hash->capacidad = (2*hash->capacidad);
	if(!inicializar_listas(aux, cantidad_aux, hash->capacidad)){
		hash->capacidad = cantidad_aux;
		free(aux);
		return ERROR;
	}

	hash->index = aux;
	elemento_t* elem[hash->cantidad_elementos];
	int tope_elem = 0;

	for(int i = 0; i < cantidad_aux; i++){

		while(!lista_vacia(hash->index[i])){
			lista_iterador_t* iter = lista_iterador_crear(hash->index[i]);
			elemento_t* aux = lista_iterador_siguiente(iter);
			if(aux){	
				elem[tope_elem] = aux;
				tope_elem++;
				lista_borrar_de_posicion(hash->index[i], 0);
				hash->cantidad_elementos--;
			}
			lista_iterador_destruir(iter);
		}
	}

	for(int j = 0; j < tope_elem; j++){

		size_t posicion_hash = (size_t) determinar_posicion_hash(elem[j]->clave) % hash->capacidad;
		lista_insertar(hash->index[posicion_hash], elem[j]);
		hash->cantidad_elementos++;
	}

	return EXITO;
}
/*
 * Inserta un elemento reservando la memoria necesaria para el mismo.
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento){

	if(!hash || !clave)
		return ERROR;

	if(hash_contiene(hash, clave))
		hash_quitar(hash, clave);


	hash->cantidad_elementos++;
	hash->factor_carga = hash->cantidad_elementos/hash->capacidad;

	if(hash->factor_carga >= FACTOR_REHASH){
		if(hash_rehashear(hash) == ERROR)
				return ERROR;
	}

	elemento_t* elemento_a_insertar = crear_elemento((char*)clave, elemento);
	if(!elemento_a_insertar)
		return ERROR;

	size_t posicion_hash = (size_t) determinar_posicion_hash(clave) % hash->capacidad;

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

	size_t posicion_hash = (size_t) determinar_posicion_hash(clave) % hash->capacidad;

	lista_iterador_t* iter = lista_iterador_crear(hash->index[posicion_hash]);
	if(!iter)
		return ERROR;

	elemento_t* elem;
	size_t posicion_a_borrar = 0;
	bool encontro = false;

	while(lista_iterador_tiene_siguiente(iter) && !encontro){
		elem = lista_iterador_siguiente(iter);

		if(strcmp(elem->clave, clave) == 0)
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

	size_t posicion_hash = (size_t) determinar_posicion_hash(clave) % hash->capacidad;
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

	size_t posicion_hash = (size_t) determinar_posicion_hash(clave) % hash->capacidad;
	lista_iterador_t* iter = lista_iterador_crear(hash->index[posicion_hash]);
	if(!iter)
		return false;

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

// pre:
// pos: borra todos los elementos del hash
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


/* 
################################################################################################################
                                               ITERADOR EXTERNO
################################################################################################################
*/

struct hash_iter{
	hash_t* hash;
	lista_iterador_t* lista_iterador;
	size_t lista_actual;
};



/*
 * Crea un iterador de claves para el hash reservando la memoria
 * necesaria para el mismo. El iterador creado es válido desde su
 * creación hasta que se modifique la tabla de hash (insertando o
 * removiendo elementos);
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
hash_iterador_t* hash_iterador_crear(hash_t* hash){

	if(!hash)
		return NULL;

	hash_iterador_t* iter = malloc(sizeof(hash_iterador_t));
	if(!iter)
		return NULL;

	iter->hash = hash;
	iter->lista_actual = 0;
	iter->lista_iterador = lista_iterador_crear(hash->index[iter->lista_actual]);
	if(!iter->lista_iterador){
		free(iter);
		return NULL;
	}

	return iter;
}

/*
 * Devuelve la próxima clave almacenada en el hash y avanza el iterador.
 * Devuelve la clave o NULL si no habia mas.
 */
void* hash_iterador_siguiente(hash_iterador_t* iterador){

	if(!iterador || !iterador->hash)
		return NULL;

	if(!lista_iterador_tiene_siguiente(iterador->lista_iterador) && iterador->lista_actual == iterador->hash->capacidad - 1)
		return NULL;

	if(lista_iterador_tiene_siguiente(iterador->lista_iterador)){
		elemento_t* elem = lista_iterador_siguiente(iterador->lista_iterador);
		return elem->clave;
	}

	if(!lista_iterador_tiene_siguiente(iterador->lista_iterador) && iterador->lista_actual < iterador->hash->capacidad - 1){

		lista_iterador_destruir(iterador->lista_iterador);
		iterador->lista_actual++;
		iterador->lista_iterador = lista_iterador_crear(iterador->hash->index[iterador->lista_actual]);
		if(!iterador->lista_iterador)
			return NULL;

		return hash_iterador_siguiente(iterador);
	}

	return NULL;
}

/*
 * Devuelve true si quedan claves por recorrer o false en caso
 * contrario.
 */
bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador){

	if(!iterador || !iterador->hash)
		return false;

	if(!lista_iterador_tiene_siguiente(iterador->lista_iterador) && iterador->lista_actual == iterador->hash->capacidad - 1)
		return false;

	if(lista_iterador_tiene_siguiente(iterador->lista_iterador))
		return true;
	

	if(!lista_iterador_tiene_siguiente(iterador->lista_iterador) && iterador->lista_actual < iterador->hash->capacidad - 1){
		
		bool tiene_siguiente = false;
		size_t lista_actual = iterador->lista_actual + 1;
		
		while(!tiene_siguiente && lista_actual <= iterador->hash->capacidad - 1){		
			tiene_siguiente = !lista_vacia(iterador->hash->index[lista_actual]);
			lista_actual++;
		}

		return tiene_siguiente;
	}

	return false;
}

/*
 * Destruye el iterador del hash.
 */
void hash_iterador_destruir(hash_iterador_t* iterador){

	if(!iterador)
		return;

	if(iterador->lista_iterador)
		lista_iterador_destruir(iterador->lista_iterador);

	free(iterador);
}


