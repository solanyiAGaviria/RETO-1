/*
 * Programa demostrativo de manipulaciónprocesamiento de imágenes BMP en C++ usando Qt.
 *
 * Descripción:
 * Este programa realiza las siguientes tareas:
 * 1. Carga una imagen BMP desde un archivo (formato RGB sin usar estructuras ni STL).
 * 2. Modifica los valores RGB de los píxeles asignando un degradado artificial basado en su posición.
 * 3. Exporta la imagen modificada a un nuevo archivo BMP.
 * 4. Carga un archivo de texto que contiene una semilla (offset) y los resultados del enmascaramiento
 *    aplicados a una versión transformada de la imagen, en forma de tripletas RGB.
 * 5. Muestra en consola los valores cargados desde el archivo de enmascaramiento.
 * 6. Gestiona la memoria dinámicamente, liberando los recursos utilizados.
 *
 * Entradas:
 * - Archivo de imagen BMP de entrada ("I_O.bmp").
 * - Archivo de salida para guardar la imagen modificada ("I_D.bmp").
 * - Archivo de texto ("M1.txt") que contiene:
 *     • Una línea con la semilla inicial (offset).
 *     • Varias líneas con tripletas RGB resultantes del enmascaramiento.
 *
 * Salidas:
 * - Imagen BMP modificada ("I_D.bmp").
 * - Datos RGB leídos desde el archivo de enmascaramiento impresos por consola.
 *
 * Requiere:
 * - Librerías Qt para manejo de imágenes (QImage, QString).
 * - No utiliza estructuras ni STL. Solo arreglos dinámicos y memoria básica de C++.
 *
 * Autores: Augusto Salazar Y Aníbal Guerra
 * Fecha: 06/04/2025
 * Asistencia de ChatGPT para mejorar la forma y presentación del código fuente
 */
#include <QCoreApplication>
#include <QImage>
#include <iostream>
#include <fstream>
using namespace std;

unsigned char* loadPixels(QString input, int &width, int &height);
unsigned char* loadPixels2(QString input);
bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida);

bool verificacion_imagenes(unsigned char* resolver ,unsigned char* gaussiana,unsigned char* mascara,int alto1,int alto2,int ancho1, int ancho2);
bool verificaciontxt(unsigned int* tmt);
bool prueba_xor(unsigned char &res_esperado,unsigned char* gaussiana,unsigned char* resolver, int &semilla);
void imagen_xor(unsigned char* gaussiana,unsigned char* resolver,int &bits);
unsigned char rotar_r(unsigned char byte, short int n_bits);
unsigned char rotar_l(unsigned char byte, short int n_bits);
bool prueba_l(unsigned char* resolver,unsigned char &res_esperado,int &semilla,short &n_bits);
bool prueba_r(unsigned char* resolver,unsigned char &res_esperado,int &semilla, short int &n_bits);
void imagen_rotar_l(unsigned char *resolver, short int &n_bits, int &bits);
void imagen_rotar_r(unsigned char* resolver,short int &n_bits,int &bits);
unsigned int* loadSeedMasking2(const char* nombreArchivo, int &seed);

