#include<iostream>;

using namespace std;

class gato{
    
    //Atributos
    private:
        string nombre;
        int edad;
        string color;

    public:
    gato(string n, int e, string c){
        nombre = n ;
        edad = e;
        color = c;

    }
    //Metodos
    void maullar(){
        cout<<nombre<< ": Dice Miauuuuuuu"<<endl;
    }

    void comer(){
        cout<<nombre<<": Esta comiendo..."<<endl;
    }

    void  mostrarInformacion(){
        cout<<"Nombre: "<< nombre <<endl;
        cout<<"Edad: "<< edad <<endl;
        cout<<"Color: "<< color <<endl;
    }

};

int main(){

    gato gato1("Pichi", 5, "Negro");
    gato gato2("Mandarino", 3, "Naranjoso");

    cout<<"Gato 1: " << endl;
    gato1.mostrarInformacion();
    gato1.maullar();
    gato1.comer();

    cout << endl;

    cout<<"Gato 2: " <<endl;
    gato2.mostrarInformacion();
    gato2.maullar();
    gato2.comer();



    return 0;
}