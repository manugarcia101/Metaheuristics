#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <conio.h>
#include <stdio.h>
#include <windows.h>

#define ARRIBA 72
#define ABAJO 80
#define DERECHA 77
#define IZQUIERDA 75
#define ENTER '\r'
#define TAM_POB 50
#define COMPROBACIONES 20000

using namespace std;
using namespace chrono;

struct dato{
    int trans1;
    int trans2;
    int dif;
    int pen;
};

struct individuo{
    vector<int> solucion;
    int coste;
};

struct individuoReiniciar{
    vector<int> solucion;
    int coste;
    int cont;
};

void loadVar(string *name, vector <int> *v, vector<int> *t);
void loadDom(string *name, vector <vector <int> > *v);
void loadCtr(string *name, vector <dato> *w, vector < vector <dato> > *v, vector<int> *t);

void inicializacion(vector<int> *var, vector <vector <int> > *dom, vector<dato> *ctr, vector< vector<dato> > *ctr2, vector< individuo > *poblacion, bool reiniciar);

void Greedy(vector<int> *var, vector <vector <int> > *dom, vector<dato> *ctr, vector< vector<dato> > *ctr2, individuo *individuo);
int ComprobarGrasp(vector<int> *var, vector< vector<dato> > *ctr2, vector< vector<int> > *dom, vector<int> *sol, int *trans);

void mutacion(vector<int> *var, vector <vector <int> > *dom, individuo *individuo);

void cruceDosPuntos(vector<int> *padre1, vector<int> *padre2, vector<int> *hijo1, vector<int> *hijo2);
void cruceBLX(vector<int> *var, vector <vector <int> > *dom, vector<int> *padre1, vector<int> *padre2, vector<int> *hijo1, vector<int> *hijo2);

int Comprobar(vector<int> *var, vector<dato> *ctr, vector <vector <int> > *dom, vector<int> *sol, int *cont);
int ComprobarFact(vector<int> *var, vector<dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol, int *transOldFreq, int *trans, int *valor);

bool reiniciar(vector<individuo> *poblacion);

void ordenar(vector<individuo> *poblacion);

void competicion(vector<individuo> *hijos, vector<individuo> *poblacion);

float mediaPoblacion(vector<individuo> *poblacion);

vector<individuo> torneoBinarioAGG(vector<individuo> *poblacion);
vector<individuo> torneoBinarioAGE(vector<individuo> *poblacion);

bool comparacion(const individuo &individuo1, const individuo &individuo2){
    if(individuo1.coste < individuo2.coste){
        return true;
    }else{
        return false;
    }
}

void AGGDosPuntos(int *cont, vector<individuo> *poblacion);
void AGGBLX(int *cont, vector<individuo> *poblacion);
void AGEDosPuntos(int *cont, vector<individuo> *poblacion);
void AGEBLX(int *cont, vector<individuo> *poblacion);

//Variables para calcular tiempos.
high_resolution_clock::time_point t1, t2, t3, t4, t5, t6, t7, t8;
duration<double> total, total1, total2, total3, total4;

//Estructuras de datos para guardar los ficheros.
vector<int> var;
vector<vector<int> > dom;
vector<int> traducciones;
vector<dato> ctr;
vector<vector<dato> > ctr2;

//Variable para controlar los cout de los distintos hilos.
mutex mtx;

void gotoxy(int x,int y){
    HANDLE hcon;
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y= y;
    SetConsoleCursorPosition(hcon,dwPos);
}

