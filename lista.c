#include "lista.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define SIN_ELEMENTOS 0
#define ERROR -1
#define EXITO 0

typedef struct nodo {
	void* elemento;
	struct nodo* siguiente;
} nodo_t;


struct lista{
    nodo_t* nodo_inicio;
	nodo_t* nodo_fin;
	size_t tamanio;
};

struct lista_iterador{
	lista_t* lista;
	nodo_t* corriente;
};

/*
 * Crea la lista reservando la memoria necesaria.
 * Devuelve un puntero a la lista creada o NULL en caso de error.
 */
lista_t* lista_crear(){

	lista_t* lista = malloc(sizeof(lista_t));
	
	if(!lista)
		return NULL;

	lista->nodo_inicio = NULL;
	lista->nodo_fin = NULL;
	lista->tamanio = SIN_ELEMENTOS;

	return lista;

}
// pre:
// pos:  crea un nuevo nodo y pone el elemento en el. Devuelve NULL si hubo error.
nodo_t* crear_nuevo_nodo(void* elemento, void* nodo_siguiente){

	nodo_t* nodo = malloc(sizeof(nodo_t));
	
	if(!nodo)
		return NULL;

	nodo->elemento = elemento;
	nodo->siguiente = nodo_siguiente;

	return nodo;
}

// pre: posicion es mayor a 0
// pos: devuelve un puntero al nodo posicion
nodo_t* obtener_puntero_a_nodo(lista_t* lista, size_t posicion){

	if(!lista || lista_vacia(lista))
		return NULL;

	nodo_t* aux = lista->nodo_inicio;
	int i = 0;

	while(i < posicion){
		
		if(aux->siguiente != NULL)
			aux = aux->siguiente;
		i++;
	}

	return aux;
}
/*
 * Inserta un elemento al final de la lista.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 */
int lista_insertar(lista_t* lista, void* elemento){

	if(!lista)
		return ERROR;

	nodo_t* nodo = crear_nuevo_nodo(elemento, NULL);
	if(!nodo)
		return ERROR;

	if(lista_vacia(lista))
		lista->nodo_inicio = nodo;
	else
		lista->nodo_fin->siguiente = nodo;

	lista->nodo_fin = nodo;
	lista->tamanio++;

	return EXITO;
}

// pre: 
// pos: reasigna los punteros limites de la lista dependiendo del tamanio de la misma. Devuelve TRUE si los modifico, FALSE en caso contrario.
bool reasignar_punteros_limites(lista_t* lista){

	if(lista->tamanio == 1){
		lista->nodo_fin = lista->nodo_inicio;
		return true;
	}

	if(lista->tamanio == SIN_ELEMENTOS){
		lista->nodo_inicio = lista->nodo_fin = NULL;
		return true;
	}

	return false;
}

// pre:
// pos: inserta el elemento en la primera posicion. Devuelve -1 si hubo error, 0 en caso contrario.
int lista_insertar_primero(lista_t* lista, void* elemento){

	if(!lista)
		return ERROR;

	nodo_t* aux = crear_nuevo_nodo(elemento, lista->nodo_inicio);
	if(!aux)
		return ERROR;

	lista->nodo_inicio = aux;
	lista->tamanio++;

	return EXITO;
}
/*
 * Inserta un elemento en la posicion indicada, donde 0 es insertar
 * como primer elemento y 1 es insertar luego del primer elemento.  
 * En caso de no existir la posicion indicada, lo inserta al final.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 */
int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){

	if(!lista)
		return ERROR;

	if((posicion >= (lista->tamanio)))
		return lista_insertar(lista, elemento);

	if(posicion == 0)
		return lista_insertar_primero(lista, elemento);

	nodo_t* nodo_anterior = obtener_puntero_a_nodo(lista, posicion - 1);
	if(!nodo_anterior)
		return ERROR;

	nodo_t* nuevo_nodo = crear_nuevo_nodo(elemento, nodo_anterior->siguiente);
	if(!nuevo_nodo)
		return ERROR;

	nodo_anterior->siguiente = nuevo_nodo;
	
	lista->tamanio++;
	return EXITO;
}

/*
 * Quita de la lista el elemento que se encuentra en la ultima posición.
 * Devuelve 0 si pudo eliminar o -1 si no pudo.
 */
int lista_borrar(lista_t* lista){

	if(!lista || lista_vacia(lista))
		return ERROR;

	free(lista->nodo_fin);
	lista->tamanio--;
	
	if(reasignar_punteros_limites(lista))
		return EXITO;

	nodo_t* ultimo_nodo = obtener_puntero_a_nodo(lista,lista->tamanio - 1);
	if(!ultimo_nodo)
		return ERROR;

	lista->nodo_fin = ultimo_nodo;
	lista->nodo_fin->siguiente = NULL;
	

	return EXITO;
}

