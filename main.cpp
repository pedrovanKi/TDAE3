#include <bits/stdc++.h>
using namespace std;

/*
 * Esta implementación está basada en la función recursiva maxAcorns. Para un arbol i y una altura j:
 *  ---> maxAcorns(i,j) = cantidad de bellotas que se obtienen en el mejor camino partiendo del arbol i a la altura j.
 *  ---> #acorns(i,j) = cantidad de bellotas en el arbol i exactamente a la altura j.
 * Sea t la cantidad de arboles que hay en el sistema.
 * Sea f la altura que se desciende para saltar del arbol i a cualquier arbol k distinto a i.
 *
 *     maxAcorns(i,j) = | Σ{#acorns(i,k): 0<k<=j}                                           si j<f  (*)
 *                      | #acorns(i,j) + max{maxAcorns(i,j-1)}U{A(k,j-f): 0<=k<t && k!=i}   en otro caso.
 *
 * (*) Si j es menor a f, si saltamos no llegamos a otro arbol antes de chocar contra el piso, sin agarrar ninguna bellota más. Por lo que
 * si recorremos el resto del arbol hasta el suelo, la cantidad de bellotas que agarremos será mayor (si hay por lo menos una) o igual (si no hay más) que saltar.
 *
 * Sea h la altura de todos los t arboles:
 * El problema se resuelve calculando max{maxAcorns(i,h) paraTodo i 0<=i<t}.

Sample input:
1
3 10 2
3 1 4 10
6 3 5 7 8 9 9
5 3 4 5 6 9
0

 */



struct arbol{
public:
    arbol(int a): cant_bellotas(a),ubi_bellotas({}){};

    int cant_bellotas; // cantidad total de bellotas en el arbol
    map<int,int> ubi_bellotas; // ubicación de cada bellota (recibe una altura y si hay bellotas alli devuelve la cantidad que hay)

    int bellotasEnAltura(int h) const; // devuelve la cantidad de bellotas en la altura h.
};

int arbol::bellotasEnAltura(int h) const{
    if (ubi_bellotas.find(h) == ubi_bellotas.end()){
        return 0;
    } else {
        return ubi_bellotas.at(h);
    }
}



// Dado un "bosque" v_arboles, empezando en el arbol = i, a la altura = j, con cierta altura perdida por los saltos = vuelo,
// se encuentra recursivamente la maxima cantidad de bellotas que se pueden conseguir.
int dinamicAcorns(const vector<arbol>& v_arboles, const int i, const int j, const int vuelo, vector<vector<int>>& Mem){

    // El caso base de la función es cuando j<vuelo. En este caso, la respuesta será la suma de todas las bellotas en alturas menores a j.
    // Igualmente conviene memorizar los valores que vamos encontrando en las distintas alturas para minimizar las búsquedas en los maps.
    // Por lo que lo que haremoes es utilizar el caso base j==1. (haremos recursión en el resto de casos).

    if (Mem[i][j]==-1){
        // Si no tenemos memorizado la solucion del subproblema, lo resolvemos y memorizamos.

        if (j==0) {
            // Si tenemos el caso base j==1, lo resolvemos directamente y memorizamos:
            Mem[i][j] = v_arboles[i].bellotasEnAltura(j);

        } else {
            // En otro caso, lo resolvemos recursivamente. Aquí tenemos otros dos casos:
            if (j < vuelo-1) {
                // Si el único camino es descender por el árbol sin saltos:

                // Hacemos recursión en el mismo árbol un nivel de altura menor (para memorizar todos los niveles)
                Mem[i][j] = v_arboles[i].bellotasEnAltura(j) + dinamicAcorns(v_arboles, i, j - 1, vuelo, Mem);
            } else {
                // Si podemos realizar saltos, debemos comparar el caso donde se sigue por el mismo arbol y los t-1 casos
                // donde se salto a algún otro árbol. Miramos todos y encontramos el máximo de ellos:

                int maxAcorns = dinamicAcorns(v_arboles, i, j - 1, vuelo,Mem); // iniciamos con el caso donde seguimos por el arbol i.
                // Luego lo comparamos con el resto de casos.
                for (int k = 0; k < v_arboles.size(); ++k) {
                    if (k == i) continue; // Para no comparar el caso inutil donde hacemos un salto al mismo arbol perdiendo altura.

                    // calculamos recursivamente el caso donde saltamos al arbol k. Es importante que luego del salto, partimos de la altura j-vuelo.
                    int candidato = dinamicAcorns(v_arboles, k, j - vuelo, vuelo, Mem);

                    if (maxAcorns < candidato) maxAcorns = candidato; // Si encontramos un caso mejor lo sustituímos.
                }
                // Conseguimos el maxAcorns, entonces memorizamos la solución del subproblema.
                Mem[i][j] = v_arboles[i].bellotasEnAltura(j) + maxAcorns;
            }
        }
    }

    // Devolvemos el valor memorizado, tanto si ya lo teníamos calculado, como si lo acabamos de calcular.
    return Mem[i][j];
}