int main()
{
    srand(77381751);

    system("color 1e");

    /***********************************************************************************/
    /*Inicializacion de la interfaz para elegir archivos.*/

    string names[5][2] = {
                         {"scen06", "graph05"},
                         {"scen07", "graph06"},
                         {"scen08", "graph07"},
                         {"scen09", "graph11"},
                         {"scen10", "graph12"}
                         };
    string name;

    char tecla;
    int x = 6, y = 3;

    gotoxy(x, y); cout << "==>";
    gotoxy(5, 1); printf("Elige un fichero para aplicar el algoritmo:");
    gotoxy(5, 2); printf("------------------------------------------------------");
    gotoxy(10, 3); printf("scen06.");
    gotoxy(10, 4); printf("scen07.");
    gotoxy(10, 5); printf("scen08.");
    gotoxy(10, 6); printf("scen09.");
    gotoxy(10, 7); printf("scen10.");
    gotoxy(30, 3); printf("graph05.");
    gotoxy(30, 4); printf("graph06.");
    gotoxy(30, 5); printf("graph07.");
    gotoxy(30, 6); printf("graph11.");
    gotoxy(30, 7); printf("graph12.");

    do{
        tecla = _getch();

        if(tecla == ARRIBA){
            if(y > 3){
                gotoxy(x, y); cout << "   ";
                y--;
            }
        }
        if(tecla == ABAJO){
            if(y < 7){
                gotoxy(x, y); cout << "   ";
                y++;
            }
        }
        if(tecla == DERECHA){
            if(x == 6){
                gotoxy(x, y); cout << "   ";
                x = 26;
            }
        }
        if(tecla == IZQUIERDA){
            if(x == 26){
                gotoxy(x, y); cout << "   ";
                x = 6;
            }
        }

        gotoxy(x, y); cout << "==>";
        gotoxy(38, 7);

    }while(tecla != ENTER);

    if(x == 6){
        x = 0;
    }else{
        x = 1;
    }

    name = names[y-3][x];

    /*Fin de la interfaz.*/
    /***********************************************************************************/

    system("cls");

    cout << "TRABAJAREMOS CON EL ARCHIVO: " << name << endl;

    t1 = high_resolution_clock::now();
    loadVar(&name, &var, &traducciones);

    loadDom(&name, &dom);

    ctr2.resize(var.size());
    loadCtr(&name, &ctr, &ctr2, &traducciones);
    t2 = high_resolution_clock::now();
    total = t2-t1;
    cout << "TIEMPO LEYENDO FICHEROS: " << total.count() << " segundos." << endl;

    cout << "NUMERO DE TRANSMISORES: " << var.size() << " Y " << ctr.size() << " CONSTRAINS." << endl << endl;

    vector<individuo> poblacion, poblacion1, poblacion2, poblacion3, poblacion4;

    inicializacion(&var, &dom, &ctr, &ctr2, &poblacion, false);

    poblacion1 = poblacion;
    poblacion2 = poblacion;
    poblacion3 = poblacion;
    poblacion4 = poblacion;

    int iter1, iter2, iter3, iter4;


    thread hilo1(AGGDosPuntos, &iter1, &poblacion1);
    thread hilo2(AGGBLX, &iter2, &poblacion2);
    thread hilo3(AGEDosPuntos, &iter3, &poblacion3);
    thread hilo4(AGEBLX, &iter4, &poblacion4);

    hilo1.join();
    hilo2.join();
    hilo3.join();
    hilo4.join();

    gotoxy(0, 8);
    cout << endl;
    cout << "Greedy:                            Mejor individuo coste (" << poblacion.at(0).coste << ")" << endl;
    cout << "A.G. Generacional con dos puntos: " << iter1 << " iteraciones" << endl;
    cout << "A.G. Generacional con dos puntos:  Mejor individuo coste (" << poblacion1.at(0).coste << ")" << endl;
    cout << "A.G. Generacional con BLX:        " << iter2 << " iteraciones" << endl;
    cout << "A.G. Generacional con BLX:         Mejor individuo coste (" << poblacion2.at(0).coste << ")" << endl;
    cout << "A.G. Estacionario con dos puntos: " << iter3 << " iteraciones" << endl;
    cout << "A.G. Estacionario con dos puntos:  Mejor individuo coste (" << poblacion3.at(0).coste << ")" << endl;
    cout << "A.G. Estacionario con BLX:        " << iter4 << " generaciones" << endl;
    cout << "A.G. Estacionario con BLX:         Mejor individuo coste (" << poblacion4.at(0).coste << ")" << endl;

    do{
        gotoxy(0, 18);
        cout << endl << "Pulsa ENTER para salir.";
        tecla = _getch();
    }while(tecla != ENTER);

    return 0;
}

void loadVar(string *name, vector <int> *v, vector<int> *t){
    string fileName = "instancias/"+*name+"/var.txt";
    string line;
    ifstream myfile;
    myfile.open(fileName.c_str());
    string a, b;
    stringstream ss;
    while(getline(myfile,line)){
        if(line != " "){
            ss.str(line);
            ss >> a;
            ss >> b;
            t->push_back(atoi(a.c_str()));
            v->push_back((atoi(b.c_str())-1));
            ss.clear();
        };
    };
    myfile.close();
}

void loadDom(string *name, vector <vector<int> > *v){
    vector<int> w;
    string fileName = "instancias/"+*name+"/dom.txt";
    string line, var;
    stringstream ss;
    ifstream myfile;
    myfile.open(fileName.c_str());
    int i;
    while(getline(myfile,line)){
        if(line != " "){
            ss.str(line);
            ss >> i;
            //Salta el valor que indica el numero de frecuencias.
            ss.get();
            while(!ss.eof()){
                ss >> var;
                w.push_back(atoi(var.c_str()));
            }
            v->push_back(w);
            w.clear();
            ss.clear();
        };
    };
    myfile.close();
}