int main()
{
    int cantidad_txt=6; // ingrese el numero del ultimo archivo txt.
    // Definición de rutas de archivo de entrada (imagen original), resultado (imagen modificada), imagen de ruido gausiano, mascara
    QString imagen_resolver ="C:/Users/kewi/terra/I_D.bmp";
    QString imagengaussiana ="C:/Users/kewi/terra/I_M.bmp";
    QString mascara = "C:/Users/kewi/terra/M.bmp";
    QString resultado= "C:/Users/kewi/terra/resutado.bmp";



    // Variables para almacenar las dimensiones de la imagen
    int altura_resolver = 0;
    int ancho_resolver = 0;
    int altura_gaussiana = 0;
    int ancho_gaussiana = 0;



    // Carga la imagen BMP en memoria dinámica y obtiene ancho y alto
    unsigned char *datos_resolver= loadPixels(imagen_resolver, ancho_resolver, altura_resolver);
    unsigned char *datos_gaussiana= loadPixels(imagengaussiana, ancho_gaussiana, altura_gaussiana);
    unsigned char *datos_mascara= loadPixels2(mascara);

    //verificaion que los dotos se cargan correctamente
    if(verificacion_imagenes(datos_resolver ,datos_gaussiana,datos_mascara,altura_resolver,altura_gaussiana,ancho_resolver, ancho_gaussiana)){
        cout  <<"Error en carga de imágenes. " << endl;
        cout << "           O " << endl;
        cout << "los tamaños de la imagen gausiana y distorcionada no funcionan " << endl;
        delete[] datos_gaussiana;
        delete[] datos_mascara;
        delete[] datos_resolver;
        return -1;
    }
    int cant_bit= altura_resolver * ancho_resolver * 3;


    for(int i=cantidad_txt ;i>=0;i--){
        QString nombreArchivoTxt = "C:/Users/kewi/terra" + QString("/M%1.txt").arg(i);
        int semilla=0;
        unsigned int *pista = loadSeedMasking2( nombreArchivoTxt.toStdString().c_str(), semilla);
        if (verificaciontxt(pista)){
            cout  <<"error en subir el archivo " << endl;
            delete[] datos_gaussiana;
            delete[] datos_mascara;
            delete[] datos_resolver;
            delete[] pista;
            return -1;
        }
        // resultado esperado == pista[0]-mascara[0]
        unsigned char res_esperado = pista[0]-datos_mascara[0];
        delete[] pista;

        // prueba de xor
        if(prueba_xor(res_esperado,datos_gaussiana,datos_resolver,semilla)){
            imagen_xor(datos_gaussiana,datos_resolver,cant_bit);
            cout << "a la imagen se le hizo un xOr";
            cout << "  "<<endl;
        }
        else{
            //pruebas rotaciones

            short int n_bits=0;
            bool izquierda=prueba_l(datos_resolver,res_esperado,semilla,n_bits);
            bool derecha=prueba_r(datos_resolver,res_esperado,semilla,n_bits);
            if (derecha){
                imagen_rotar_r(datos_resolver,n_bits,cant_bit);
                cout << "a la imagen se le hizo una ratacion a la derecha de "<< n_bits<<endl;
                cout << "  "<<endl;
            }
            else if(izquierda){
                imagen_rotar_l(datos_resolver,n_bits,cant_bit);
                bool exportI = exportImage(datos_resolver, ancho_resolver, altura_resolver, resultado);
                cout << exportI<<endl;
                cout << "a la imagen se le hizo una ratacion a la izquierda de "<< n_bits<<endl;
                cout << "  "<<endl;
            }

            else{
                cout<< "no se encontro ninguna tranformacion"<<endl;
            }
        }
    }
    bool exportI = exportImage(datos_resolver, ancho_resolver, altura_resolver, resultado);
    delete[] datos_gaussiana;
    delete[] datos_mascara;
    delete[] datos_resolver;


    return exportI; // Fin del programa
}

