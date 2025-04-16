#include "infomain.h"
#include <iostream>
using namespace std;
// CREACION DE LAS FUNCIONES, IMPLEMENTACION COMPLETA DE TODAS Y CADA UNA DE ELLAS

//FUNCIONES DE TRANSFORMACIONES

int suma(int a,int b){
    return (a+b);
}


//rotar(&valora_rotar,&cantidad_arotar)
/* esta funcion se encarga de rotar el bit */


//girar(&cantidad_arotar)
/* esta funcion se encarga de rotar el valor binario la cantidad que sea necsaria */


//convetira_binario(int &valor,int posicion) //posicion=derecha o izquierda, valor=puntero


//convetira_valor(int &valor, int posicion)
/* proceso inversi de convertir a binario, para recuperar el valor*/


//desplazamientos (short int posicion, short int valor, &mascaraeimagen)
/* esta funcion se encarga de retornar un valor binario con desplazamiento de
 * 1 a 8*/


//xor_unpixel(puntero_pixel,puntero_imagen)
//puntero_pixel= D.M del pixel - puntero_imagen=D.M imagen completa (distorcionada)
/* esta funcion se encarga de aplicar el xOr en el valor de un pixel*/



//encontrar_posicion(semilla)
/*esta funcion se ubica en la posicion deseada de la base de datos (imagen distorcionada, ruido gausseano
 * y mascara) que vamos a utilizar para comprar los txt y ver si son los mismos valores*/


//semilla(archivo)
//esta funcion encuentra el valor de la semilla guardada en el txt. lee archivo, primera linea y retorna el numero


//encontrar_transformaciones(&filtros,basededatosimagen,basedatosxOr)
/* esta funcion se utilia para conocer los filtros que se le aplicaron a la imagen
 * y retorna un puntero que apunta a un arreglo con dos posiciones (la primera posicion guarda el valor de 1 o 0
 * pasar saber si se hizo xOr y la segunda posicion de 0 a 7 para saber cuanto se rota */














//FUNCIONES DE MASCARA

//desplazamiento_mascara(DMimagen,DMmascara,anchoI, anchoM)
/*esta funcion se encarga de ver en que partes de la imagen verdaderamente se debe poner la mascara
encuentra las posiciones especificas, para poder restar la mascara
retorna dos valores (arreglo 2[], ancho mascara, cantidad de salto de pixeles) */



//quitar_mascara(retorno_desplazamientomascara,semilla,basedatosmascara,basedatosimagendistorsionada)
/*Esta funcion se encarga de modificar la base de datos de la imagen quitandole los valores de la mascara
 * donde los tenga */





//basededatosimagen=imagen distorsionada completa





//FUNCIONES DE LA IMAGEN COMPLETA

//imagenfinal(&filtros,basedatosimagen,)
/*esta funcion se utiliza en la primera parte del programa para hallar las transformaciones
 * iniciales para llegar al segundo paso, y se utiliza para llegar al ultimo paso
 * y hallar la imagen original*/




//xor_todaimagen(basededatosimagen,basedatosruidogausseano)
/* esta funcion se encarga de aplicar los Xor a toda la imagen*/



//rotaciones_todalaimagen(basedatosimagen,&valorarotar)
/* esta funcion se encarga de aplicar las rotaciones a toda la imagen
 valorarotar= el valor del fitro de la posicion #2 de la funcion encontra transformaciones*/