void loadCtr(string *name, vector <dato> *w, vector< vector <dato> > *v, vector<int> *t){
    string fileName = "instancias/"+*name+"/ctr.txt";
    string line, var;
    stringstream ss;
    ifstream myfile;
    myfile.open(fileName.c_str());
    string a, b, c, d, e, f;
    dato dato;
    while(getline(myfile,line)){
        if(line != "  "){
            ss.str(line);
            ss >> a;
            ss >> b;
            ss >> c;
            ss >> d;
            ss >> e;
            ss >> f;
            if(c != "D"){
                bool aEncontrado = false, bEncontrado = false;
                for(unsigned int i = 0; i < t->size(); i++){
                    if(t->at(i) == atoi(a.c_str()) && !aEncontrado){
                        aEncontrado = true;
                        dato.trans1 = i;
                    }
                    if(t->at(i) == atoi(b.c_str()) && !bEncontrado){
                        bEncontrado = true;
                        dato.trans2 = i;
                    }
                    if(aEncontrado && bEncontrado){
                        break;
                    }
                }
                dato.dif = atoi(e.c_str());
                dato.pen = atoi(f.c_str());
                w->push_back(dato);
            }
            ss.clear();
        };
    };
    myfile.close();


    for(unsigned int i = 0; i < w->size(); i++){
        v->at(w->at(i).trans1).push_back(w->at(i));
        v->at(w->at(i).trans2).push_back(w->at(i));
    }
}

//Esta función sirve como método de arranque para establecer la población inicial
void inicializacion(vector<int> *var, vector <vector <int> > *dom, vector<dato> *ctr, vector< vector<dato> > *ctr2, vector< individuo > *poblacion, bool reiniciar){
	//Creo un individuo auxiliar de relleno con la función Greedy(es un individuo).
	individuo individuoAux;
	//Si hay que reiniciar guardamos el mejor porque esta ordenada la poblacion, limpiamos y volvemos a insertar.
    int pos = 0;
    if(reiniciar){
        pos++;
        individuoAux = poblacion->at(0);
        poblacion->clear();
        poblacion->push_back(individuoAux);
    }
	//Generar 50 o 49 individuos con Greedy e insertarlos en poblacion.
	for (unsigned int i = pos; i < 50; i++){
		//Genera un individuo.
		Greedy(var, dom, ctr, ctr2, &individuoAux);
		//Introduce cada vez un individuo en la poblacion, nos devuelve un individuo con su coste ya calculado.
		poblacion->push_back(individuoAux);
	}
	ordenar(poblacion);
}

void Greedy(vector<int> *var, vector <vector <int> > *dom, vector<dato> *ctr, vector< vector<dato> > *ctr2, individuo *individuo){
    //Limpiamos el vector solucion por si se ha mandado con datos que no queremos.
    individuo->solucion.clear();
    individuo->solucion.resize(var->size(), -1);
    //Elegimos el transmisor aleatoriamente.
    int trans = rand()%individuo->solucion.size();
    //insertamos para el cualquier transmisor una frecuencia aleatoria dentrode las que tiene disponibles.
    individuo->solucion.at(trans) = (rand()%(dom->at(var->at(trans)).size()));

    //Para el resto de transmisores elegimos una frecuencia aleatoria y a partir
    //de ahi comprobamos todos sus vecinos y nos quedamos con la de menor coste.
    int posInicial = trans;
    //Aumentamos una posicion trans sin que se salga de rango.
    trans++;
    trans = trans%individuo->solucion.size();
    while(trans != posInicial){

        int pos = trans;
        int pen = INT_MAX;
        int freq = 0;

        //Elegimos un nemero aleatorio para empezar y otro para comprobrar hacia arriba o abajo.
        int j = rand()%dom->at(var->at(trans)).size();
        int iniPos = j;
        int aleatorio = rand()%2;
        if(aleatorio == 0){
            aleatorio = -1;
        }

        //Realizamos tantas veces hasta que se vuelva a comprobar la misma frecuencia en la que empezamos.
        do{
            j += aleatorio;
            if(j < 0){
                j += dom->at(var->at(trans)).size();
            };
            j = j%dom->at(var->at(trans)).size();

            individuo->solucion.at(trans) = j;

            int a = ComprobarGrasp(var, ctr2, dom, &individuo->solucion, &pos);
            if(pen > a){
                pen = a;
                freq = j;
            }
        }while(j != iniPos);

        //Esta parte es donde insertamos la frecuencia y modificamos el colocado, es comun a las dos partes anteriores.
        individuo->solucion.at(trans) = freq;

        //Aumentamos una posicion trans sin que se salga de rango.
        trans++;
        trans = trans%individuo->solucion.size();
    }
    //Calculamos el coste de dicho individuo.
    int contNoNecesario = 0;
    individuo->coste = Comprobar(var, ctr, dom, &individuo->solucion, &contNoNecesario);
};

