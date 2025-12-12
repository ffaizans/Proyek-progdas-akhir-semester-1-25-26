#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

// ============================
// STRUCT DAN CLASS
// ============================

struct User {
    string username;
    string password;
    string role; // "pembeli" atau "penjual"
    string nama;
    string alamat;
    string noTelp;
};

struct Barang {
    int id;
    string nama;
    double harga;
    int stok;
    string deskripsi;
};

class AplikasiToko {
private:
    vector<User> users;
    vector<Barang> katalog;
    User userAktif;
    bool sudahLogin;

    // File paths
    const string USER_FILE = "users.txt";
    const string BARANG_FILE = "barang.txt";
    const string OUTPUT_FILE = "output.txt";

public:
    AplikasiToko() : sudahLogin(false) {
        loadData();
    }

    // ============================
    // LOAD & SAVE DATA
    // ============================
    void loadData() {
        // Load users
        ifstream userIn(USER_FILE);
        if (userIn.is_open()) {
            User u;
            while (userIn >> u.username >> u.password >> u.role >> u.nama >> u.alamat >> u.noTelp) {
                users.push_back(u);
            }
            userIn.close();
        }

        // Load barang
        ifstream barangIn(BARANG_FILE);
        if (barangIn.is_open()) {
            Barang b;
            while (barangIn >> b.id >> b.nama >> b.harga >> b.stok >> b.deskripsi) {
                katalog.push_back(b);
            }
            barangIn.close();
        }
    }

    void saveData() {
        // Save users
        ofstream userOut(USER_FILE);
        for (const auto& u : users) {
            userOut << u.username << " " << u.password << " " << u.role << " "
                    << u.nama << " " << u.alamat << " " << u.noTelp << endl;
        }
        userOut.close();

        // Save barang
        ofstream barangOut(BARANG_FILE);
        for (const auto& b : katalog) {
            barangOut << b.id << " " << b.nama << " " << b.harga << " "
                      << b.stok << " " << b.deskripsi << endl;
        }
        barangOut.close();

        // Simpan log ke output.txt
        ofstream out(OUTPUT_FILE, ios::app);
        out << "Data tersimpan pada: " << time(nullptr) << endl;
        out.close();
    }

    // ============================
    // AUTHENTICATION
    // ============================
    void registrasi() {
        User baru;
        cout << "\n=== REGISTRASI ===" << endl;
        cout << "Username: "; cin >> baru.username;
        cout << "Password: "; cin >> baru.password;
        cout << "Role (pembeli/penjual): "; cin >> baru.role;
        cout << "Nama Lengkap: "; cin.ignore(); getline(cin, baru.nama);
        cout << "Alamat: "; getline(cin, baru.alamat);
        cout << "No. Telepon: "; getline(cin, baru.noTelp);

        users.push_back(baru);
        saveData();
        cout << "Registrasi berhasil!" << endl;
    }

    bool login() {
        string username, password;
        cout << "\n=== LOGIN ===" << endl;
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;

        for (const auto& u : users) {
            if (u.username == username && u.password == password) {
                userAktif = u;
                sudahLogin = true;
                cout << "Login berhasil sebagai " << u.role << "!" << endl;
                return true;
            }
        }
        cout << "Login gagal!" << endl;
        return false;
    }

    void logout() {
        sudahLogin = false;
        cout << "Logout berhasil!" << endl;
    }

    // ============================
    // FUNGSI UMUM
    // ============================
    void tampilkanKatalog() {
        cout << "\n=== KATALOG BARANG ===" << endl;
        cout << left << setw(5) << "ID" << setw(20) << "Nama" 
             << setw(10) << "Harga" << setw(10) << "Stok" << "Deskripsi" << endl;
        cout << string(70, '-') << endl;
        for (const auto& b : katalog) {
            cout << left << setw(5) << b.id << setw(20) << b.nama 
                 << setw(10) << b.harga << setw(10) << b.stok << b.deskripsi << endl;
        }
    }

    // ============================
    // MENU PEMBELI
    // ============================
    void menuPembeli() {
        int pilihan;
        do {
            cout << "\n=== MENU PEMBELI ===" << endl;
            cout << "1. Lihat Katalog" << endl;
            cout << "2. Beli Barang" << endl;
            cout << "3. Edit Profil" << endl;
            cout << "4. Logout" << endl;
            cout << "Pilihan: "; cin >> pilihan;

            switch (pilihan) {
                case 1: tampilkanKatalog(); break;
                case 2: prosesPembelian(); break;
                case 3: editProfil(); break;
                case 4: logout(); break;
                default: cout << "Pilihan tidak valid!" << endl;
            }
        } while (pilihan != 4 && sudahLogin);
    }

