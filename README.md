# CeskySyntax - Nástroj k programování v Českém jazyce

### Toto je projekt určený pro uživatele českého jazyka, kteří mají po krk anglicismů a zkratek, jež nedávají smysl. Přicházím se zcela novým programovacím jazykem, který používá syntax C++, akorát s tím rozdílem, že používá výhradně České výrazy (s výjimkou nezbytné interpunkce). Pro překlad byl použit překladač Google (© Google Translate), s jakýmikoli stížnostmi proto prosím kontaktujte © Google LLC

Tento projekt je kompilátor typu zdrojový kód na zdrojový kód (transpiler)

Obsahuje také jednoduchý editor pro zjednodušení práce s kódem, jeho kompilací a spuštěním

Zjednodušeně:
1. Čte kód napsaný ve vlastním jazyce
2. Překládá jej na platný C++
3. Generuje soubor `.cpp`
4. Volitelně kompiluje a spouští vygenerovaný C++

## Architektura

- `Lexer`: rozděluje vstup na řádky/tokeny
- `Translator`: mapuje vlastní klíčová slova na C++ a transformuje příkazy
- `CodeGenerator`: obaluje přeložené řádky do platného C++ programu
- `Executor`: volitelný krok kompilace a spuštění pomocí `g++`

## Vlastní Klíčová Slova

Mapování se nachází v `src/Translator.cpp` (konstruktor třídy `Translator`).

Komplexní seznam klíčových slov viz tabulka níže

## Instalace a Nastavení (Windows)

### Podpora operačních systémů

- **Windows 10/11 (x86_64)** - plná podpora (doporučeno)
- **Linux** - částečná podpora (TUI editor, CLI transpiler)
- **macOS** - nedokončeno

### Požadavky

- **CMake** verze 3.16 nebo vyšší
- **C++ kompilátor** (MinGW 32-bit nebo LLVM-MinGW 64-bit)
- **Git** (pro klonování repozitáře)
- **PowerShell** 5.1 nebo vyšší (pro Windows)

### Krok 1: Instalace CMake

1. Jděte na https://cmake.org/download/
2. Stáhněte **CMake 3.28.0** nebo novější pro Windows (64-bit)
3. Spusťte instalátor a vyberte: **"Add CMake to the system PATH"** (důležité)
4. Ověřte instalaci v PowerShellu:
   ```powershell
   cmake --version
   ```

### Krok 2: Instalace LLVM-MinGW Kompilátoru

1. Navštívte https://github.com/mstorsjo/llvm-mingw/releases
2. Stáhněte nejnovější verzi **`llvm-mingw-*.ucrt-x86_64.zip`** (přibližně 1.5 GB)
   - Doporučeno: verze 20260421 nebo novější
3. Rozbalte ZIP soubor na jednoduchou cestu bez speciálních znaků, např.:
   ```
   C:\llvm-mingw
   ```
   nebo
   ```
   C:\Users\{YourUsername}\AppData\Local\llvm-mingw
   ```
4. Ověřte instalaci otevřením PowerShellu a spuštěním:
   ```powershell
   C:\llvm-mingw\bin\g++.exe --version
   ```
   Měl by se zobrazit výstup s verzí LLVM.

### Krok 3: Klonování Repozitáře

```powershell
cd C:\Users\{YourUsername}\Documents
git clone https://github.com/vase-uzivatelske-jmeno/CeskySyntax.git
cd CeskySyntax
```

### Krok 4: Konfigurace Prostředí

1. Otevřete PowerShell v adresáři projektu
2. Spusťte následující příkazy (nahraďte cestu dle vaší instalace):

```powershell
# Nastavte cestu ke kompilátoru
$env:PATH = "C:\Program Files\CMake\bin;C:\llvm-mingw\bin;" + $env:PATH

# Ověřte, že g++ je dostupný
g++ --version
```

### Krok 5: Konfigurace Projektu (CMake)