//Comprobamos coste solo con los transmisores que ya han sido situados.
//Comprobamos el coste del transmisor con los transmisores que ya hay colocados.
int ComprobarGrasp(vector<int> *var, vector< vector<dato> > *ctr2, vector< vector<int> > *dom, vector<int> *sol, int *trans){
    int devolver = 0;

    for(unsigned int i = 0; i < ctr2->at(*trans).size(); i++){
	//Solo comparamos penalizaciones con transmisores que sean menores que el que comprobamos y por tanto ya estarán colocados.
        int trans1 = ctr2->at(*trans).at(i).trans1;
        int trans2 = ctr2->at(*trans).at(i).trans2;
        if(sol->at(trans1) != -1 && sol->at(trans2) != -1){
            int a = dom->at(var->at(trans1)).at(sol->at(trans1));
            int b = dom->at(var->at(trans2)).at(sol->at(trans2));
            int c = a - b;
            c = abs(c);
            if(c > ctr2->at(*trans).at(i).dif){
                devolver += ctr2->at(*trans).at(i).pen;
            }
        }
    }

    return devolver;
};

//Mutamos el individuo que haya sido elegido.
void mutacion(vector<int> *var, vector <vector <int> > *dom, individuo *individuo){
    //Para cada uno de sus genes.
    for(unsigned int i = 0; i < individuo->solucion.size(); i++){
        //Obtenemos un aleatorio de 4 decimales.
        float aleatorio = rand() % 10000;
        aleatorio = aleatorio / 10000;
        //Si dicho aleatorio es menor que 0.1 (un 10%) mutamos el gen.
        if(aleatorio <= 0.1){
            individuo->solucion.at(i) = rand() % dom->at(var->at(i)).size();
        }
    }
    int contNoNecesario = 0;
    individuo->coste = Comprobar(var, &ctr, dom, &individuo->solucion, &contNoNecesario);
}

//Esta función implementa el operador de cruce en dos puntos, para lo cual se le mandan dos padres con datos diferentes, y dos hijos vacíos, a rellenar
void cruceDosPuntos(vector<int> *padre1, vector<int> *padre2, vector<int> *hijo1, vector<int> *hijo2) {
    //Limpiar los hijos.
    hijo1->clear();
    hijo2->clear();

	//Creo una variable auxiliar para el intercambio de datos
	int aux = 0;
	//Relleno los hijos, con la información de los padres
	*hijo1 = *padre1;
	*hijo2 = *padre2;
	//Intercambio la información de el segundo tercio de los vectores hijo1 e hijo2 entre si
	for (unsigned int i = hijo1->size()/3; i < (hijo1->size()/3)*2; i++)
	{
		aux = hijo1->at(i);
		hijo1->at(i) = hijo2->at(i);
		hijo2->at(i) = aux;
	}
	//Se devuelven por parámetro los hijos ya modificados y cruzados
}