    void prosesPembelian() {
        tampilkanKatalog();
        int id, jumlah;
        cout << "\nMasukkan ID barang yang ingin dibeli: "; cin >> id;
        cout << "Jumlah: "; cin >> jumlah;

        for (auto& b : katalog) {
            if (b.id == id) {
                if (b.stok >= jumlah) {
                    double total = b.harga * jumlah;
                    cout << "\n=== CHECKOUT ===" << endl;
                    cout << "Barang: " << b.nama << endl;
                    cout << "Jumlah: " << jumlah << endl;
                    cout << "Total: Rp" << total << endl;
                    
                    int metode;
                    cout << "\nPilih metode pembayaran:" << endl;
                    cout << "1. Transfer Bank" << endl;
                    cout << "2. E-Wallet" << endl;
                    cout << "3. COD" << endl;
                    cout << "Pilihan: "; cin >> metode;
                    
                    // Simulasi pembayaran
                    cout << "Memproses pembayaran..." << endl;
                    cout << "Pembayaran berhasil!" << endl;
                    
                    // Kurangi stok
                    b.stok -= jumlah;
                    saveData();
                    
                    // Simpan transaksi
                    ofstream out(OUTPUT_FILE, ios::app);
                    out << "Transaksi: " << userAktif.username << " membeli " 
                        << jumlah << " " << b.nama << " seharga Rp" << total << endl;
                    out.close();
                    
                } else {
                    cout << "Stok tidak mencukupi!" << endl;
                }
                return;
            }
        }
        cout << "Barang tidak ditemukan!" << endl;
    }

    void editProfil() {
        cout << "\n=== EDIT PROFIL ===" << endl;
        cout << "Nama baru [" << userAktif.nama << "]: ";
        cin.ignore(); getline(cin, userAktif.nama);
        cout << "Alamat baru [" << userAktif.alamat << "]: ";
        getline(cin, userAktif.alamat);
        cout << "No. Telepon baru [" << userAktif.noTelp << "]: ";
        getline(cin, userAktif.noTelp);

        // Update di vector users
        for (auto& u : users) {
            if (u.username == userAktif.username) {
                u = userAktif;
                break;
            }
        }
        saveData();
        cout << "Profil berhasil diupdate!" << endl;
    }

    // ============================
    // MENU PENJUAL
    // ============================
    void menuPenjual() {
        int pilihan;
        do {
            cout << "\n=== MENU PENJUAL ===" << endl;
            cout << "1. Lihat Katalog" << endl;
            cout << "2. Tambah Barang" << endl;
            cout << "3. Edit Barang" << endl;
            cout << "4. Hapus Barang" << endl;
            cout << "5. Logout" << endl;
            cout << "Pilihan: "; cin >> pilihan;

            switch (pilihan) {
                case 1: tampilkanKatalog(); break;
                case 2: tambahBarang(); break;
                case 3: editBarang(); break;
                case 4: hapusBarang(); break;
                case 5: logout(); break;
                default: cout << "Pilihan tidak valid!" << endl;
            }
        } while (pilihan != 5 && sudahLogin);
    }

    void tambahBarang() {
        Barang baru;
        cout << "\n=== TAMBAH BARANG ===" << endl;
        cout << "ID: "; cin >> baru.id;
        cout << "Nama: "; cin.ignore(); getline(cin, baru.nama);
        cout << "Harga: "; cin >> baru.harga;
        cout << "Stok: "; cin >> baru.stok;
        cout << "Deskripsi: "; cin.ignore(); getline(cin, baru.deskripsi);

        katalog.push_back(baru);
        saveData();
        cout << "Barang berhasil ditambahkan!" << endl;
    }

    void editBarang() {
        tampilkanKatalog();
        int id;
        cout << "\nMasukkan ID barang yang akan diedit: "; cin >> id;

        for (auto& b : katalog) {
            if (b.id == id) {
                cout << "Nama baru [" << b.nama << "]: ";
                cin.ignore(); getline(cin, b.nama);
                cout << "Harga baru [" << b.harga << "]: "; cin >> b.harga;
                cout << "Stok baru [" << b.stok << "]: "; cin >> b.stok;
                cout << "Deskripsi baru [" << b.deskripsi << "]: ";
                cin.ignore(); getline(cin, b.deskripsi);
                
                saveData();
                cout << "Barang berhasil diupdate!" << endl;
                return;
            }
        }
        cout << "Barang tidak ditemukan!" << endl;
    }

    void hapusBarang() {
        tampilkanKatalog();
        int id;
        cout << "\nMasukkan ID barang yang akan dihapus: "; cin >> id;

        for (auto it = katalog.begin(); it != katalog.end(); ++it) {
            if (it->id == id) {
                katalog.erase(it);
                saveData();
                cout << "Barang berhasil dihapus!" << endl;
                return;
            }
        }
        cout << "Barang tidak ditemukan!" << endl;
    }

    // ============================
    // MAIN MENU
    // ============================
    void jalankan() {
        int pilihan;
        do {
            cout << "\n=== APLIKASI TOKO ONLINE ===" << endl;
            cout << "1. Login" << endl;
            cout << "2. Registrasi" << endl;
            cout << "3. Keluar" << endl;
            cout << "Pilihan: "; cin >> pilihan;

            switch (pilihan) {
                case 1: 
                    if (login()) {
                        if (userAktif.role == "pembeli") {
                            menuPembeli();
                        } else if (userAktif.role == "penjual") {
                            menuPenjual();
                        }
                    }
                    break;
                case 2: 
                    registrasi(); 
                    break;
                case 3: 
                    cout << "Terima kasih!" << endl; 
                    break;
                default: 
                    cout << "Pilihan tidak valid!" << endl;
            }
        } while (pilihan != 3);
    }
};

// ============================
// MAIN FUNCTION
// ============================
int main() {
    AplikasiToko app;
    app.jalankan();
    return 0;
}