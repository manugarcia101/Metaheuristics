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

//#include "random.h"

using namespace std;
using namespace chrono;

struct dato{
    int trans1;
    int trans2;
    int dif;
    int pen;
};

struct transFreq{
    int trans;
    int freq;
};

struct tipo{
    int freq;
    int cont = 1;
};

struct datoGrasp{
    int trans;
    int freq;
    bool colocado = false;
    bool elegido = false;
};

struct datoCoste{
    int trans;
    int coste;
    int pos;
    double sesgo;
};

void loadVar(string *name, vector <int> *v, vector<int> *t);
void loadDom(string *name, vector <vector <int> > *v);
void loadCtr(string *name, vector <dato> *w, vector < vector <dato> > *v, vector<int> *t);

void Greedy(vector<int> *var, vector <vector <int> > *dom, vector<int> *sol);

void BusquedaLocal(vector<int> *var, vector <dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol, int *iteraciones);

void BusquedaTabu(vector<int> *var, vector <dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol);
void insertarTabu(int *trans, int *pos, vector<transFreq> *listaTabu, int *tam, int *i);
bool esTabu(int *trans, int *freqPos, vector<transFreq> *listaTabu);
void insertarMatrix(int *trans, int *freq, vector< vector<tipo> > *matrix);
void reinicializar(vector<int> *sol, vector< vector<tipo> > *matrix);

void grasp(vector<int> *var, vector <dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol);
void insertarAleatorios(vector<datoGrasp> *solAux, vector<int> *var, vector <vector <int> > *dom, int *aleatorios);
void insertarBusquedaLocal(vector<datoGrasp> *solAux, vector<int> *var, vector <vector <int> > *dom, vector< vector<dato> > *ctr2);
int ComprobarGrasp(vector<int> *var, vector< vector<dato> > *ctr2, vector< vector<int> > *dom, vector<datoGrasp> *solAux, int *trans);
void ordenarMenorMayor(vector<datoCoste> *costes);
bool comparar(const datoCoste &a, const datoCoste &b){
    if(a.coste < b.coste){
        return true;
    }else{
        return false;
    }
};

int Comprobar(vector<int> *var, vector<dato> *ctr, vector <vector <int> > *dom, vector<int> *sol);
int ComprobarFact(vector<int> *var,  vector<dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol, int *transOldFreq, int *trans, int *valor);