unsigned char* loadPixels(QString input, int &width, int &height){
    /*
 * @brief Carga una imagen BMP desde un archivo y extrae los datos de píxeles en formato RGB.
 *
 * Esta función utiliza la clase QImage de Qt para abrir una imagen en formato BMP, convertirla al
 * formato RGB888 (24 bits: 8 bits por canal), y copiar sus datos de píxeles a un arreglo dinámico
 * de tipo unsigned char. El arreglo contendrá los valores de los canales Rojo, Verde y Azul (R, G, B)
 * de cada píxel de la imagen, sin rellenos (padding).
 *
 * @param input Ruta del archivo de imagen BMP a cargar (tipo QString).
 * @param width Parámetro de salida que contendrá el ancho de la imagen cargada (en píxeles).
 * @param height Parámetro de salida que contendrá la altura de la imagen cargada (en píxeles).
 * @return Puntero a un arreglo dinámico que contiene los datos de los píxeles en formato RGB.
 *         Devuelve nullptr si la imagen no pudo cargarse.
 *
 * @note Es responsabilidad del usuario liberar la memoria asignada al arreglo devuelto usando `delete[]`.
 */

    // Cargar la imagen BMP desde el archivo especificado (usando Qt)
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << std::endl;
        return nullptr; // Retorna un puntero nulo si la carga falló
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    // Obtiene el ancho y el alto de la imagen cargada
    width = imagen.width();
    height = imagen.height();

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width * height * 3;

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    // Retorna el puntero al arreglo de datos de píxeles cargado en memoria
    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida){
    /*
 * @brief Exporta una imagen en formato BMP a partir de un arreglo de píxeles en formato RGB.
 *
 * Esta función crea una imagen de tipo QImage utilizando los datos contenidos en el arreglo dinámico
 * `pixelData`, que debe representar una imagen en formato RGB888 (3 bytes por píxel, sin padding).
 * A continuación, copia los datos línea por línea a la imagen de salida y guarda el archivo resultante
 * en formato BMP en la ruta especificada.
 *
 * @param pixelData Puntero a un arreglo de bytes que contiene los datos RGB de la imagen a exportar.
 *                  El tamaño debe ser igual a width * height * 3 bytes.
 * @param width Ancho de la imagen en píxeles.
 * @param height Alto de la imagen en píxeles.
 * @param archivoSalida Ruta y nombre del archivo de salida en el que se guardará la imagen BMP (QString).
 *
 * @return true si la imagen se guardó exitosamente; false si ocurrió un error durante el proceso.
 *
 * @note La función no libera la memoria del arreglo pixelData; esta responsabilidad recae en el usuario.
 */

    // Crear una nueva imagen de salida con el mismo tamaño que la original
    // usando el formato RGB888 (3 bytes por píxel, sin canal alfa)
    QImage outputImage(width, height, QImage::Format_RGB888);

    // Copiar los datos de píxeles desde el buffer al objeto QImage
    for (int y = 0; y < height; ++y) {
        // outputImage.scanLine(y) devuelve un puntero a la línea y-ésima de píxeles en la imagen
        // pixelData + y * width * 3 apunta al inicio de la línea y-ésima en el buffer (sin padding)
        // width * 3 son los bytes a copiar (3 por píxel)
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    // Guardar la imagen en disco como archivo BMP
    if (!outputImage.save(archivoSalida, "BMP")) {
        // Si hubo un error al guardar, mostrar mensaje de error
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false; // Indica que la operación falló
    } else {
        // Si la imagen fue guardada correctamente, mostrar mensaje de éxito
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true; // Indica éxito
    }

}

unsigned int* loadSeedMasking2(const char* nombreArchivo, int &seed){
    /*
 * @brief Carga la semilla y los resultados del enmascaramiento desde un archivo de texto.
 *
 * Esta función abre un archivo de texto que contiene una semilla en la primera línea y,
 * a continuación, una lista de valores RGB resultantes del proceso de enmascaramiento.
 * Primero cuenta cuántos tripletes de píxeles hay, luego reserva memoria dinámica
 * y finalmente carga los valores en un arreglo de enteros.
 *
 * @param nombreArchivo Ruta del archivo de texto que contiene la semilla y los valores RGB.
 * @param seed Variable de referencia donde se almacenará el valor entero de la semilla.
 * @param n_pixels Variable de referencia donde se almacenará la cantidad de píxeles leídos
 *                 (equivalente al número de líneas después de la semilla).
 *
 * @return Puntero a un arreglo dinámico de enteros que contiene los valores RGB
 *         en orden secuencial (R, G, B, R, G, B, ...). Devuelve nullptr si ocurre un error al abrir el archivo.
 *
 * @note Es responsabilidad del usuario liberar la memoria reservada con delete[].
 */

    // Abrir el archivo que contiene la semilla y los valores RGB
    int n_pixels =0;
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        // Verificar si el archivo pudo abrirse correctamente
        cout << "No se pudo abrir el archivo." << endl;
        return nullptr;
    }

    // Leer la semilla desde la primera línea del archivo
    archivo >> seed;

    int r, g, b;

    // Contar cuántos grupos de valores RGB hay en el archivo
    // Se asume que cada línea después de la semilla tiene tres valores (r, g, b)
    while (archivo >> r >> g >> b) {
        n_pixels++;  // Contamos la cantidad de píxeles
    }

    // Cerrar el archivo para volver a abrirlo desde el inicio
    archivo.close();
    archivo.open(nombreArchivo);

    // Verificar que se pudo reabrir el archivo correctamente
    if (!archivo.is_open()) {
        cout << "Error al reabrir el archivo." << endl;
        return nullptr;
    }

    // Reservar memoria dinámica para guardar todos los valores RGB
    // Cada píxel tiene 3 componentes: R, G y B
    unsigned int* RGB = new unsigned int[n_pixels * 3];

    // Leer nuevamente la semilla desde el archivo (se descarta su valor porque ya se cargó antes)
    archivo >> seed;

    // Leer y almacenar los valores RGB uno por uno en el arreglo dinámico
    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    // Cerrar el archivo después de terminar la lectura
    archivo.close();

    // Mostrar información de control en consola
    //cout << "Semilla: " << seed << endl;
    //cout << "Cantidad de píxeles leídos: " << n_pixels << endl;

    // Retornar el puntero al arreglo con los datos RGB
    return RGB;
}

