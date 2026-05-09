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

## Kompilace

### CMake

```bash
cmake -S . -B build
cmake --build build
```

## Konfigurace Místního Toolchainu

Umístěte místní hodnoty toolchainu do souboru `.env` v kořeni projektu, na základě `.env.example`.

Potřebné položky:

- `CESKYSYNTAX_TOOLBIN` = adresář obsahující `g++.exe` a DLL knihovny LLVM-MinGW
- `CESKYSYNTAX_COMPILER` = příkaz kompilátoru, který používá transpilér, obvykle `g++.exe`

Editor TUI a skripty smoke-test budou tento soubor automaticky číst, pokud je přítomen.

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

## GUI (Dear ImGui) — Volitelně

Projekt byl od posledního commitu obohacen o nenáročný GUI editor uzpůsobený českému syntaxu.

editor je schopen:
- uložit kód
- přeložit a spustit kód
- spustit poslední přeložený soubor
- načíst poslední uložený kód/příklad kódu
- vybrat a načíst předpřipravené ukázky z tlačítka "Ukázky" vedle ostatních akcí

### Spuštění GUI

Závislosti (ImGui, GLFW, ImGuiColorTextEdit) jsou již zahrnuty v `third_party/` jako vendored knihovny.

Na Windows s MinGW:

```powershell
$env:PATH = "C:/Program Files/CMake/bin;C:/path/to/llvm-mingw/bin;" + $env:PATH
cmake -S . -B build-mingw -G "Unix Makefiles"
cmake --build build-mingw --target cesky_gui
.\build-mingw\cesky_gui.exe
```

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

## Build

### CMake

```bash
cmake -S . -B build
cmake --build build
```

## Local Toolchain Config

Place the local toolchain values in a `.env` file in the project root, based on `.env.example`.

Required entries:

- `CESKYSYNTAX_TOOLBIN` = directory containing `g++.exe` and LLVM-MinGW DLL libraries
- `CESKYSYNTAX_COMPILER` = compiler command used by the transpiler, usually `g++.exe`

The TUI editor and smoke-test scripts will read this file automatically when present.

## Run Transpiler

```bash
./build/cesky_transpiler examples/demo.csx generated.cpp
```

Optionally compile and run the generated C++:

```bash
./build/cesky_transpiler examples/demo.csx generated.cpp --run --compiler=g++
```

On Windows with MinGW in PATH replace `./build/cesky_transpiler` with the path to the executable generated by CMake.

## TUI Editor

Run the terminal editor with environment state:

```powershell
$toolbin = "C:\path\to\llvm-mingw\bin"
$env:CESKYSYNTAX_TOOLBIN = $toolbin
$env:CESKYSYNTAX_COMPILER = "g++.exe"
$env:PATH = "$toolbin;$env:PATH"
./build/cesky_editor.exe
```

Commands:
- `pridat` — Adding code lines (possible to add multiple lines at once, press Enter 2 times to finish)
- `upravit <N>` — Editing line number N
- `smazat <N>` — Deletion of line number N
- `ukazat` — Show all lines
- `spustit` — Transpilation, compilation and execution of code
- `spustitexe` — Running the last generated file (without recompilation)
- `smazatvse` — Deletion of all code
- `ukoncit` — Closing the editor
- and a secret command that you can find in the code (it's a niche reference)

## GUI (Dear ImGui)

The project now includes a lightweight GUI editor tailored to Czech syntax. The editor is capable of:
- Saving code
- Transpiling and running code
- Running the last compiled file
- Loading previously saved code/example code
- Picking from premade examples in an `Ukázky` menu

### Run GUI

Dependencies (ImGui, GLFW, ImGuiColorTextEdit) are already included in `third_party/` as vendored libraries.

On Windows with MinGW:

```powershell
$env:PATH = "C:/Program Files/CMake/bin;C:/path/to/llvm-mingw/bin;" + $env:PATH
cmake -S . -B build-mingw -G "Unix Makefiles"
cmake --build build-mingw --target cesky_gui
.\build-mingw\cesky_gui.exe
```

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
