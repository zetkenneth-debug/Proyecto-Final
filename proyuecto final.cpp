#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <limits>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

using namespace std;

// ==================== Clase Account ====================
class Account {
private:
    string usuario;
    string contra;
    string nombre;
    double dinero;

public:
    Account() : dinero(0.0) {}
    Account(string u, string p, string n, double b = 0.0)
        : usuario(u), contra(p), nombre(n), dinero(b) {
    }

    string getUsuario() const { return usuario; }
    string getNombre() const { return nombre; }
    double getDinero() const { return dinero; }
    bool verificarContra(const string& pass) const { return pass == contra; }

    void depositar(double amount) { dinero += amount; }
    bool retirarDinero(double amount) {
        if (amount > dinero) {
            cout << "Fondos Insuficientes";
            return false;
        };
        dinero -= amount;
        return true;
    }

    void setPassword(const string& newPass) { contra = newPass; }

    string toText() const {
        ostringstream oss;
        oss << usuario << "|" << contra << "|" << nombre << "|" << dinero;
        return oss.str();
    }

    static bool fromTextLine(const string& line, Account& out) {
        stringstream ss(line);
        string user, pass, nombre, balStr;

        if (!getline(ss, user, '|')) return false;
        if (!getline(ss, pass, '|')) return false;
        if (!getline(ss, nombre, '|')) return false;
        if (!getline(ss, balStr, '|')) return false;

        double bal = stod(balStr);
        out = Account(user, pass, nombre, bal);
        return true;
    }
};

// ==================== Clase BankSystem ====================
class BankSystem {
private:
    vector<Account> accounts;
    string filenombre;

public:
    BankSystem(const string& fnombre = "accounts.txt") : filenombre(fnombre) {
        loadFromFile();
    }

    void loadFromFile() {
        accounts.clear();
        ifstream file(filenombre);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            Account acc;
            if (Account::fromTextLine(line, acc))
                accounts.push_back(acc);
        }
        file.close();
    }

    void saveToFile() const {
        ofstream file(filenombre, ios::trunc);
        for (const auto& acc : accounts)
            file << acc.toText() << endl;
        file.close();
    }

    Account* findAccount(const string& usuario) {
        for (auto& acc : accounts)
            if (acc.getUsuario() == usuario)
                return &acc;
        return nullptr;
    }

    void registerAccount() {
        system(CLEAR);
        cout << "=== REGISTRAR NUEVA CUENTA ===\n";
        string usuario, contra, confirm, nombre;

        cout << "Usuario: ";
        getline(cin, usuario);
        if (findAccount(usuario)) {
            cout << "El usuario ya existe.\n";
            pause();
            return;
        }

        cout << "Contraseña: ";
        getline(cin, contra);
        cout << "Confirma la contraseña: ";
        getline(cin, confirm);
        if (contra != confirm) {
            cout << "Las contraseñas no coinciden.\n";
            pause();
            return;
        }

        cout << "Nombre completo: ";
        getline(cin, nombre);

        accounts.emplace_back(usuario, contra, nombre);
        saveToFile();
        cout << "Cuenta creada exitosamente.\n";
        pause();
    }

    Account* login() {
        system(CLEAR);
        cout << "=== INICIAR SESIÓN ===\n";
        string usuario, contra;
        cout << "Usuario: ";
        getline(cin, usuario);
        cout << "Contraseña: ";
        getline(cin, contra);

        Account* acc = findAccount(usuario);
        if (!acc || !acc->verificarContra(contra)) {
            cout << "Usuario o contraseña incorrectos.\n";
            pause();
            return nullptr;
        }

        cout << "Bienvenido, " << acc->getNombre() << "!\n";
        pause();
        return acc;
    }

    void accountMenu(Account* acc) {
        int option = 0;
        do {
            system(CLEAR);
            cout << "=== MENU DE CUENTA ===\n";
            cout << "Usuario: " << acc->getUsuario() << "\n";
            cout << "Saldo actual: Q" << fixed << setprecision(2) << acc->getDinero() << "\n\n";
            cout << "1. Depositar\n";
            cout << "2. Retirar\n";
            cout << "3. Cambiar contraseña\n";
            cout << "4. Cerrar sesión\n";
            cout << "Seleccione una opción: ";

            cin >> option;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            system(CLEAR);

            switch (option) {
            case 1: depositar(acc); break;
            case 2: retirarDinero(acc); break;
            case 3: changePassword(acc); break;
            case 4: cout << "Cerrando sesión...\n"; break;
            default: cout << "Opción inválida.\n"; pause();
            }
        } while (option != 4);
    }

    void depositar(Account* acc) {
        double amount;
        cout << "Ingrese monto a depositarar: Q";
        cin >> amount;
        if (cin.fail() || amount <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Monto inválido.\n";
            pause();
            return;
        }

        acc->depositar(amount);
        saveToFile();
        cout << "Depósito exitoso. Nuevo saldo: Q" << acc->getDinero() << "\n";
        pause();
    }

    void retirarDinero(Account* acc) {
        double amount;
        cout << "Ingrese monto a retirar: Q";
        cin >> amount;
        if (cin.fail() || amount <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Monto inválido.\n";
            pause();
            return;
        }

        if (!acc->retirarDinero(amount)) {
            cout << "Saldo insuficiente.\n";
        }
        else {
            saveToFile();
            cout << "Retiro exitoso. Nuevo saldo: Q" << acc->getDinero() << "\n";
        }
        pause();
    }

    void changePassword(Account* acc) {
        string contraAntigua, contraNueva;
        cout << "Contraseña actual: ";
        getline(cin, contraNueva);
        if (!acc->verificarContra(contraAntigua)) {
            cout << "Contraseña incorrecta.\n";
            pause();
            return;
        }
        cout << "Nueva contraseña: ";
        getline(cin, contraNueva);
        acc->setPassword(contraNueva);
        saveToFile();
        cout << "Contraseña actualizada correctamente.\n";
        pause();
    }

    void pause() {
        cout << "\nPresiona Enter para continuar...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
};

// ==================== Función principal ====================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BankSystem bank;
    int option = 0;

    do {
        system(CLEAR);
        cout << "=== CAJERO AUTOMÁTICO ===\n";
        cout << "1. Iniciar sesión\n";
        cout << "2. Registrarse\n";
        cout << "3. Salir\n";
        cout << "Seleccione una opción: ";
        cin >> option;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        system(CLEAR);
        switch (option) {
        case 1: {
            Account* acc = bank.login();
            if (acc) bank.accountMenu(acc);
            break;
        }
        case 2: bank.registerAccount(); break;
        case 3: cout << "Saliendo del sistema...\n"; break;
        default: cout << "Opción inválida.\n"; bank.pause();
        }
    } while (option != 3);

    return 0;
}