```powershell
# V adresáři projektu CeskySyntax
cmake -S . -B build-mingw -G "Unix Makefiles"
```

Tento příkaz vytvoří adresář `build-mingw` s konfiguračními soubory.

### Krok 6: Kompilace

Pro transpiler CLI:
```powershell
cmake --build build-mingw --target cesky_transpiler
```

Pro GUI editor (doporučeno pro uživatele):
```powershell
cmake --build build-mingw --target cesky_gui
```

Pro obě aplikace:
```powershell
cmake --build build-mingw
```

**Časový odhad:** První kompilace trvá 2-5 minut (ImGui se kompiluje poprvé), následující kompilace je rychlejší.

### Krok 7: Spuštění Aplikace

#### GUI Editor (Doporučeno)

```powershell
.\build-mingw\cesky_gui.exe
```

GUI se otevře v okně s:
- **Levý panel**: Editor kódu s českou syntaxí a zvýrazňováním
- **Pravý panel** (volitelný): Náhled přeloženého C++ kódu
- **Tlačítka**: Načíst příklad, Uložit, Přeložit a spustit, Zobrazit/Skrýt náhled
- **Dolní panel**: Výstup programu a chybové zprávy

#### CLI Transpiler

```powershell
.\build-mingw\cesky_transpiler.exe examples\demo.csx generated.cpp
```

Nebo s automatickým spuštěním:
```powershell
.\build-mingw\cesky_transpiler.exe examples\demo.csx generated.cpp --run --compiler=g++.exe
```

### Krok 8: První Test

1. Otevřete GUI (`cesky_gui.exe`)
2. Klikněte na tlačítko **"Ukázky"** a vyberte **"Základní cyklus"**
3. Klikněte **"Přeložit a spustit"**
4. V dolním panelu by se měl zobrazit výstup: `0 1 2 3 4`

### Řešení Problémů

**Chyba: "cmake: příkaz nebyl nalezen"**
- Zkontrolujte, zda je CMake nainstalován v `C:\Program Files\CMake\bin`
- Restartujte PowerShell a zkuste znovu

**Chyba: "g++: příkaz nebyl nalezen"**
- Ověřte, že LLVM-MinGW je rozbalena v `C:\llvm-mingw`
- Spusťte: `$env:PATH = "C:\llvm-mingw\bin;" + $env:PATH` v PowerShellu

**Chyba: "undefined reference to" během linkování**
- Ujistěte se, že kompilujete s `cmake --build build-mingw`, nikoli ručně
- Smazte `build-mingw` a zkonfigurujte znovu: `cmake -S . -B build-mingw -G "Unix Makefiles"`

**Chyba: Znaky jsou v GUI špatně zobrazeny (místo "č" se zobrazuje jiný znak)**
- To je normální, pokud systém nemá nainstalován správný font
- GUI by měl automaticky načíst Segoe UI nebo Consolas

**Chyba: "Permission denied" při kompilaci**
- Zavřete `cesky_gui.exe`, pokud je spuštěný
- Zkuste znovu: `cmake --build build-mingw --target cesky_gui`

### Volitelné: Nastavení Prostředí do Profilu PowerShellu

Abyste nemuseli pokaždé nastavovat `$env:PATH`, přidejte do PowerShellprofile:

```powershell
$profile
```

Otevřete soubor v textovém editoru a přidejte:
```powershell
# CeskySyntax Setup
$env:PATH = "C:\Program Files\CMake\bin;C:\llvm-mingw\bin;" + $env:PATH
```

Příště se prostředí automaticky nastaví při otevření PowerShellu.

## Spuštění Transpilátoru

```bash
./build/cesky_transpiler examples/demo.csx generated.cpp
```

Volitelně kompilujte a spusťte vygenerovaný C++:

```bash
./build/cesky_transpiler examples/demo.csx generated.cpp --run --compiler=g++
```