int main()
{
    unsigned int iteration = -1;

    int cont = 0;
    int a;
    vector<int> semilla = {77381751, 73817517, 38175177, 84109773, 41097738};
    vector<int> semillaAux;
    cout << "Introduce un numero para la semilla." << endl <<
    "Al introducir 0 al principio se insertan las semillas 77381751, 73817517, 38175177, 84109773 y 41097738 por defecto. " << endl <<
    "(Introduce 0 para terminar)" << endl;
    do{
        cout << "Semilla " << ++cont << ": ";
        cin >> a;
        if(a != 0){
            semilla.clear();
            semillaAux.push_back(a);
        };
        //Al pulsar enter al introducir 0 se activa la tecla al final del main por tanto limpiamos el buffer de teclas.
        getchar();
    }while(a != 0);

    for(unsigned int i = 0; i < semillaAux.size(); i++){
        semilla.push_back(semillaAux.at(i));
    }

    if(semilla.size() == 0){
        return 0;
    }

    system("cls");

    do{

        iteration++;

        srand(semilla[iteration]);

        cout << "SEMILLA: " << semilla[iteration] << " ." << endl << endl;

        int tam = 11;
        string name[tam] = {"scen06", "scen07", "scen08", "scen09", "scen10", "graph05", "graph06", "graph07", "graph11", "graph12", "graph13"};

        for(int i = 0; i < tam; i++){
            cout << "EJECUCION CON " << name[i] << "." << endl;

            high_resolution_clock::time_point t1 = high_resolution_clock::now();
            vector<int> traducciones;

            vector<int> var;
            loadVar(&name[i], &var, &traducciones);

            vector<vector<int> > dom;
            loadDom(&name[i], &dom);

            vector<dato> ctr;
            vector< vector<dato> > ctr2;
            ctr2.resize(var.size());
            loadCtr(&name[i], &ctr, &ctr2, &traducciones);
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> total = t2-t1;
            cout << "TIEMPO LEYENDO FICHEROS: " << total.count() << " segundos." << endl;

            cout << "NUMERO DE TRANSMISORES: " << var.size() << " Y " << ctr.size() << " CONSTRAINS." << endl;

            vector<int> sol;
            t1 = high_resolution_clock::now();
            Greedy(&var, &dom, &sol);
            t2 = high_resolution_clock::now();
            total = t2-t1;
            cout << "GREEDY         -> Penalizacion: " << Comprobar(&var, &ctr, &dom, &sol) << "; Tiempo: " << total.count() << " segundos." << endl;

            vector<int> solLocal = sol;
            t1 = high_resolution_clock::now();
            int iteraciones = 10000;
            BusquedaLocal(&var, &ctr, &ctr2, &dom, &solLocal, &iteraciones);
            t2 = high_resolution_clock::now();
            total = t2-t1;
            cout << "BUSQUEDA LOCAL -> Penalizacion: " << Comprobar(&var, &ctr, &dom, &solLocal) << "; Tiempo: " << total.count() << " segundos."  << endl;

            vector<int> solTabu = sol;
            t1 = high_resolution_clock::now();
            BusquedaTabu(&var, &ctr, &ctr2, &dom, &solTabu);
            t2 = high_resolution_clock::now();
            total = t2-t1;
            cout << "BUSQUEDA TABU  -> Penalizacion: " << Comprobar(&var, &ctr, &dom, &solTabu) << "; Tiempo: " << total.count() << " segundos." << endl;

            vector<int> solGrasp;
            t1 = high_resolution_clock::now();
            grasp(&var, &ctr, &ctr2, &dom, &solGrasp);
            t2 = high_resolution_clock::now();
            total = t2-t1;
            cout << "GRASP          -> Penalizacion: " << Comprobar(&var, &ctr, &dom, &solGrasp) << "; Tiempo: " << total.count() << " segundos." << endl;
            cout << endl;
        }

        if(iteration < semilla.size()-1){
            int tecla;
            cout << "Pulsa ENTER para la siguiente ejecucion con una semilla diferente.";
            do{
                tecla = getchar();
            }while(tecla != 10);
            system("cls");
        }else{
            cout << "FIN DEL PROGRAMA.";
        }

    }while(iteration < semilla.size()-1);

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

void Greedy(vector<int> *var, vector <vector <int> > *dom, vector<int> *sol){
    sol->clear();
    for(unsigned int i = 0; i<var->size(); i++){
        sol->push_back(rand()%(dom->at(var->at(i)).size()));
    }
}

//Se elige un transmisor al azar, se elige la direccion de la nueva frecuencia al azar, miras todas las frecuencias del transmisor elegido,
//copruebas con compruebaFact y se hace por primero mejor y lo hacemos el numero de iteraciones
void BusquedaLocal(vector<int> *var, vector<dato> *ctr, vector < vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol, int *iteraciones){
    //Creas un vector con la solucion, que almacena la solucion Greedy ya almacenada. Compruebas la penalización.
    vector<int> solAux = *sol;
    int penalizacion = Comprobar(var, ctr, dom, &solAux);
    for(int i = 0; i < *iteraciones; i++){
        //Transmisor aleatorio que modificamos.
        int transistor = rand()%var->size();
        //Aleatorio para comprobar vecinos superiores o inferiores.
        int aleatorio = rand()%2;
        if(aleatorio == 0){
            aleatorio = -1;
        }
        //Guardamos la antigua frecuencia que tenia el transistor.
        int transOldFreq = solAux.at(transistor);
        //Sumamos una posicion sin salirnos del rango de frecuencias posibles.
        solAux.at(transistor) += aleatorio;
        if(solAux.at(transistor) < 0){
            solAux.at(transistor) += dom->at(var->at(transistor)).size();
        };
        solAux.at(transistor) = solAux.at(transistor)%dom->at(var->at(transistor)).size();
        //Variable aux, para obtener el mejor.
        int penalizacionNew = INT_MAX;
        //Iteramos mientras que no se mejor o estemos en la posicion de inicio, porque hacemos un bucle.
        while(transOldFreq != solAux.at(transistor) && penalizacion <= penalizacionNew){
            //CAmbiamos la penalizacion a la nueva de haber cambiado la frecuencia a un transmisor.
            penalizacionNew = ComprobarFact(var, ctr, ctr2, dom, &solAux, &transOldFreq, &transistor, &penalizacion);
            //Volvemos a cambiar la frecuencia del transmisor sin salirnos del rango.
            solAux.at(transistor) += aleatorio;
            if(solAux.at(transistor) < 0){
                solAux.at(transistor) += dom->at(var->at(transistor)).size();
            };
            solAux.at(transistor) = solAux.at(transistor)%dom->at(var->at(transistor)).size();
            //Añadios una iteracion porque ya hemos comprobado una solucion.
            i++;
        }
        //Se cambia el vector solucion por la nueva solucion si la nueva penalizacion es menor que la antigua.
        if(penalizacionNew < penalizacion){
            *sol = solAux;
        }
    }
}

void BusquedaTabu(vector<int> *var, vector <dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol){
    //solTotal almacena la mejor solucion de todas y su penalizacion es penalizacion Total.
    vector<int> solTotal = *sol;
    //solAux es la mejor solucion local de los 20 vecinos que comprobamos.
    vector<int> solAux = *sol;
    int penalizacion = Comprobar(var, ctr, dom, &solAux);
    int penalizacionTotal = penalizacion;

    //Matrix que guardamos el transmisor y sus frecuencias y el numero que las usamos, para reinicializar.
    vector< vector<tipo> > matrix;
    matrix.resize(var->size());

    //Insertamos en la matriz los datos que se han insertado ern el Greedy.
    //Cont se usa para acceder a los transmisores, ya que i no se puede pasar por parametro al tener distinto tipo.
    int cont;
    for(unsigned int i = 0; i < solAux.size(); i++){
        cont = i;
        insertarMatrix(&cont, &solAux.at(i), &matrix);
    }

    //Declaramos la lista tabu de tipo transFreq que almacena el transmisor y la frecuencia.
    vector<transFreq> listaTabu;
    //Tamaño de la lista tabu.
    int tam = ceil(var->size()/6);
    listaTabu.resize(tam);

    //Variable para comprobar si reinicializamos.
    int reinicio = 0;

    //Variable que contiene la posicion donde insertamos el siguiente dato tabu.
    int k = 0;

    //Comprobamos si hay que reinicializar, y si elegimos una frecuencia se inserta en lista tabu.
    for(int i = 0; i<10000; i++){
        //Si llegamos a 2000 iteraciones sin mejorar reuinicializamos con la matriz.
        if(reinicio == 2000){
            reinicializar(&solAux, &matrix);
            reinicio = 0;
        };
        //Buscamos un transmisor aleatorio y un numero aleatorio para saber si comprobamos vecinos superiores o inferiores.
        int trans = rand()%var->size();
        int pos = solAux.at(trans);
        //Guardamos la frecuencia inicial.
        int transOldFreq = solAux.at(trans);
        int aleatorio = rand()%2;
        if(aleatorio == 0){
            aleatorio = -1;
        }

        //Penalizaciones para comprobar cosas.
        int pen = INT_MAX;
        int penAux = INT_MAX;
        int freqBest;
        //Comprobamos de los 20 vecinos cual es el mejor.
        for(int j= 0; j < 20; j++){
            //Sumamos una posicion a la frecuencia y corregimos par que no se nos salga del rango.
            pos += aleatorio;
            if(pos < 0){
                pos += dom->at(var->at(trans)).size();
            };
            pos = pos%dom->at(var->at(trans)).size();
            //Guardamos la nueva frecuencia encontrada.
            solAux.at(trans) = pos;
            //Comprobamos su penalizacion.
            penAux = ComprobarFact(var, ctr, ctr2, dom, &solAux, &transOldFreq, &trans, &penalizacion);
            //Comprobamos si es tabu. Si es tabu, comprobamos si es mejor que la mejor solucion actual.
            if(esTabu(&trans, &pos, &listaTabu)){
                if(penAux < penalizacionTotal){
                    pen = penAux;
                    freqBest = pos;
                    reinicio = 0;
                }
            //Si no es tabu, simplemente comprobamos si es mejor que la mejor solucion de los vecinos que llevamos.
            }else if(penAux < pen){
                pen = penAux;
                freqBest = pos;
                reinicio = 0;
            }
            //Sumamos una iteracion porque hemos comprobado un vecino.
            i++;
            //Si no se ha mejorado se añadiria una iteracion al reinicio.
            reinicio++;
        }
        //Cuando hemos comprobado los vecinos guardamos la frecuencia mejor y la penalizacion local.
        solAux.at(trans) = freqBest;
        penalizacion = pen;
        //Insertamos en la lista tabu cuando lo hemos elegido.
        insertarTabu(&trans, &freqBest, &listaTabu, &tam, &k);
        //Insertamos en la matriz que usamos para reinicializar.
        insertarMatrix(&trans, &freqBest, &matrix);
        //Y si fuera mejor que la solucionTotal se modifica.
        if(penalizacionTotal > penalizacion){
            solTotal.at(trans) = solAux.at(trans);
            penalizacionTotal = penalizacion;
        }
        //Si no mejoramos se añade una iteracion a reinicio.
        reinicio++;
    }
    //Igualamos la solucion que queremos a la que hemos encontrado.
    *sol = solTotal;
}

void grasp(vector<int> *var, vector <dato> *ctr, vector< vector<dato> > *ctr2, vector <vector <int> > *dom, vector<int> *sol){
    //Solucion donde se almaenan solo los datos que han logrado pasar el concurso.
    vector<datoGrasp> solTotal;
    solTotal.resize(var->size());

    //Numero de aleatorios que insertamos por defectoal iniciar el concurso.
    int aleatorios = 10;

    //Variable para comprobar los que hemos insertado.
    int insertados = 0;

    for(unsigned int i = 0; i < solTotal.size(); i++){
        //Si quedan 10 huecos a insertar, insertamos con busqueda local y nos salimos.
        int tam = solTotal.size();
        if(tam-aleatorios == insertados){
            insertarBusquedaLocal(&solTotal, var, dom, ctr2);
            break;
        }

        //Vector de soluciones de tipo datoGrasp, con la frecuencia, si esta colocado y si es elegido(se encuentra en solTotal)
        vector<datoGrasp> solAux;
        solAux.resize(var->size());

        //Rellenamos el vector solAux dependiendo de los que ya han pasado el "concurso".
        solAux = solTotal;

        insertarAleatorios(&solAux, var, dom, &aleatorios);

        insertarBusquedaLocal(&solAux, var, dom, ctr2);

        int pos;
        vector<datoCoste> costes;
        for(unsigned int i = 0; i < solAux.size(); i++){
            pos = i;
            datoCoste a;
            a.trans = i;
            if(solAux.at(i).elegido == true){
                a.coste = INT_MAX;
            }else{
                a.coste = ComprobarGrasp(var, ctr2, dom, &solAux, &pos);
            }
            a.pos = 0;
            a.sesgo = 0;
            costes.push_back(a);
        }

        //Ordena de menor a mayor segun los costes y calcula el sesgo.
        ordenarMenorMayor(&costes);

        //Obtenemos un aleatorio de 3 decimales.
        float aleatorio = (rand()%1000);
        aleatorio = aleatorio / 1000;

        //Comprobamos con dicho aleatorio que transmisor escogemos dependiendo del sesgo.
        double totalSum = 0;
        int trans;
        for(unsigned int i = 0; i < costes.size(); i++){
            totalSum += costes.at(i).sesgo;
            if(totalSum >= aleatorio){
                trans = costes.at(i).trans;
                break;
            }
        }

        solTotal.at(trans).freq = solAux.at(trans).freq;
        solTotal.at(trans).colocado = true;
        solTotal.at(trans).elegido = true;
    }

    //Copiamos los datos en el vector input.
    for(unsigned int i = 0; i < solTotal.size(); i++){
        sol->push_back(solTotal.at(i).freq);
    }

    int iteraciones = 400;
    BusquedaLocal(var, ctr, ctr2, dom, sol, &iteraciones);

}

void insertarAleatorios(vector<datoGrasp> *solAux, vector<int> *var, vector <vector <int> > *dom, int *aleatorios){
    vector<int> libres;
    //Comprobamos los que podemos elegir aleatoriamente y los insertamos en el vector.
    for(unsigned int i = 0; i < solAux->size(); i++){
        if(solAux->at(i).colocado == false){
            libres.push_back(i);
        }
    }

    int pos;
    int tam = libres.size();
    //Solo si hay huecos libres insertamos los n aleatorios.
    if(tam >= *aleatorios){
        //Una vez elegidos los disponibles elegimos al azar n de ellos.
        for(int i = 0; i < *aleatorios; i++){
            pos = rand()%libres.size();
            libres.erase(libres.begin()+pos);
            solAux->at(pos).freq = rand()%dom->at(var->at(pos)).size();
            solAux->at(pos).colocado = true;
        }
    }
};

//Si el ocste (que pasamos por atributo) es igual al que obtenemos paramos.
void insertarBusquedaLocal(vector<datoGrasp> *solAux, vector<int> *var, vector <vector <int> > *dom, vector< vector<dato> > *ctr2){
    for(unsigned int i = 0; i < solAux->size(); i++){
        if(solAux->at(i).colocado != true){
            int pos = i;
            int pen = INT_MAX;
            int freq;

            //Cogemos la primera frecuencia y apartir de ahi comprobamos con las siguientes en orden ascendente.
            /*
            for(unsigned int j = 0; j < dom->at(var->at(i)).size(); j++){
                solAux->at(i).freq = j;
                int a = ComprobarGrasp(var, ctr2, dom, solAux, &pos);
                if(pen > a){
                    pen = a;
                    freq = j;
                }
            }
            */

            //Escogemos una fecuencia aleatoria y comprobamos de manera aleatoria la siguiente si orden ascendente o descendente.

            int j = rand()%dom->at(var->at(i)).size();
            int iniPos = j;
            int aleatorio = rand()%2;
            if(aleatorio == 0){
                aleatorio = -1;
            }
            int iteracion = 0;
            //Numero de soluciones vecinas que comprobamos.
            int itMaxima = 20;
            do{
                iteracion++;
                j += aleatorio;
                if(j < 0){
                    j += dom->at(var->at(i)).size();
                };
                j = j%dom->at(var->at(i)).size();

                solAux->at(i).freq = j;
                int a = ComprobarGrasp(var, ctr2, dom, solAux, &pos);
                if(pen > a){
                    pen = a;
                    freq = j;
                }
            }while(j != iniPos && iteracion != itMaxima);

            //Esta parte es donde insertamos la frecuencia y modificamos el colocado, es comun a las dos partes anteriores.
            solAux->at(i).freq = freq;
            solAux->at(i).colocado = true;
        }
    }
};

int ComprobarGrasp(vector<int> *var, vector< vector<dato> > *ctr2, vector< vector<int> > *dom, vector<datoGrasp> *solAux, int *trans){
    int devolver = 0;

    for(unsigned int i = 0; i < ctr2->at(*trans).size(); i++){
        int trans1 = ctr2->at(*trans).at(i).trans1;
        int trans2 = ctr2->at(*trans).at(i).trans2;
        if(solAux->at(trans1).colocado == true && solAux->at(trans2).colocado == true){
            int a = dom->at(var->at(trans1)).at(solAux->at(trans1).freq);
            int b = dom->at(var->at(trans2)).at(solAux->at(trans2).freq);
            int c = a - b;
            c = abs(c);
            if(c > ctr2->at(*trans).at(i).dif){
                devolver += ctr2->at(*trans).at(i).pen;
            }
        }
    }

    return devolver;
};

void ordenarMenorMayor(vector<datoCoste> *costes){
    //Ordena el vector dde menor a mayor segun el coste, y con la ayuda de la funcion comparar.
    sort(costes->begin(), costes->end(), comparar);

    //Colocamos el dato de la posicion para clacular la probabilidad de dicho dato.
    int coste = INT_MAX;
    double totalSum = 0;
    int pos = 1;
    for(unsigned int i = 0; i < costes->size(); i++){
        //Comprobamos si el coste actual es igual o no que el coste anterior y dependiendo de eso le ponemos la misma posicion o no.
        if(costes->at(i).coste == coste){
            coste = costes->at(i).coste;
            costes->at(i).pos = pos;
        }else{
            coste = costes->at(i).coste;
            pos = i+1;
            costes->at(i).pos = pos;
        }
        //Si no es un transmisor ya elegido sumamos para calcular el denominador que usaremos para calcular el sesgo.
        double a = costes->at(i).pos;
        int b = costes->at(i).pos;
        if(b != INT_MAX){
            totalSum += 1/a;
        }else{
            break;
        }
    }

    //Calculamos el sesgo para cada transmisor.
    for(unsigned int i = 0; i < costes->size(); i++){
        double pos = costes->at(i).pos;
        int b = costes->at(i).pos;
        if(b != INT_MAX){
            costes->at(i).sesgo = (1/pos)/(totalSum);
        }else{
            break;
        }
    }
};











void insertarTabu(int *trans, int *pos, vector<transFreq> *listaTabu, int *tam, int *i){
    //Cramos una variable de tipo transFreq.
    transFreq a;
    a.trans = *trans;
    a.freq = *pos;
    //Insertamos en la posicion i que es la variable k en la funcion, nos da igual machacar datos
    listaTabu->at(*i) = a;
    //Aumentamos i sin salirnos del tamaño de la lista.
    *i = (*i+1)%*tam;
}

bool esTabu(int *trans, int *freqPos, vector<transFreq> *listaTabu){
    //Recorre la lista y comprueba si se encuentra o no en ella dicha frecuencia.
    for(unsigned int i = 0; i < listaTabu->size(); i++){
        if(listaTabu->at(i).trans == *trans && listaTabu->at(i).freq == *freqPos){
            return true;
        }
    }
    return false;
}

void insertarMatrix(int *trans, int *freq, vector< vector<tipo> > *matrix){
    bool encontrado = false;
    //Buscamos si ya se encuentra la frecuencia para dicho transmisor y sumamos 1 al contador.
    for(unsigned int i = 0; i < matrix->at(*trans).size(); i++){
        if(matrix->at(*trans).at(i).freq == *freq){
            matrix->at(*trans).at(i).cont++;
            encontrado = true;
        }
    }
    //Si no se ha encontrado insertamos la nueva frecuencia.
    if(!encontrado){
        tipo a;
        a.freq = *freq;
        a.cont = 1;
        matrix->at(*trans).push_back(a);
    }
}

void reinicializar(vector<int> *sol, vector< vector<tipo> > *matrix){
    //Buscamos para cada transmisor.
    for(unsigned int i = 0; i < matrix->size(); i++){
        int contMax = INT_MIN;
        int freq;
        //Buscamos la mayor ocurrencia de una frecuencia para dicho transmisor y la guardamos en la solucion.
        for(unsigned int j = 0; j < matrix->at(i).size(); j++){
            if(matrix->at(i).at(j).cont > contMax){
                contMax = matrix->at(i).at(j).cont;
                freq = matrix->at(i).at(j).freq;
            }
        }
        sol->at(i) = freq;
    }
}

int Comprobar(vector<int> *var, vector<dato> *ctr, vector< vector<int> > *dom, vector<int> *sol){
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

int ComprobarFact(vector<int> *var, vector<dato> *ctr, vector< vector<dato> > *ctr2, vector< vector<int> > *dom, vector<int> *sol, int *transOldFreq, int *trans, int *valor){
    int devolver = *valor;

    //Comprobar con el ctr que es un vector con las constrains.

    /*for(unsigned int i = 0; i<ctr->size(); i++){
        if(*trans == ctr->at(i).trans1 || *trans == ctr->at(i).trans2){
            int trans1 = ctr->at(i).trans1;
            int trans2 = ctr->at(i).trans2;

            //Guardamos las dos frecuencias y comprobamos si hay o no penalizacion. Con la nueva frecuencia.
            int a = dom->at(var->at(trans1)).at(sol->at(trans1));
            int b = dom->at(var->at(trans2)).at(sol->at(trans2));
            int c = a - b;
            c = abs(c);
            if(c > ctr->at(i).dif){
                devolver += ctr->at(i).pen;
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
            if(c > ctr->at(i).dif){
                devolver -= ctr->at(i).pen;
            }
        }
    }*/

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
