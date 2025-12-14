#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>
#include <algorithm>
#include <map>
#include <ctime>

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
    string kategori;
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
    const string BARANG_DEFAULT = "barang_default.txt";
    const string OUTPUT_FILE = "output.txt";

public:
    AplikasiToko() : sudahLogin(false) {
        cout << "\n ===== TOKO INDOMIE =====" << endl;
        
        if (!loadData()) {
            cout << "\n PROGRAM DIHENTIKAN" << endl;
            cout << "File katalog default tidak ditemukan!" << endl;
            cout << "\n Buat file 'barang_default.txt' dengan format:" << endl;
            cout << "   ID ; Nama ; Harga ; Stok ; Deskripsi ; Kategori" << endl;
            cout << "\n Contoh:" << endl;
            cout << "1 ; Indomie Goreng Original ; 3500 ; 200 ; Indomie goreng original ; GORENG" << endl;
            cout << "2 ; Indomie Soto ; 3500 ; 150 ; Indomie kuah soto ; KUAH" << endl;
            exit(1); // Keluar program
        }
    }


    // LOAD DATA - HANYA BACA DARI FILE
    bool loadData() {
        // 1. Cek file default ada?
        ifstream defaultFile(BARANG_DEFAULT);
        if (!defaultFile.is_open()) {
            cout << " ERROR: File '" << BARANG_DEFAULT << "' tidak ditemukan!" << endl;
            return false;
        }
        defaultFile.close();
        
        // 2. Load users (jika ada)
        ifstream userIn(USER_FILE);
        if (userIn.is_open()) {
            User u;
            while (userIn >> u.username >> u.password >> u.role 
                   >> u.nama >> u.alamat >> u.noTelp) {
                users.push_back(u);
            }
            userIn.close();
        }

        // 3. Load barang dari file default
        return loadBarangFromFile(BARANG_DEFAULT);
    }

    bool loadBarangFromFile(const string& filename) {
        ifstream fileIn(filename);
        if (!fileIn.is_open()) {
            cout << " Gagal membuka file: " << filename << endl;
            return false;
        }
        
        string line;
        katalog.clear();
        int lineNum = 0;
        
        while (getline(fileIn, line)) {
            lineNum++;
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse line: ID ; Nama ; Harga ; Stok ; Deskripsi ; Kategori
            Barang b;
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, ';')) {
                // Trim whitespace
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                tokens.push_back(token);
            }
            
            // Harus ada minimal 6 field
            if (tokens.size() >= 6) {
                try {
                    b.id = stoi(tokens[0]);
                    b.nama = tokens[1];
                    b.harga = stod(tokens[2]);
                    b.stok = stoi(tokens[3]);
                    b.deskripsi = tokens[4];
                    b.kategori = tokens[5];
                    
                    katalog.push_back(b);
                } catch (...) {
                    cout << "  Baris " << lineNum << " format salah: " << line << endl;
                }
            } else {
                cout << "  Baris " << lineNum << " kolom kurang: " << line << endl;
            }
        }
        
        fileIn.close();
        
        if (katalog.empty()) {
            cout << " File katalog kosong atau format salah!" << endl;
            return false;
        }
        
        cout << " Katalog dimuat: " << katalog.size() << " varian indomie" << endl;
        
        // Simpan ke barang.txt (copy dari default)
        saveBarangToFile();
        return true;
    }

    void saveBarangToFile() {
        ofstream barangOut(BARANG_FILE);
        for (const auto& b : katalog) {
            barangOut << b.id << " ; " << b.nama << " ; " << b.harga 
                      << " ; " << b.stok << " ; " << b.deskripsi 
                      << " ; " << b.kategori << endl;
        }
        barangOut.close();
        
        // Juga update file default (untuk sync)
        ofstream defaultOut(BARANG_DEFAULT);
        for (const auto& b : katalog) {
            defaultOut << b.id << " ; " << b.nama << " ; " << b.harga 
                       << " ; " << b.stok << " ; " << b.deskripsi 
                       << " ; " << b.kategori << endl;
        }
        defaultOut.close();
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
        saveBarangToFile();

        // Log
        ofstream out(OUTPUT_FILE, ios::app);
        time_t now = time(0);
        out << "Save: " << ctime(&now);
        out.close();
    }

    // AUTHENTICATION
    void registrasi() {
        User baru;
        cout << "\n REGISTRASI " << endl;
        cout << "Username: "; cin >> baru.username;
        cout << "Password: "; cin >> baru.password;
        cout << "Role (pembeli/penjual): "; cin >> baru.role;
        cout << "Nama Lengkap: "; cin.ignore(); getline(cin, baru.nama);
        cout << "Alamat: "; getline(cin, baru.alamat);
        cout << "No. Telepon: "; getline(cin, baru.noTelp);

        users.push_back(baru);
        saveData();
        cout << " Registrasi berhasil!" << endl;
    }

    bool login() {
        string username, password;
        cout << "\n LOGIN " << endl;
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;

        for (const auto& u : users) {
            if (u.username == username && u.password == password) {
                userAktif = u;
                sudahLogin = true;
                cout << " Login berhasil sebagai " << u.role << "!" << endl;
                return true;
            }
        }
        cout << " Login gagal!" << endl;
        return false;
    }

    void logout() {
        sudahLogin = false;
        cout << " Logout berhasil!" << endl;
    }

    // FUNGSI TAMPILAN
    void tampilkanKatalog() {
        if (katalog.empty()) {
            cout << "\n Katalog kosong!" << endl;
            return;
        }
        
        cout << "\n KATALOG INDOMIE & VARIAN" << endl;
        cout << string(70, '=') << endl;
        cout << left << setw(5) << "ID" << setw(30) << "NAMA" 
             << setw(10) << "HARGA" << setw(8) << "STOK" << "KATEGORI" << endl;
        cout << string(70, '=') << endl;
        
        for (const auto& b : katalog) {
            string nama = b.nama;
            if (nama.length() > 28) nama = nama.substr(0, 25) + "...";
            
            cout << left << setw(5) << b.id 
                 << setw(30) << nama
                 << "Rp" << setw(8) << fixed << setprecision(0) << b.harga
                 << setw(8) << b.stok
                 << b.kategori << endl;
        }
        cout << string(70, '=') << endl;
        cout << " Total: " << katalog.size() << " varian" << endl;
    }

    // MENU PEMBELI
    void menuPembeli() {
        int pilihan;
        do {
            cout << "\n MENU PEMBELI" << endl;
            cout << "1. Lihat Katalog" << endl;
            cout << "2. Beli Indomie" << endl;
            cout << "3. Edit Profil" << endl;
            cout << "4. Logout" << endl;
            cout << "Pilihan: "; cin >> pilihan;

            switch (pilihan) {
                case 1: tampilkanKatalog(); break;
                case 2: prosesPembelian(); break;
                case 3: editProfil(); break;
                case 4: logout(); break;
                default: cout << " Pilihan tidak valid!" << endl;
            }
        } while (pilihan != 4 && sudahLogin);
    }

    void prosesPembelian() {
        tampilkanKatalog();
        int id, jumlah;
        cout << "\nMasukkan ID varian: "; cin >> id;
        cout << "Jumlah: "; cin >> jumlah;

        for (auto& b : katalog) {
            if (b.id == id) {
                if (b.stok >= jumlah) {
                    double total = b.harga * jumlah;
                    cout << "\n CHECKOUT " << endl;
                    cout << "Barang: " << b.nama << endl;
                    cout << "Total: Rp" << fixed << setprecision(0) << total << endl;
                    
                    cout << "\nMetode pembayaran: ";
                    cout << "\n1. Transfer Bank\n2. E-Wallet\n3. COD\nPilihan: ";
                    int metode; cin >> metode;
                    
                    cout << " Pembayaran berhasil!" << endl;
                    
                    b.stok -= jumlah;
                    saveData();
                    
                    // Log
                    ofstream out(OUTPUT_FILE, ios::app);
                    out << userAktif.username << " beli " << jumlah << " " << b.nama << endl;
                    out.close();
                    
                } else {
                    cout << " Stok tidak cukup!" << endl;
                }
                return;
            }
        }
        cout << " Barang tidak ditemukan!" << endl;
    }

    void editProfil() {
        cout << "\n EDIT PROFIL " << endl;
        cout << "Nama baru: "; cin.ignore(); getline(cin, userAktif.nama);
        cout << "Alamat baru: "; getline(cin, userAktif.alamat);
        cout << "No. Telepon baru: "; getline(cin, userAktif.noTelp);

        for (auto& u : users) {
            if (u.username == userAktif.username) {
                u = userAktif;
                break;
            }
        }
        saveData();
        cout << " Profil berhasil diupdate!" << endl;
    }

    // MENU PENJUAL
    void menuPenjual() {
        int pilihan;
        do {
            cout << "\n MENU PENJUAL" << endl;
            cout << "1. Lihat Katalog" << endl;
            cout << "2. Tambah Barang" << endl;
            cout << "3. Edit Barang" << endl;
            cout << "4. Hapus Barang" << endl;
            cout << "5. Update Stok" << endl;
            cout << "6. Logout" << endl;
            cout << "Pilihan: "; cin >> pilihan;

            switch (pilihan) {
                case 1: tampilkanKatalog(); break;
                case 2: tambahBarang(); break;
                case 3: editBarang(); break;
                case 4: hapusBarang(); break;
                case 5: updateStok(); break;
                case 6: logout(); break;
                default: cout << " Pilihan tidak valid!" << endl;
            }
        } while (pilihan != 6 && sudahLogin);
    }

    void tambahBarang() {
        Barang baru;
        cout << "\n TAMBAH BARANG " << endl;
        cout << "ID: "; cin >> baru.id;
        
        for (const auto& b : katalog) {
            if (b.id == baru.id) {
                cout << " ID sudah digunakan!" << endl;
                return;
            }
        }
        
        cout << "Nama: "; cin.ignore(); getline(cin, baru.nama);
        cout << "Harga: "; cin >> baru.harga;
        cout << "Stok: "; cin >> baru.stok;
        cout << "Deskripsi: "; cin.ignore(); getline(cin, baru.deskripsi);
        cout << "Kategori: "; getline(cin, baru.kategori);

        katalog.push_back(baru);
        saveData();
        cout << " Barang berhasil ditambahkan!" << endl;
    }

    void editBarang() {
        tampilkanKatalog();
        int id;
        cout << "\nMasukkan ID barang yang akan diedit: "; cin >> id;

        for (auto& b : katalog) {
            if (b.id == id) {
                cout << "Nama baru: "; cin.ignore(); getline(cin, b.nama);
                cout << "Harga baru: "; cin >> b.harga;
                cout << "Stok baru: "; cin >> b.stok;
                cout << "Deskripsi baru: "; cin.ignore(); getline(cin, b.deskripsi);
                cout << "Kategori baru: "; getline(cin, b.kategori);
                
                saveData();
                cout << " Barang berhasil diupdate!" << endl;
                return;
            }
        }
        cout << " Barang tidak ditemukan!" << endl;
    }

    void updateStok() {
        tampilkanKatalog();
        int id, tambahan;
        cout << "\nMasukkan ID barang: "; cin >> id;
        cout << "Jumlah tambahan: "; cin >> tambahan;

        for (auto& b : katalog) {
            if (b.id == id) {
                b.stok += tambahan;
                saveData();
                cout << " Stok berhasil diupdate: " << b.stok << endl;
                return;
            }
        }
        cout << " Barang tidak ditemukan!" << endl;
    }

    void hapusBarang() {
        tampilkanKatalog();
        int id;
        cout << "\nMasukkan ID barang yang akan dihapus: "; cin >> id;

        for (auto it = katalog.begin(); it != katalog.end(); ++it) {
            if (it->id == id) {
                katalog.erase(it);
                saveData();
                cout << " Barang berhasil dihapus!" << endl;
                return;
            }
        }
        cout << " Barang tidak ditemukan!" << endl;
    }

    // MAIN MENU
    void jalankan() {
        int pilihan;
        do {
            cout << "\n APLIKASI TOKO INDOMIE" << endl;
            cout << "1. Login" << endl;
            cout << "2. Registrasi" << endl;
            cout << "3. Keluar" << endl;
            cout << "Pilihan: "; cin >> pilihan;

            switch (pilihan) {
                case 1: 
                    if (login()) {
                        if (userAktif.role == "pembeli") menuPembeli();
                        else if (userAktif.role == "penjual") menuPenjual();
                    }
                    break;
                case 2: registrasi(); break;
                case 3: cout << "Terima kasih!" << endl; break;
                default: cout << "Pilihan tidak valid!" << endl;
            }
        } while (pilihan != 3);
    }
};

// MAIN FUNCTION
int main() {
    AplikasiToko app;
    app.jalankan();
    return 0;

}