//Realizamos el cruceBLX entre dos padres y obtenemos dos nuevos hijos.
void cruceBLX(vector<int> *var, vector <vector <int> > *dom, vector<int> *padre1, vector<int> *padre2, vector<int> *hijo1, vector<int> *hijo2){
    //Limpiar los hijos.
    hijo1->clear();
    hijo2->clear();

    //Variables que vamos a utilizar
    float alfa = 0.5, mayor, menor, diff;
    int freqPadre1, freqPadre2;
    //Recorremos los vectores calculando las nuevas frecuencias para los hijos.
    for(unsigned int i = 0; i < padre1->size(); i++){
        //Obtenemos las dos frecuencias de cada padre.
        freqPadre1 = dom->at(var->at(i)).at(padre1->at(i));
        freqPadre2 = dom->at(var->at(i)).at(padre2->at(i));
        //Calculamos el mayor y el menor de dichas frecuencias.
        mayor = max(freqPadre1, freqPadre2);
        menor = min(freqPadre1, freqPadre2);
        //Calculamos el valor I*alfa que nos ayudará a explorar.
        diff = (mayor - menor)*alfa;
        //Obtenemos las dos frecuencias que son el limite de nuestro rango.
        mayor = mayor + diff;
        menor = menor - diff;

        vector<int>::iterator low, up;
        //Buscamos la siguiente frecuencia que sea menor que el limite mayor del intervalo.
        up = lower_bound(dom->at(var->at(i)).begin(), dom->at(var->at(i)).end(), mayor);
        //Buscamos la siguiente frecuencia que sea mayor que el limite menor del intervalo.
        low = lower_bound(dom->at(var->at(i)).begin(), dom->at(var->at(i)).end(), menor);
        //Obtenemos las dos frecuencias.
        int posFreqMenor = (low-dom->at(var->at(i)).begin());
        int posFreqMayor;
        //Comprobamos que no sea 0.
        int pos = up-dom->at(var->at(i)).begin();
        if(pos == 0){
            posFreqMayor = (pos);
        }else{
            posFreqMayor = (pos-1);
        };

        //Obtenemos dos aleatorios entre las dos posiciones del rango.
        int aleatorio1 = 0;
        int aleatorio2 = 0;
        if(posFreqMayor == posFreqMenor){
            aleatorio1 = posFreqMenor;
            aleatorio2 = posFreqMenor;
        }else{
            aleatorio1 = posFreqMenor+(rand()%(posFreqMayor-posFreqMenor));
            aleatorio2 = posFreqMenor+(rand()%(posFreqMayor-posFreqMenor));
        }

        //Insertamos los aleatorios obtenidos en los hijos.
        hijo1->push_back(aleatorio1);
        hijo2->push_back(aleatorio2);
    }
}

int Comprobar(vector<int> *var, vector<dato> *ctr, vector< vector<int> > *dom, vector<int> *sol, int *cont){
    *cont = *cont+1;
    int devolver = 0;

    for(unsigned int i = 0; i<ctr->size(); i++){
        int trans1 = ctr->at(i).trans1;
        int trans2 = ctr->at(i).trans2;

        int a = dom->at(var->at(trans1)).at(sol->at(trans1));
        int b = dom->at(var->at(trans2)).at(sol->at(trans2));

        int c = a-b;
        c = abs(c);
        if(c > ctr->at(i).dif){
            devolver += ctr->at(i).pen;
        }
    }

    return devolver;
}

//Comprobamos la nueva penalización solamente comprobando el transmisor que ha sido modificado, transmisor y su antiguo valor.
int ComprobarFact(vector<int> *var, vector<dato> *ctr, vector< vector<dato> > *ctr2, vector< vector<int> > *dom, vector<int> *sol, int *transOldFreq, int *trans, int *valor){
    int devolver = *valor;

    //Comprobar con ctr2 que es un vector con las cnstrains de cada transmisor, ejemplo posicion 0 solo constrains
    //del 0, pero tiene datos repetidos. El ctr no tiene repetidos.

    for(unsigned int i = 0; i<ctr2->at(*trans).size(); i++){
        int trans1 = ctr2->at(*trans).at(i).trans1;
        int trans2 = ctr2->at(*trans).at(i).trans2;
        //Guardamos las dos frecuencias y comprobamos si hay o no penalizacion. Con la nueva frecuencia.
        int a = dom->at(var->at(trans1)).at(sol->at(trans1));
        int b = dom->at(var->at(trans2)).at(sol->at(trans2));
        int c = a - b;
        c = abs(c);
        if(c > ctr2->at(*trans).at(i).dif){
            devolver += ctr2->at(*trans).at(i).pen;
        }
        //Ahora tenemos que comprobar si antes habia habido penalizacion y quitarla en dicho caso.
        //Solamente cambiamos la frecuencia del transistor que hemos cambaido y volvemos a comprobar.
        if(trans1 == *trans){
            a = dom->at(var->at(trans1)).at(*transOldFreq);
        }else{
            b = dom->at(var->at(trans2)).at(*transOldFreq);
        }
        c = a - b;
        c = abs(c);
        if(c > ctr2->at(*trans).at(i).dif){
            devolver -= ctr2->at(*trans).at(i).pen;
        }
    }

    return devolver;
}

