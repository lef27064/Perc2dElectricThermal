# ETMPEWPT (Εκτίμηση Ηλεκτρικών, Θερμικών και Μηχανικών Ιδιοτήτων με Θεωρία Διήθησης) - 2D Έκδοση

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenMP](https://img.shields.io/badge/Parallel-OpenMP-orange.svg)](https://www.openmp.org/)

## Επισκόπηση (Overview)
Το **ETMPEWPT** είναι ένα ανοικτού κώδικα, υψηλής απόδοσης λογισμικό προσομοίωσης σε C++, σχεδιασμένο για την εκτίμηση των ενεργών ηλεκτρικών, θερμικών και μηχανικών ιδιοτήτων 2D συνθέτων μικροδομών μέσω **θεωρίας συνεχούς διήθησης (continuum percolation theory)** και **ψηφιοποίησης σωματιδίων (particle digitization)**.

Το πρόγραμμα δημιουργεί αντιπροσωπευτικά στοιχεία όγκου (RVEs) μέσω δειγματοληψίας Monte Carlo, αναλύει τη τοπολογία των συσσωματωμάτων (DFS / Hoshen-Kopelman) και αξιολογεί τις μακροσκοπικές μεταφορικές και μηχανικές ιδιότητες με βάση τις παραμέτρους των μεμονωμένων συστατικών και τη χωρική τους κατανομή.

---

## Πίνακας Περιεχομένων (Table of Contents)
1. [Βασικά Χαρακτηριστικά](#βασικά-χαρακτηριστικά)
2. [Απαιτήσεις Συστήματος](#απαιτήσεις-συστήματος)
3. [Γρήγορη Εκκίνηση & Δομή Φακέλων](#γρήγορη-εκκίνηση--δομή-φακέλων)
4. [Μεταγλώττιση (Compilation)](#μεταγλώττιση-compilation)
5. [Προδιαγραφές Αρχείου Εισόδου](#προδιαγραφές-αρχείου-εισόδου)
6. [Ρύθμιση Γενικών Παραμέτρων](#ρύθμιση-γενικών-παραμέτρων)
7. [Κατανόηση Αποτελεσμάτων](#κατανόηση-αποτελεσμάτων)
8. [Άδεια Χρήσης (License)](#άδεια-χρήσης-license)
9. [Αναφορές & Δημοσιεύσεις](#αναφορές--δημοσιεύσεις)

---

## Βασικά Χαρακτηριστικά
- **Ευέλικτες Γεωμετρίες Εγκλεισμάτων 2D:** Υποστηρίζει Ορθογώνια, Κεκλιμένα Ορθογώνια, Ελλείψεις και περιοχές Φάσης Μήτρας.
- **Επίλυση Πολλαπλών Ιδιοτήτων:** Υπολογίζει την ενεργό ηλεκτρική αγωγιμότητα ($\sigma_{eff}$), τη θερμική αγωγιμότητα ($k_{eff}$), το μέτρο ελαστικότητας Young ($E$) και το λόγο Poisson ($\nu$).
- **Αριθμητικές Μέθοδοι:** Υποστηρίζει τόσο γρήγορη ανάλυση διήθησης συσσωματωμάτων όσο και μεθόδους Πεπερασμένων Διαφορών (FD2D).
- **Πλέγματα Υψηλής Ανάλυσης:** Βελτιστοποιημένο για μικροδομές μεγάλης κλίμακας (π.χ. από $1000 \times 1000$ έως $10000 \times 10000$ pixels).
- **Παράλληλη Επεξεργασία:** Επιτάχυνση μέσω παράλληλου προγραμματισμού OpenMP για ταχεία σύγκλιση υπολογισμών Monte Carlo (200–300 πραγματοποιήσεις).
- **Εξαγωγή Οπτικών & Αριθμητικών Δεδομένων:** Αποθηκεύει μικροδομές σε μορφή BMP και αρχεία συντεταγμένων σωματιδίων (`shapes/`) για οπτική επαλήθευση.

---

## Απαιτήσεις Συστήματος
- **Λειτουργικό Σύστημα:** Windows 10/11, Linux, ή macOS.
- **Μεταγλωττιστής (Compiler):** Συμβατός με πρότυπο C++17 (GCC 8+, Clang 10+, ή MSVC 2019+).
- **Απαιτήσεις Μνήμης RAM:**
  - Πλέγμα $1000 \times 1000$: ~2 GB RAM
  - Πλέγμα $5000 \times 5000$: ~8 GB RAM
  - Πλέγμα $10000 \times 10000$: ~16 GB RAM

---

## Γρήγορη Εκκίνηση & Δομή Φακέλων

Βεβαιωθείτε ότι το εκτελέσιμο αρχείο βρίσκεται σε έναν κατάλογο με την ακόλουθη δομή:

```text
├── ETMPEWPT.exe
├── settings.txt
├── inputs/
│   └── example_2d.txt
└── outputs/
```

1. Τοποθετήστε τα αρχεία ρυθμίσεων στον φάκελο `inputs/`.
2. Επεξεργαστείτε το `settings.txt` για να ορίσετε τις διαδρομές εξόδου και το όριο πραγματοποιήσεων.
3. Εκτελέστε το `ETMPEWPT.exe`.

---

## Μεταγλώττιση (Compilation)

### Linux / macOS (GCC / Clang)
```bash
git clone https://github.com/lef27064/Perc2dElectricThermal.git
cd Perc2dElectricThermal/Percolation
g++ main.cpp cluster.cpp BatchMonteCarlo.cpp Settings.cpp Grid.cpp Shapes.cpp image.cpp FD2DEL.cpp ShapeGenerator.cpp general.cpp -o ETMPEWPT -fopenmp -std=c++17 -O3
```

### Windows (Visual Studio)
1. Ανοίξτε το solution του έργου στο Visual Studio.
2. Ορίστε το C++ Language Standard σε **C++17** (`/std:c++17`).
3. Ενεργοποιήστε την υποστήριξη **OpenMP** (`/openmp`) στις ιδιότητες του έργου (C/C++ Project Properties).
4. Κάντε Build σε λειτουργία **Release - x64**.

---

## Προδιαγραφές Αρχείου Εισόδου

Οι παράμετροι εισόδου ορίζονται μέσω αρχείων κειμένου (.txt) εντός του φακέλου `inputs/`.

### Παράδειγμα Αρχείου Εισόδου (`inputs/example_2d.txt`):
```text
#name
example_run
#total component
3
#components % weights
0.4 0.3 0.3
#Components type Geometry : RECTANGLE = 0, SLOPEDRECTANGLE = 1, ELLIPSE = 3, NONE = 4
4 3 1
#SizeType CONSTANT = 0, VARIABLE = 1, NONE = 3
3 0 0
#Special Weights
1.6532 1.3279 1.342
#Dimension X
10.0e-09 10.0e-09 1.0e-09 
#Dimension Y
10.0e-09 10.0e-09 10.0e-09 
#hoops
0.5e-09 0.5e-09 0.5e-09
#Electric Conductivities
1.e-16 1.0 0.5
#Thermal Conductivities
1.e-16 1.0 0.5
#Young Modulus
1.e+5 1.e+10 0.5e+9
#Poisson Ratio
0.3 0.32 0.31
#width - height
1000 1000
#iterations
200
#pixels per min Sphere
101
#Swiss chesse case TRUE or FALSE
FALSE
# CALC ELECTRIC CONDUCTIVITY
TRUE
# CALC ELECTRIC CONDUCTIVITY WITH FINITE DIFFERENCES
FALSE
# CALCULATE STATISTICS
FALSE
```

---

## Ρύθμιση Γενικών Παραμέτρων

Οι γενικές οδηγίες εκτέλεσης ορίζονται στο αρχείο `settings.txt`:

```text
# Συνολικές επαναλήψεις προσομοίωσης
Total Iterations to Run : 300
# Αποθήκευση αρχείων εικόνας (TRUE/FALSE)
Save image file(s) : TRUE
# Πλήθος τυχαίων εικόνων για αποθήκευση
Save random image(s) : 3
# Αποθήκευση εικόνων για συγκεκριμένες επαναλήψεις (TRUE/FALSE)
Save image(s) for specific iteration(s) : FALSE
Iterations to save : 1, 50, 100
# Κατάλογος Εξόδου
Current directory : "./outputs/"
```

---

## Κατανόηση Αποτελεσμάτων

Όλα τα αποτελέσματα ταξινομούνται αυτόματα στον κατάλογο `outputs/<όνομα_έργου>/`:
- `report.csv`: Δεδομένα ανά πραγματοποίηση και μέσοι όροι συνόλου ($\sigma_{eff}$, $k_{eff}$, κατάσταση διήθησης).
- `ReportWithSemicolon.csv`: Μορφή CSV με ελληνικό/ευρωπαϊκό διαχωριστικό ερωτηματικού (`;`) για το Excel.
- `Statistics.csv`: Κατανομή μεγέθους συσσωματωμάτων και δεδομένα κρίσιμου μονοπατιού διήθησης.
- `images/`: Εξαγόμενες 2D εικόνες BMP των μικροδομών για οπτικό έλεγχο.
- `shapes/`: Συντεταγμένες και δεδομένα προσανατολισμού των γεωμετριών των εγκλεισμάτων.

---

## Άδεια Χρήσης (License)

Διανέμεται υπό την άδεια **GNU General Public License v3.0 (GPL-3.0)**. Δείτε το αρχείο `LICENSE` για λεπτομέρειες.

---

## Αναφορές & Δημοσιεύσεις

**Συγγραφέας:** Ελευθέριος Λάμπρου  
**Επικοινωνία:** `lef27064@otenet.gr`

Εάν χρησιμοποιήσετε το **ETMPEWPT** στην έρευνά σας, παρακαλούμε να αναφέρετε τις ακόλουθες επιστημονικές δημοσιεύσεις:

1. **E. Lambrou and L. N. Gergidis**, *"A computational method for calculating the electrical and thermal properties of random composite"*, *Physica A: Statistical Mechanics and its Applications*, vol. 642, p. 129760, 2024. [DOI: 10.1016/j.physa.2024.129760](https://doi.org/10.1016/j.physa.2024.129760)
2. **E. Lambrou and L. N. Gergidis**, *"A particle digitization-based computational method for continuum percolation"*, *Physica A: Statistical Mechanics and its Applications*, vol. 590, p. 126738, 2022. [DOI: 10.1016/j.physa.2021.126738](https://doi.org/10.1016/j.physa.2021.126738)
