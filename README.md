## Snake And Rat game

-   Language Used : C++
-   Libraries Used : libsfml
-   OS : Linux (Ubuntu/Debian Based)

### How to Execute?

```bash
~> make
```

-   will generate the executable

```bash
~> make install
```

-   will install the required libraries/language support

```bash
~> make clean
```

-   will delete the generated executable

[Note]
There is also a `compile-game.sh` file present, which can also be used to generate executable.

```bash
~> ./compile-game.sh main.cpp # compile and generate executable (USES CLANG++)
~> ./main # to run the game
```

For clang Support :
Install clang++ on Ubuntu/Debian Based Systems:

```bash
    wget https://apt.llvm.org/llvm.sh
    chmod u+x llvm.sh
    sudo ./llvm.sh 17 # clang version 17
    clang++ --version
```