Na Windows s MinGW v PATH nahraďte `./build/cesky_transpiler` cestou spustitelného souboru vygenerovanou nástrojem CMake.

## Editor TUI

Spusťte editor terminálu se stavem prostředí:

```powershell
$toolbin = "C:\path\to\llvm-mingw\bin"
$env:CESKYSYNTAX_TOOLBIN = $toolbin
$env:CESKYSYNTAX_COMPILER = "g++.exe"
$env:PATH = "$toolbin;$env:PATH"
./build/cesky_editor.exe
```

Příkazy:
- `pridat` — Přidání řádky kódu (možno přidat víc řádků naráz, pro ukončení vložte jeden prázdný řádek)
- `upravit <N>` — Upravení řádky číslo N
- `smazat <N>` — Smazání řádky číslo N
- `ukazat` — Zobrazení všech řádek
- `spustit` — Transpilace, kompilace a spuštění kódu
- `spustitexe` — Spuštění posledního vygenerovaného souboru (bez rekompilace)
- `smazatvse` — Vymazání veškerého kódu
- `ukoncit` — Zavření editoru

## GUI Editor (Dear ImGui)

GUI editor je doporučený způsob používání CeskySyntax. Nabízí:

- **Editor s zvýrazňováním syntaxe** - Podpora všech českých klíčových slov s barvením
- **Náhled C++** - Vidíte generovaný C++ kód vedle svého zdrojového kódu
- **Příklady** - Předdefinované příklady (Základní cyklus, FizzBuzz, Součet a průměr atd.)
- **Spuštění** - Přeložit, zkompilovat a spustit kód jedním kliknutím
- **Uložení/Načtení** - Uložte svůj kód a načtěte jej později
- **Automata `int main()`** - Volitelně automaticky obalte kód do `int main()` funkce

### Spuštění GUI

Po kompilaci (viz výše) jednoduše spusťte:

```powershell
.\build-mingw\cesky_gui.exe
```

GUI závisí na ImGui, GLFW a dalších knihovnách, které jsou již zahrnuty v `third_party/` jako vendored knihovny.

### Ovládání GUI

1. **Tlačítko "Načíst příklad kódu"** - Načte poslední uložený kód
2. **Tlačítko "Uložit"** - Uloží aktuální kód
3. **Tlačítko "Ukázky"** - Nabídne seznam příkladů
4. **Tlačítko "Přeložit a spustit"** - Transpiluje, kompiluje a spustí kód
5. **Tlačítko "Skrýt/Zobrazit náhled C++"** - Zapne/vypne náhled vygenerovaného C++
6. **Tlačítko "Spustit poslední spustitelný kód"** - Spustí poslední zkompilovaný výstup bez nové transpilace
7. **Checkbox "Přidat int main() automaticky"** - Určuje, zda se automaticky přidá `int main()` obal
8. **Horní panel** - Zvýrazňování syntaxe a editace kódu
9. **Pravý panel** - Náhled generovaného C++ kódu (pokud je aktivován)
10. **Dolní panel** - Výstup programu a chybové zprávy

## Příklad Vstupu (`.csx`) (hezký čitelný kód)

```txt
celé číslo x rovná se 0
zatímco x menší než 5 {
    norma::znakový výstup << x << norma::koncová čára
    x rovná se x plus 1
}
když x rovná se? 5 {
    norma::znakový výstup << (x krát 10) << norma::koncová čára
} jinak {
    norma::znakový výstup << 0 << norma::koncová čára
}
```

## Příklad Vygenerovaného C++ (složité C++ plné zkratek a anglických výrazů)

```cpp
#include <iostream>
#include <string>

int main() {
    int x = 0;
    while (x < 5) {
    std::cout << x << std::endl;
    x = x + 1;
    }
    if (x == 5) {
    std::cout << (x * 10) << std::endl;
    } else {
    std::cout << 0 << std::endl;
    }
    return 0;
}
```

## Tabulka Klíčových Slov

