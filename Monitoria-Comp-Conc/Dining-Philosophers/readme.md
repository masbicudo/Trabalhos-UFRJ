Dining Philosophers
===================

Exemplos de códigos que funcionam e que não funcionam para resolver esse problema.

A forma mais fácil de rodar os exemplos é usando alguma plataforma online como:
- **[repl.it](https://repl.it):** basta copiar o código do exemplo e colar no site, em um arquivo `cpp`. Neste site, o `c` não oferece acesso à biblioteca pthread, que é necessária, mas o `cpp` oferece acesso.

    Os exemplos desse projeto podem ser acessador nesses links:

    - https://repl.it/@masbicudo/diningdeadlock1
    - https://repl.it/@masbicudo/diningstarve1
    - https://repl.it/@masbicudo/diningworking1
    - https://repl.it/@masbicudo/diningworking2

WSL (Windows Subsystems for Linux)
==================================

Os códigos foram feitos para compilar no Linux, mas funcionam perfeitamente dentro do WSL.

O WSL permite usar um ambiente Linux dentro do Windows. No caso deste projeto, estou usando WSL + Ubuntu. Instalei o Ubuntu pela Windows Store.

Parece funcionar tudo perfeitamente, inclusive a lib pthread.

Como fazer
----------

- [Windows Subsystem for Linux](https://github.com/Microsoft/vscode-cpptools/blob/master/Documentation/LanguageServer/Windows%20Subsystem%20for%20Linux.md): explica com fazer para configurar o VSCode para editar código C++ com auto-complete usando bibliotecas de dentro do WSL.

- [Windows 10's Windows Subsystem for Linux](https://github.com/Microsoft/vscode-cpptools/blob/master/Documentation/Debugger/gdb/Windows%20Subsystem%20for%20Linux.md): explica como fazer para debugar programas rodando dentro do WSL.