int main() {
    int casos; cin >> casos; // Cantidad de datasets que serán computados

    for(int c = 1; c <= casos; c++){
        int cant_arboles; cin >> cant_arboles; // Cantidad de árboles del caso (t).
        int altura; cin >> altura; // Altura de cada árbol (h).
        int vuelo; cin >> vuelo; // Altura que se pierde en un salto de un árbol a otro (f).

        // Armamos el vector de arboles, tomando el input para cada uno de los t arboles.
        vector<arbol> v_arboles = {};

        for (int i = 0; i < cant_arboles; ++i) {
            int cant_bellotas; cin >> cant_bellotas; // Cantidad de bellotas del arbol i.
            arbol arbol_i(cant_bellotas); // Creamos un el i-ésimo arbol que tiene cant_bellotas de bellotas.

            // Tomamos la altura de la k-ésima bellota y la definimos en el map del arbol i.
            for (int k = 0; k < cant_bellotas; ++k) {
                int altura_bellota_k; cin >> altura_bellota_k;

                // Buscamos si la altura de la k-ésima bellota ya fue definida:
                if(arbol_i.ubi_bellotas.find(altura_bellota_k) == arbol_i.ubi_bellotas.end()) {//(si la altura no está definida, map.find(...) nos devuelve el iterador map.end())
                    // Si no lo fue, la definimos:
                    arbol_i.ubi_bellotas[altura_bellota_k] = 1;
                } else {
                    // Si ya fue definida, debemos sumar una bellota más a las que ya hay:
                    arbol_i.ubi_bellotas[altura_bellota_k]++;
                }
            }

            // Una vez definido por completo el i-ésimo árbol, lo insertamos a nuestro vector de árboles en la i-ésima posición.
            v_arboles.push_back(arbol_i);
        }

        // Una vez tenemos el vector de arboles, encontraremos la cantidad de bellotas del mejor camino encontrando el máximo de bellotas que se consiguen
        // empezando desde los distintos t arboles. Entonces, vamos resolviendop para cada arbol de inicio y comparamos los resultados.

        // Usaremos una estructura de memoria para guardar la respuesta de los subproblemas. Mem[i][j] = maxAcorns(i,j).
        vector<vector<int>> Mem(cant_arboles,vector<int>(altura,-1));


        int maxAcorns = 0;
        for (int i = 0; i < cant_arboles; ++i) {
            int candidato = dinamicAcorns(v_arboles,i,altura-1,vuelo,Mem);
            if (maxAcorns < candidato) maxAcorns = candidato;
        }

        // Una vez encontrado el mejor maxAcorns, lo imprimimos y pasamos al siguiente caso.
        cout << "\n" << maxAcorns;

    }
    return 0;
}