| České Klíčové Slovo | C++ Ekvivalent | Kategorie |
|---|---|---|
| celé číslo | int | Datový typ |
| levituj | float | Datový typ |
| krátké desetinné číslo | float | Datový typ (varianta) |
| dvojté | double | Datový typ |
| dlouhé desetinné číslo | double | Datový typ (varianta) |
| charakter | char | Datový typ |
| znak | char | Datový typ (varianta) |
| pravdivostní hodnota | bool | Datový typ |
| prázdnota | void | Datový typ |
| funkce | void | Datový typ (varianta) |
| provázek | string | Datový typ |
| text | string | Datový typ (varianta) |
| pravda | true | Literál |
| falešný | false | Literál |
| nepravda | false | Literál (varianta) |
| když | if | Kontrolní tok |
| jinak kdyby | else if | Kontrolní tok |
| jinak | else | Kontrolní tok |
| přepínač | switch | Kontrolní tok |
| věc | case | Kontrolní tok |
| případ | case | Kontrolní tok (varianta) |
| výchozí | default | Kontrolní tok |
| přerušení | break | Kontrolní tok |
| pokračovat | continue | Kontrolní tok |
| návrat | return | Kontrolní tok |
| vrátit | return | Kontrolní tok (varianta) |
| pro | for | Kontrolní tok |
| zatímco | while | Kontrolní tok |
| dělat | do | Kontrolní tok |
| třída | class | Objektové programování |
| strukturovat | struct | Objektové programování |
| veřejnost | public | Objektové programování |
| veřejný | public | Objektové programování (varianta) |
| soukromé | private | Objektové programování |
| soukromý | private | Objektové programování (varianta) |
| chráněný | protected | Objektové programování |
| tento | this | Objektové programování |
| viruální | virtual | Objektové programování |
| přepsat | override | Objektové programování |
| nový | new | Objektové programování |
| smazat | delete | Objektové programování |
| v řadě za sebou | inline | Modifikátor |
| jmenný prostor | namespace | Organizace |
| pomocí | using | Organizace |
| pokus | try | Výjimky |
| chytit | catch | Výjimky |
| hod | throw | Výjimky |
| nulový ukazatel | nullptr | Ukazatel |
| norma | std | Jmenný prostor |
| norma::znakový výstup | std::cout | Výstup |
| začátek výpisu | std::cout | Výstup |
| znakový výstup | cout | Výstup |
| zadávání znaků | cin | Vstup |
| norma::koncová čára | std::endl | Výstup |
| konec výpisu | std::endl | Výstup |
| koncová čára | endl | Výstup |
| rovná se | = | Operátor přiřazení |
| plus | + | Aritmetický operátor |
| mínus | - | Aritmetický operátor |
| krát | * | Aritmetický operátor |
| děleno | / | Aritmetický operátor |
| zbytek po dělení | % | Aritmetický operátor |
| rovná se? | == | Porovnání |
| nerovná se | != | Porovnání |
| menší než | < | Porovnání |
| větší než | > | Porovnání |
| menší nebo rovno | <= | Porovnání |
| větší nebo rovno | >= | Porovnání |
| a zároveň | && | Logika |
| nebo | \|\| | Logika |
| není | ! | Logika |

---

# CeskySyntax - Tool for Programming in the Czech Language

### This is a project intended for Czech-speaking users who are fed up with anglicisms and abbreviations that make no sense. I am introducing a completely new programming language that uses C++ syntax, with the difference that it uses exclusively Czech expressions (except for necessary punctuation). Google Translate (© Google Translate) was used for the translation, so please direct any complaints to © Google LLC.

This project is a source-to-source compiler (transpiler)

It also includes a simple editor to simplify working with code, its compilation and execution

In simple terms:
1. Reads code written in a custom language
2. Translates it to valid C++
3. Generates a `.cpp` file
4. Optionally compiles and runs the generated C++

## Architecture