unsigned char* loadPixels2(QString input){

    /*
 * @brief Carga una imagen BMP desde un archivo y extrae los datos de píxeles en formato RGB.
 *
 * Esta función utiliza la clase QImage de Qt para abrir una imagen en formato BMP, convertirla al
 * formato RGB888 (24 bits: 8 bits por canal), y copiar sus datos de píxeles a un arreglo dinámico
 * de tipo unsigned char. El arreglo contendrá los valores de los canales Rojo, Verde y Azul (R, G, B)
 * de cada píxel de la imagen, sin rellenos (padding).
 *
 * @param input Ruta del archivo de imagen BMP a cargar (tipo QString).
 * @return Puntero a un arreglo dinámico que contiene los datos de los píxeles en formato RGB. *         Devuelve nullptr si la imagen no pudo cargarse.
 *

 * @note Es responsabilidad del usuario liberar la memoria asignada al arreglo devuelto usando `delete[]`.
 */

    // Cargar la imagen BMP desde el archivo especificado (usando Qt)
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << std::endl;
        return nullptr; // Retorna un puntero nulo si la carga falló
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    // Obtiene el ancho y el alto de la imagen cargada
    int width = imagen.width();
    int height = imagen.height();

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width * height * 3;

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    // Retorna el puntero al arreglo de datos de píxeles cargado en memoria
    return pixelData;
}

bool verificacion_imagenes(unsigned char* resolver ,unsigned char* gaussiana,unsigned char* mascara,int alto1,int alto2,int ancho1, int ancho2){
    /*
     *  utilidad: verifica que los datos de las imagenes que se hayan cargado y
     *  que los tamaños de las imagenes de ruido y distorcionada sean iguales
     *
     *
     *  @param resolver puntero de la array de imagen distorcionada
     *  @param gaussiana  puntero de la array de imagen gausinana
     *  @param mascara  puntero de la array de imagen de la mascra
     *  @param alto1 valor int dela altura de una de las imagenes de gaussiana o distorcinada
     *  @param alto2 valor int dela altura de una de las imagenes de gaussiana o distorcinada
     *  @param ancho1 valor int dela altura de una de las imagenes de gaussiana o distorcinada
     *  @param ancho2 valor int dela altura de una de las imagenes de gaussiana o distorcinada
     *
     *  @return returna true significa que un datos de las imagenes no esta bien cargadas
     *   o los tamaños de las imagenes gaussiana y distorcinada no son iguales
     */


    if (!resolver || !gaussiana || !mascara || alto1 != alto2 || ancho1 != ancho2) {
        return true;
    }
    return false;
}

bool verificaciontxt(unsigned int* tmt){
    /*
     * utilidad: verificaion de los datos del archivo se hayan cargado correctamente
     *
     * @param tmt puntero de la array de los valores del txt
     *
     * @return retorna true significa que no se subieron los datos correctamente
     *
     */


    if (!tmt) {
        return true;
    }
    return false;
}

bool prueba_xor(unsigned char &res_esperado,unsigned char* gaussiana,unsigned char* resolver, int &semilla){

    /*
     * utilidad: comprueba que el valor esperado sea igual al calculado, en el caso de xor
     *
     *@param res_esperado varibale que contiene el valor esperado
     *@param gaussiana  puntero de la array de imagen gausinana
     *@param resolver  puntero de la array de imagen resolver
     *@param semilla variable que contiene el valor de donde se aplica la mascara
     *
     *
     *@return retorna true si los valores son iguales, significando que se tiene que hacer un xor
     */
    unsigned char determinar = gaussiana[semilla] ^ resolver[semilla];
    if ( res_esperado==determinar){
        return true;
    }
    return false;
}