// pre: 
// pos: borra el primer elemento de la lista. Devuelve -1 si hay error, 0 en caso contrario.
int lista_borrar_primero(lista_t* lista){

	if(!lista)
		return ERROR;

	nodo_t* aux = lista->nodo_inicio;		
	lista->nodo_inicio = aux->siguiente;
	
	free(aux);
	lista->tamanio--;
	
	return EXITO;
}

/*
 * Quita de la lista el elemento que se encuentra en la posición
 * indicada, donde 0 es el primer elemento.  
 * En caso de no existir esa posición se intentará borrar el último
 * elemento.  
 * Devuelve 0 si pudo eliminar o -1 si no pudo.
 */
int lista_borrar_de_posicion(lista_t* lista, size_t posicion){

	if(!lista)
		return ERROR;

	if((posicion >= (lista->tamanio)))
		return lista_borrar(lista);

	if(posicion == 0)
		return lista_borrar_primero(lista);
	
	nodo_t* nodo_anterior = obtener_puntero_a_nodo(lista, posicion - 1);
	if(!nodo_anterior)
		return ERROR;

	nodo_t* nodo_a_borrar = nodo_anterior->siguiente;
	nodo_anterior->siguiente = nodo_a_borrar->siguiente;
	
	lista->tamanio--;
	free(nodo_a_borrar);
	reasignar_punteros_limites(lista);

	return EXITO;
}

/*
 * Devuelve el elemento en la posicion indicada, donde 0 es el primer
 * elemento.
 *
 * Si no existe dicha posicion devuelve NULL.
 */
void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){

	if(!lista || lista_vacia(lista) || posicion >= (lista->tamanio))
		return NULL;

	nodo_t* nodo_posicion = obtener_puntero_a_nodo(lista, posicion);
	if(!nodo_posicion)
		return NULL;

	return nodo_posicion->elemento;
}

/* 
 * Devuelve el último elemento de la lista o NULL si la lista se
 * encuentra vacía.
 */
void* lista_ultimo(lista_t* lista){

	if(!lista || lista_vacia(lista))
		return NULL;

	return lista->nodo_fin->elemento;
}

/* 
 * Devuelve true si la lista está vacía o false en caso contrario.
 */
bool lista_vacia(lista_t* lista){

	if(!lista)
		return true;

	return lista->tamanio == SIN_ELEMENTOS;
}

/*
 * Devuelve la cantidad de elementos almacenados en la lista.
 */
size_t lista_elementos(lista_t* lista){

	if(!lista)
		return 0;
	
	return lista->tamanio;

}

// pre: 
// pos: vacia toda la lista. Si lista no existe no hace nada
void lista_vaciar(lista_t* lista){

	if(!lista)
		return;

	while(!lista_vacia(lista))
		lista_borrar(lista);
	
}

/*
 * Libera la memoria reservada por la lista.
 */
void lista_destruir(lista_t* lista){

	if(!lista)
		return;

	if(!lista_vacia(lista))
		lista_vaciar(lista);

	free(lista);
}

/*
 * Crea un iterador para una lista. El iterador creado es válido desde
 * el momento de su creación hasta que no haya mas elementos por
 * recorrer o se modifique la lista iterada (agregando o quitando
 * elementos de la lista).
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
lista_iterador_t* lista_iterador_crear(lista_t* lista){

	if(!lista)
		return NULL;

	lista_iterador_t* iter = malloc(sizeof(lista_iterador_t));
	if(!iter)
		return NULL;

	iter->lista = lista;
	iter->corriente = NULL;

	return iter;
}

/*
 * Devuelve true si hay mas elementos sobre los cuales iterar o false
 * si no hay mas.
 */
bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){

	if(!iterador)
		return false;

	if(!iterador->corriente)
		return !lista_vacia(iterador->lista);

	return (iterador->corriente->siguiente != NULL);

}

/*
 * Avanza el iterador al proximo elemento y lo devuelve.
 * En caso de error devuelve NULL.
 */
void* lista_iterador_siguiente(lista_iterador_t* iterador){

	if(!iterador || lista_vacia(iterador->lista))
		return NULL;

	if(!iterador->corriente)
		iterador->corriente = iterador->lista->nodo_inicio;
	
	else if(lista_iterador_tiene_siguiente(iterador))
		iterador->corriente = iterador->corriente->siguiente;
	
	else
		return NULL;


	return iterador->corriente->elemento;
}

/*
 * Libera la memoria reservada por el iterador.
 */
void lista_iterador_destruir(lista_iterador_t* iterador){

	if(!iterador)
		return;

	free(iterador);
}

/*
 * Iterador interno. Recorre la lista e invoca la funcion con cada
 * elemento de la misma.
 */
void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*)){

	if(!lista)
		return;

	nodo_t* corriente = lista->nodo_inicio;
	
	while(corriente != NULL){
		funcion(corriente->elemento);
		corriente = corriente->siguiente;
	}
}
