#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace std;

int main()
{
    ifstream arq("teste.txt");
    string linha;
    
    /*
	arq >> linha;
    cout << "Linha:" << linha << "Borda" << endl;
    arq >> linha;
    cout << "Linha:" << linha << "Borda" << endl;
    
    getline(arq, linha);    
    cout << "linha:" << linha << "Borda" << endl;
    */
    
    getline(arq, linha,';');
    cout << "linha:" << linha << "Borda" << typeid(linha).name() << endl;
    arq >> ws;
    getline(arq, linha, ';');
    cout << "linha:" << linha << "Borda" << endl;
    getline(arq, linha);
    
    cout << "linha:" << linha << "Borda" << endl;

    return 0;
}