void imagen_xor(unsigned char* gaussiana,unsigned char* resolver,int &bits){
    /*
     *funcion: hacer la operacion xor a toda la datos
     *
     *@param bits variables con la cantidad de bits de la imagen distorcionada
     *@param gaussiana  puntero de la array de imagen gausinana
     *@param resolver  puntero de la array de imagen resolver
     *
     */
    for(int i=0; i<bits ;i++ ){
        resolver[i]= gaussiana[i] ^ resolver[i];
    }
}

bool prueba_l(unsigned char* resolver,unsigned char &res_esperado,int &semilla,short int &n_bits){
    /*
     *funcion: determina si se hizo una rotacion de bits a la izquiedad
     *
     *@param res_esperado varibale que contiene el valor esperado
     *@param semilla  varibale que contiene el valor de donde se aplica la mascara
     *@param n_bits varibale que contiene el valor de cuantos bits rotamos
     *@param resolver  puntero de la array de imagen resolver
     *
     *@return retorna true si el valor concide con el valor esperado
     */
    for(int i=1;i<8;i++){
        if(res_esperado==rotar_l(resolver[semilla],i)){
            n_bits=i;
            return true;
        }
    }
    return false;
}

unsigned char rotar_l(unsigned char byte, short int n_bits) {
    /*
     * funcion: rota un una cantidad de n de bit el valor de una variable unsigned char a la izquierda
     *
     * @param byte varible que vamos aplicarle la rotacion
     * @param n_bits varibale que contiene el valor de cuantos bits rotamos
     *
     * @return retorna el valor de la variable rotada la cantidad de bits
     */
    n_bits = n_bits % 8;
    return ((byte << n_bits) | (byte >> (8 - n_bits))) & 0xFF;
}

bool prueba_r(unsigned char* resolver,unsigned char &res_esperado,int &semilla,short int &n_bits){
    /*
     *funcion: determina si se hizo una rotacion de bits a la derecha
     *
     *@param res_esperado varibale que contiene el valor esperado
     *@param semilla  varibale que contiene el valor de donde se aplica la mascara
     *@param n_bits varibale que contiene el valor de cuantos bits rotamos
     *@param resolver  puntero de la array de imagen resolver
     *
     *@return retorna true si el valor concide con el valor esperado
     */
    for(int i=1;i<8;i++){
        if(res_esperado==rotar_r(resolver[semilla],i)){
            n_bits=i;
            return true;
        }
    }
    return false;
}

unsigned char rotar_r(unsigned char byte,short int n_bits) {
    /*
     * funcion: rota un una cantidad de n de bit el valor de una variable unsigned char a la derecha
     *
     * @param byte varible que vamos aplicarle la rotacion
     * @param n_bits varibale que contiene el valor de cuantos bits rotamos
     * @param resolver  puntero de la array de imagen resolver
     * @return retorna el valor de la variable rotada la cantidad de bits
     */
    n_bits = n_bits % 8;
    return ((byte >> n_bits) | (byte << (8 - n_bits))) & 0xFF;
}

void imagen_rotar_l(unsigned char* resolver,short int &n_bits,int &bits){
    /*
     *funcion: rota todos los valores de la imagen, una cantinada de n bits a la izquierdad
     *
     * @param resolver  puntero de la array de imagen resolver
     * @param n_bits varibale que contiene el valor de cuantos bits rotamos
     * @param bits variables con la cantidad de bits de la imagen distorcionada
     */
    for(int i=0;i<bits;i++){
        resolver[i]= rotar_l(resolver[i],n_bits);
    }

}

void imagen_rotar_r(unsigned char* resolver,short int &n_bits,int &bits){
    /*
     *funcion: rota todos los valores de la imagen, una cantinada de n bits a la derecha
     *
     * @param resolver  puntero de la array de imagen resolver
     * @param n_bits varibale que contiene el valor de cuantos bits rotamos
     * @param bits variables con la cantidad de bits de la imagen distorcionada
     */
    for(int i=0;i<bits;i++){
        resolver[i]= rotar_r(resolver[i],n_bits);
    }

}