bool reiniciar(vector<individuo> *poblacion){
    //Hemos hecho una tabla hash con el modulo del tamaño de la poblacion y puede haberse cometido
    //colisiones por tanto comprobamos que todos los costes son iguales.
    vector<individuoReiniciar> contador;
    for(unsigned int i = 0; i < poblacion->size(); i++){
        individuo individuoAux = poblacion->at(i);
        bool metido = false;
        for(unsigned int j = 0; j < contador.size(); j++){
            if(individuoAux.coste == contador.at(j).coste){
                if(individuoAux.solucion == contador.at(j).solucion){
                    contador.at(j).cont++;
                    metido = true;
                    break;
                }
            }
        }
        if(!metido){
            individuoReiniciar indvR;
            indvR.solucion = individuoAux.solucion;
            indvR.coste = individuoAux.coste;
            indvR.cont = 1;
            contador.push_back(indvR);
        }
    }

    //Comprobar si hay algun cont en contador que sea mayor al 80%.
    for(unsigned int i = 0; i < contador.size(); i++){
        if(contador.at(i).cont >= TAM_POB*0.8){
            return true;
        }
    }

    return false;
}

void ordenar(vector<individuo> *poblacion){
    //Ordenamos la poblacion por costes.
    sort(poblacion->begin(), poblacion->end(), comparacion);
};

void competicion(vector<individuo> *hijos, vector<individuo> *poblacion){
    ordenar(hijos);
	int posPoblacion = poblacion->size() - 1;
	for (int i = hijos->size()-1; i >= 0; i--){
		if (hijos->at(i).coste < poblacion->at(posPoblacion).coste){
			poblacion->at(posPoblacion).solucion = hijos->at(i).solucion;
			poblacion->at(posPoblacion).coste = hijos->at(i).coste;
			posPoblacion--;
		}
	}
	ordenar(poblacion);
}

float mediaPoblacion(vector<individuo> *poblacion){
    int cont = 0;
    for(unsigned int i = 0; i < poblacion->size(); i++){
        cont += poblacion->at(i).coste;
    }
    return cont/poblacion->size();
}

vector<individuo> torneoBinarioAGG(vector<individuo> *poblacion){
    vector<individuo> padres;
    int limite = TAM_POB*0.7;

    if(limite%2 != 0){
        limite++;
    }

    for(int i = 0; i < limite; i++){
        padres.push_back(poblacion->at(rand()%poblacion->size()));
    }

    return padres;
}

vector<individuo> torneoBinarioAGE(vector<individuo> *poblacion){
    vector<individuo> padres;

    individuo padre1 = poblacion->at(rand()%poblacion->size());
    individuo padre2 = poblacion->at(rand()%poblacion->size());
    individuo padre3 = poblacion->at(rand()%poblacion->size());
    individuo padre4 = poblacion->at(rand()%poblacion->size());

    int aleatorio1 = rand()%2;
    int aleatorio2 = rand()%2;

    if(aleatorio1 == 0){
        padres.push_back(padre1);
    }else{
        padres.push_back(padre2);
    }

    if(aleatorio2 == 0){
        padres.push_back(padre3);
    }else{
        padres.push_back(padre4);
    }

    return padres;
}

void AGGDosPuntos(int *cont, vector<individuo> *poblacion){
    t1 = high_resolution_clock::now();
    int x = 0, y = 4;// repetir = 0;
    *cont = 0;
    int contComprobaciones = 50;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Generacional con dos puntos:   0% completado." << endl;
    mtx.unlock();
    while(contComprobaciones < COMPROBACIONES){
        float porc = float(contComprobaciones)/COMPROBACIONES*100;
        if(contComprobaciones%int(COMPROBACIONES*0.01)==0){
            mtx.lock();
            gotoxy(x, y);
            cout << "A.G. Generacional con dos puntos:   " << porc << "% completado." << endl;
            mtx.unlock();
        }
        individuo mejorIndividuo = poblacion->at(0);

        //int costeIni = mejorIndividuo.coste;

        vector<individuo> padres = torneoBinarioAGG(poblacion);
        vector<individuo> hijos;
        for(unsigned int i = 0; i < padres.size(); i=i+2){
            individuo hijo1, hijo2;
            cruceDosPuntos(&padres.at(i).solucion, &padres.at(i+1).solucion, &hijo1.solucion, &hijo2.solucion);
            hijo1.coste = Comprobar(&var, &ctr, &dom, &hijo1.solucion, &contComprobaciones);
            hijo2.coste = Comprobar(&var, &ctr, &dom, &hijo2.solucion, &contComprobaciones);
            hijos.push_back(hijo1);
            hijos.push_back(hijo2);
        }

        competicion(&hijos, poblacion);

        int aleatorio = rand()%poblacion->size();
        mutacion(&var, &dom, &poblacion->at(aleatorio));
        contComprobaciones++;

        if(reiniciar(poblacion)){
            inicializacion(&var, &dom, &ctr, &ctr2, poblacion, true);
            contComprobaciones += 49;
        }

        /*int costeFin = poblacion->at(0).coste;

        //Contamos para reiniciar si no se ha mejorado en 20 generaciones.
        if(costeIni == costeFin){
            repetir++;
        }else{
            repetir = 0;
        }
        //Si se llega a 20 generaciones con el mismo coste se reinicializa.
        if(repetir == 20){
            repetir = 0;
            inicializacion(&var, &dom, &ctr, &ctr2, poblacion, true);
            contComprobaciones += 49;
        }*/

        //Solo si el mejor no se encuentra en nuestra poblacion insertamos el mejor que teniamos almacenado.
        if(poblacion->at(0).coste != mejorIndividuo.coste){
            poblacion->at(TAM_POB-1) = mejorIndividuo;
            ordenar(poblacion);
        }else{
            if(poblacion->at(0).solucion != mejorIndividuo.solucion){
                poblacion->at(TAM_POB-1) = mejorIndividuo;
                ordenar(poblacion);
            }
        }

        *cont = *cont+1;
    }
    t2 = high_resolution_clock::now();
    total1 = t2-t1;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Generacional con dos puntos:   COMPLETADO.  Tiempo: " << total1.count() << endl;
    mtx.unlock();
};