- `Lexer`: splits input into lines/tokens
- `Translator`: maps custom keywords to C++ and transforms statements
- `CodeGenerator`: wraps translated lines into a valid C++ program
- `Executor`: optional compile and run step using `g++`

## Custom Keywords

The mapping is in `src/Translator.cpp` (constructor of `Translator`).

Comprehensive keyword table see below

## Installation and Setup (Windows)

### Supported Operating Systems

- **Windows 10/11 (x86_64)** - Full support (recommended)
- **Linux** - Partial support (TUI editor, CLI transpiler)
- **macOS** - In progress

### Requirements

- **CMake** version 3.16 or higher
- **C++ Compiler** (MinGW 32-bit or LLVM-MinGW 64-bit)
- **Git** (for cloning the repository)
- **PowerShell** 5.1 or higher (for Windows)

### Step 1: Install CMake

1. Go to https://cmake.org/download/
2. Download **CMake 3.28.0** or newer for Windows (64-bit)
3. Run the installer and select: **"Add CMake to the system PATH"** (important)
4. Verify installation in PowerShell:
   ```powershell
   cmake --version
   ```

### Step 2: Install LLVM-MinGW Compiler

1. Visit https://github.com/mstorsjo/llvm-mingw/releases
2. Download the latest **`llvm-mingw-*.ucrt-x86_64.zip`** (approximately 1.5 GB)
   - Recommended: version 20260421 or newer
3. Extract the ZIP file to a simple path without special characters, e.g.:
   ```
   C:\llvm-mingw
   ```
   or
   ```
   C:\Users\{YourUsername}\AppData\Local\llvm-mingw
   ```
4. Verify installation by opening PowerShell and running:
   ```powershell
   C:\llvm-mingw\bin\g++.exe --version
   ```
   You should see LLVM version information.

### Step 3: Clone the Repository

```powershell
cd C:\Users\{YourUsername}\Documents
git clone https://github.com/your-username/CeskySyntax.git
cd CeskySyntax
```

### Step 4: Configure Environment

1. Open PowerShell in the project directory
2. Run the following commands (replace the path with your installation):

```powershell
# Set path to compiler
$env:PATH = "C:\Program Files\CMake\bin;C:\llvm-mingw\bin;" + $env:PATH

# Verify that g++ is available
g++ --version
```

### Step 5: Configure Project (CMake)

```powershell
# In the CeskySyntax directory
cmake -S . -B build-mingw -G "Unix Makefiles"
```

This command creates a `build-mingw` directory with configuration files.

### Step 6: Build

For the CLI transpiler:
```powershell
cmake --build build-mingw --target cesky_transpiler
```

For the GUI editor (recommended for users):
```powershell
cmake --build build-mingw --target cesky_gui
```

For both applications:
```powershell
cmake --build build-mingw
```

**Time estimate:** First build takes 2-5 minutes (ImGui compiles for the first time), subsequent builds are faster.

### Step 7: Run the Application

#### GUI Editor (Recommended)

```powershell
.\build-mingw\cesky_gui.exe
```

The GUI opens in a window with:
- **Left panel**: Code editor with Czech syntax and highlighting
- **Right panel** (optional): Preview of the translated C++ code
- **Buttons**: Load example, Save, Transpile and run, Show/Hide preview
- **Bottom panel**: Program output and error messages

#### CLI Transpiler

```powershell
.\build-mingw\cesky_transpiler.exe examples\demo.csx generated.cpp
```

Or with automatic execution:
```powershell
.\build-mingw\cesky_transpiler.exe examples\demo.csx generated.cpp --run --compiler=g++.exe
```

### Step 8: First Test

1. Open the GUI (`cesky_gui.exe`)
2. Click the **"Ukázky"** button and select **"Základní cyklus"**
3. Click **"Přeložit a spustit"**
4. In the bottom panel you should see output: `0 1 2 3 4`

### Troubleshooting

