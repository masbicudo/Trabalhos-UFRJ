# Compiling

    gcc -I src/libs -I src/os src/os/*.c src/libs/*.c src/*.c -o a.exe -g

# Generating numbers

### Random Number Generators

- [Mersene Twister](https://en.wikipedia.org/wiki/Mersenne_Twister)
    - [Mersene Twister](https://www.cs.hmc.edu/~geoff/mtwist.html)
- [Permuted congruential generator](https://en.wikipedia.org/wiki/Permuted_congruential_generator)
    - [Implementation](http://www.pcg-random.org/)
    - [Example](https://riptutorial.com/c/example/1323/permuted-congruential-generator)

### Distributions

- [Generating Poisson-distributed random variables](https://en.wikipedia.org/wiki/Poisson_distribution#Generating_Poisson-distributed_random_variables)

### Online number generators

- [Random.org](https://www.random.org/)
- [Next prime number](https://www.dcode.fr/next-prime-number)

# Data Structures

- [uthash](http://troydhanson.github.io/uthash/index.html)
- [utarray](https://troydhanson.github.io/uthash/utarray.html)

# Console

- [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code)
- [Using ANSI escape codes on Windows, macOS and Linux terminals](https://solarianprogrammer.com/2019/04/08/c-programming-ansi-escape-codes-windows-macos-linux-terminals/)
- [Google Image search: printf color c windows GetStdHandle](https://www.google.com/search?q=printf+color+c+windows+GetStdHandle&tbm=isch)
- [Google Search: printf color c windows ENABLE_VIRTUAL_TERMINAL_PROCESSING](https://www.google.com/search?q=printf+color+c+windows+ENABLE_VIRTUAL_TERMINAL_PROCESSING)

# Debugging tools

- [Application Verifier](https://docs.microsoft.com/pt-br/windows/win32/win7appqual/application-verifier)

# Limitations

- No check after invoking malloc
- All queues support max amount of processes