void AGGBLX(int *cont, vector<individuo> *poblacion){
    t3 = high_resolution_clock::now();
    int x = 0, y = 5;// repetir = 0;
    *cont = 0;
    int contComprobaciones = 50;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Generacional con BLX:          0% completado." << endl;
    mtx.unlock();
    while(contComprobaciones < COMPROBACIONES){
        float porc = float(contComprobaciones)/COMPROBACIONES*100;
        if(contComprobaciones%int(COMPROBACIONES*0.01)==0){
            mtx.lock();
            gotoxy(x, y);
            cout << "A.G. Generacional con BLX:          " << porc << "% completado." << endl;
            mtx.unlock();
        }
        individuo mejorIndividuo = poblacion->at(0);

        //int costeIni = mejorIndividuo.coste;

        vector<individuo> padres = torneoBinarioAGG(poblacion);
        vector<individuo> hijos;
        for(unsigned int i = 0; i < padres.size(); i=i+2){
            individuo hijo1, hijo2;
            cruceBLX(&var, &dom, &padres.at(i).solucion, &padres.at(i+1).solucion, &hijo1.solucion, &hijo2.solucion);
            hijo1.coste = Comprobar(&var, &ctr, &dom, &hijo1.solucion, &contComprobaciones);
            hijo2.coste = Comprobar(&var, &ctr, &dom, &hijo2.solucion, &contComprobaciones);
            hijos.push_back(hijo1);
            hijos.push_back(hijo2);
        }

        competicion(&hijos, poblacion);

        int aleatorio = rand()%poblacion->size();
        mutacion(&var, &dom, &poblacion->at(aleatorio));
        contComprobaciones++;

        if(reiniciar(poblacion)){
            inicializacion(&var, &dom, &ctr, &ctr2, poblacion, true);
            contComprobaciones += 49;
        }

        /*int costeFin = poblacion->at(0).coste;

        //Contamos para reiniciar si no se ha mejorado en 20 generaciones.
        if(costeIni == costeFin){
            repetir++;
        }else{
            repetir = 0;
        }
        //Si se llega a 20 generaciones con el mismo coste se reinicializa.
        if(repetir == 20){
            repetir = 0;
            inicializacion(&var, &dom, &ctr, &ctr2, poblacion, true);
            contComprobaciones += 49;
        }*/

        //Solo si el mejor no se encuentra en nuestra poblacion insertamos el mejor que teniamos almacenado.
        if(poblacion->at(0).coste != mejorIndividuo.coste){
            poblacion->at(TAM_POB-1) = mejorIndividuo;
            ordenar(poblacion);
        }else{
            if(poblacion->at(0).solucion != mejorIndividuo.solucion){
                poblacion->at(TAM_POB-1) = mejorIndividuo;
                ordenar(poblacion);
            }
        }

        *cont = *cont+1;
    }
    t4 = high_resolution_clock::now();
    total2 = t4-t3;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Generacional con BLX:          COMPLETADO.  Tiempo: " << total2.count() << endl;
    mtx.unlock();
};