**Error: "cmake: command not found"**
- Check that CMake is installed in `C:\Program Files\CMake\bin`
- Restart PowerShell and try again

**Error: "g++: command not found"**
- Verify that LLVM-MinGW is extracted in `C:\llvm-mingw`
- Run: `$env:PATH = "C:\llvm-mingw\bin;" + $env:PATH` in PowerShell

**Error: "undefined reference to" during linking**
- Make sure you compile with `cmake --build build-mingw`, not manually
- Delete `build-mingw` and reconfigure: `cmake -S . -B build-mingw -G "Unix Makefiles"`

**Error: Characters are displayed incorrectly in GUI (instead of "č" a different character appears)**
- This is normal if the system doesn't have the correct font installed
- GUI should automatically load Segoe UI or Consolas

**Error: "Permission denied" during compilation**
- Close `cesky_gui.exe` if it's running
- Try again: `cmake --build build-mingw --target cesky_gui`

### Optional: Add Environment Setup to PowerShell Profile

To avoid setting up `$env:PATH` every time, add to your PowerShell profile:

```powershell
$profile
```

Open the file in a text editor and add:
```powershell
# CeskySyntax Setup
$env:PATH = "C:\Program Files\CMake\bin;C:\llvm-mingw\bin;" + $env:PATH
```

Next time you open PowerShell, the environment will be configured automatically.

---

## Quick Build (For Those Who Already Have Everything Set Up)

### CMake

```bash
cmake -S . -B build-mingw -G "Unix Makefiles"
cmake --build build-mingw
```

## GUI Editor (Dear ImGui)

The GUI editor is the recommended way to use CeskySyntax. It offers:

- **Syntax-highlighted editor** - Support for all Czech keywords with coloring
- **C++ Preview** - See the generated C++ code next to your source code
- **Examples** - Pre-defined examples (Basic loop, FizzBuzz, Sum and average, etc.)
- **Execution** - Transpile, compile and run code in one click
- **Save/Load** - Save your code and load it later
- **Automatic `int main()`** - Optionally automatically wrap code in `int main()` function

### Running the GUI

After compiling (see above), simply run:

```powershell
.\build-mingw\cesky_gui.exe
```

The GUI depends on ImGui, GLFW and other libraries that are already included in `third_party/` as vendored libraries.

### GUI Controls

1. **"Load example code" button** - Loads the last saved code
2. **"Save" button** - Saves the current code
3. **"Ukázky" (Examples) button** - Offers a list of examples
4. **"Transpile and run" button** - Transpiles, compiles and runs the code
5. **"Hide/Show C++ preview" button** - Toggles the preview of generated C++
6. **"Run last compiled code" button** - Runs the last compiled output without transpiling again
7. **"Add int main() automatically" checkbox** - Determines whether `int main()` wrapper is added automatically
8. **Top panel** - Syntax highlighting and code editing
9. **Right panel** - Preview of generated C++ code (if enabled)
10. **Bottom panel** - Program output and error messages

---

## CLI Transpiler Usage

```bash
./build-mingw/cesky_transpiler.exe examples/demo.csx generated.cpp
```

Optionally compile and run the generated C++:

```bash
./build-mingw/cesky_transpiler.exe examples/demo.csx generated.cpp --run --compiler=g++.exe
```

## TUI Editor

```powershell
# Run terminal editor with environment setup
$toolbin = "C:\llvm-mingw\bin"
$env:CESKYSYNTAX_TOOLBIN = $toolbin
$env:CESKYSYNTAX_COMPILER = "g++.exe"
$env:PATH = "$toolbin;$env:PATH"
.\build-mingw\cesky_editor.exe
```

Available Commands:
- `pridat` — Add code lines (can add multiple lines at once, press Enter twice to finish)
- `upravit <N>` — Edit line number N
- `smazat <N>` — Delete line number N
- `ukazat` — Show all lines
- `spustit` — Transpile, compile and execute code
- `spustitexe` — Run the last generated executable (without recompilation)
- `smazatvse` — Clear all code
- `ukoncit` — Close the editor