void AGEDosPuntos(int *cont, vector<individuo> *poblacion){
    t5 = high_resolution_clock::now();
    int x = 0, y = 6;
    *cont = 0;
    int contComprobaciones = 50;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Estacionario con dos puntos:   0% completado." << endl;
    mtx.unlock();
    while(contComprobaciones < COMPROBACIONES){
        float porc = float(contComprobaciones)/COMPROBACIONES*100;
        if(contComprobaciones%int(COMPROBACIONES*0.1)==0){
            mtx.lock();
            gotoxy(x, y);
            cout << "A.G. Estacionario con dos puntos:   " << porc << "% completado." << endl;
            mtx.unlock();
        }
        individuo mejorIndividuo = poblacion->at(0);

        vector<individuo> padres = torneoBinarioAGE(poblacion);
        vector<individuo> hijos;
        for(unsigned int i = 0; i < padres.size(); i=i+2){
            individuo hijo1, hijo2;
            cruceDosPuntos(&padres.at(i).solucion, &padres.at(i+1).solucion, &hijo1.solucion, &hijo2.solucion);
            float aleatorio1 = (rand()%1000)/1000;
            float aleatorio2 = (rand()%1000)/1000;
            if(aleatorio1 <= 0.02){
                mutacion(&var, &dom, &hijo1);
                contComprobaciones++;
            }else{
                hijo1.coste = Comprobar(&var, &ctr, &dom, &hijo1.solucion, &contComprobaciones);
            }
            if(aleatorio2 <= 0.02){
                mutacion(&var, &dom, &hijo2);
                contComprobaciones++;
            }else{
                hijo2.coste = Comprobar(&var, &ctr, &dom, &hijo2.solucion, &contComprobaciones);
            }
            hijos.push_back(hijo1);
            hijos.push_back(hijo2);
        }

        competicion(&hijos, poblacion);

        if(reiniciar(poblacion)){
            inicializacion(&var, &dom, &ctr, &ctr2, poblacion, true);
            contComprobaciones += 49;
        }

        *cont = *cont+1;
    }
    t6 = high_resolution_clock::now();
    total3 = t6-t5;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Estacionario con dos puntos:   COMPLETADO.  Tiempo: " << total3.count() << endl;
    mtx.unlock();
};

void AGEBLX(int *cont, vector<individuo> *poblacion){
    t7 = high_resolution_clock::now();
    int x = 0, y = 7;
    *cont = 0;
    int contComprobaciones = 50;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Estacionario con BLX:          0% completado." << endl;
    mtx.unlock();
    while(contComprobaciones < COMPROBACIONES){
        float porc = float(contComprobaciones)/COMPROBACIONES*100;
        if(contComprobaciones%int(COMPROBACIONES*0.1)==0){
            mtx.lock();
            gotoxy(x, y);
            cout << "A.G. Estacionario con BLX:          " << porc << "% completado." << endl;
            mtx.unlock();
        }
        individuo mejorIndividuo = poblacion->at(0);

        vector<individuo> padres = torneoBinarioAGE(poblacion);
        vector<individuo> hijos;
        for(unsigned int i = 0; i < padres.size(); i=i+2){
            individuo hijo1, hijo2;
            cruceBLX(&var, &dom, &padres.at(i).solucion, &padres.at(i+1).solucion, &hijo1.solucion, &hijo2.solucion);
            float aleatorio1 = (rand()%1000)/1000;
            float aleatorio2 = (rand()%1000)/1000;
            if(aleatorio1 <= 0.02){
                mutacion(&var, &dom, &hijo1);
                contComprobaciones++;
            }else{
                hijo1.coste = Comprobar(&var, &ctr, &dom, &hijo1.solucion, &contComprobaciones);
            }
            if(aleatorio2 <= 0.02){
                mutacion(&var, &dom, &hijo2);
                contComprobaciones++;
            }else{
                hijo2.coste = Comprobar(&var, &ctr, &dom, &hijo2.solucion, &contComprobaciones);
            }
            hijos.push_back(hijo1);
            hijos.push_back(hijo2);
        }

        competicion(&hijos, poblacion);

        if(reiniciar(poblacion)){
            inicializacion(&var, &dom, &ctr, &ctr2, poblacion, true);
            contComprobaciones += 49;
        }

        *cont = *cont+1;
    }
    t8 = high_resolution_clock::now();
    total4 = t8-t7;
    mtx.lock();
    gotoxy(x, y);
    cout << "A.G. Estacionario con BLX:          COMPLETADO.  Tiempo: " << total4.count() << endl;
    mtx.unlock();
};