---

## Example Input (`.csx`) (nice readable code)

```txt
celé číslo x rovná se 0
zatímco x menší než 5 {
    norma::znakový výstup << x << norma::koncová čára
    x = x plus 1
}
když x rovná se? 5 {
    norma::znakový výstup << (x krát 10) << norma::koncová čára
} jinak {
    norma::znakový výstup << 0 << norma::koncová čára
}
```

## Example Generated C++ (complex C++ full of abbreviations and English expressions)

```cpp
#include <iostream>
#include <string>

int main() {
    int x = 0;
    while (x < 5) {
    std::cout << x << std::endl;
    x = x + 1;
    }
    if (x == 5) {
    std::cout << (x * 10) << std::endl;
    } else {
    std::cout << 0 << std::endl;
    }
    return 0;
}
```

## Keyword Translation Table

| Czech Keyword | C++ Equivalent | Category |
|---|---|---|
| celé číslo | int | Data Type |
| levituj | float | Data Type |
| krátké desetinné číslo | float | Data Type (variant) |
| dvojté | double | Data Type |
| dlouhé desetinné číslo | double | Data Type (variant) |
| charakter | char | Data Type |
| znak | char | Data Type (variant) |
| pravdivostní hodnota | bool | Data Type |
| prázdnota | void | Data Type |
| funkce | void | Data Type (variant) |
| provázek | string | Data Type |
| text | string | Data Type (variant) |
| pravda | true | Literal |
| falešný | false | Literal |
| nepravda | false | Literal (variant) |
| když | if | Control Flow |
| jinak kdyby | else if | Control Flow |
| jinak | else | Control Flow |
| přepínač | switch | Control Flow |
| věc | case | Control Flow |
| případ | case | Control Flow (variant) |
| výchozí | default | Control Flow |
| přerušení | break | Control Flow |
| pokračovat | continue | Control Flow |
| návrat | return | Control Flow |
| vrátit | return | Control Flow (variant) |
| pro | for | Control Flow |
| zatímco | while | Control Flow |
| dělat | do | Control Flow |
| třída | class | Object-Oriented |
| strukturovat | struct | Object-Oriented |
| veřejnost | public | Object-Oriented |
| veřejný | public | Object-Oriented (variant) |
| soukromé | private | Object-Oriented |
| soukromý | private | Object-Oriented (variant) |
| chráněný | protected | Object-Oriented |
| tento | this | Object-Oriented |
| viruální | virtual | Object-Oriented |
| přepsat | override | Object-Oriented |
| nový | new | Object-Oriented |
| smazat | delete | Object-Oriented |
| v řadě za sebou | inline | Modifier |
| jmenný prostor | namespace | Organization |
| pomocí | using | Organization |
| pokus | try | Exception Handling |
| chytit | catch | Exception Handling |
| hod | throw | Exception Handling |
| nulový ukazatel | nullptr | Pointer |
| norma | std | Namespace |
| norma::znakový výstup | std::cout | Output |
| začátek výpisu | std::cout | Output |
| znakový výstup | cout | Output |
| zadávání znaků | cin | Input |
| norma::koncová čára | std::endl | Output |
| konec výpisu | std::endl | Output |
| koncová čára | endl | Output |
| rovná se | = | Assignment Operator |
| plus | + | Arithmetic Operator |
| mínus | - | Arithmetic Operator |
| krát | * | Arithmetic Operator |
| děleno | / | Arithmetic Operator |
| zbytek po dělení | % | Arithmetic Operator |
| rovná se? | == | Comparison |
| nerovná se | != | Comparison |
| menší než | < | Comparison |
| větší než | > | Comparison |
| menší nebo rovno | <= | Comparison |
| větší nebo rovno | >= | Comparison |
| a zároveň | && | Logical |
| nebo | \|\| | Logical |
| není | ! | Logical |
