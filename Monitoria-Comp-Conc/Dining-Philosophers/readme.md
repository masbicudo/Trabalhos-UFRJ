Jantar dos Filósofos (Dining Philosophers)
==========================================

Exemplos de códigos que funcionam e que não funcionam para resolver o problema do Jantar dos Filósofos.

Problema
--------

Imagine cinco filósofos que só pensam e comem, e uma mesa redonda, com cinco lugares e cinco talheres.
Cada talher é colocado entre dois lugares, de forma que cada filósofo tem acesso a dois talheres, um de cada lado.
Quando tiver fome, o filósofo deve pegar os talheres, um de cada vez e então pode comer de uma fonte infinita de comida.
Dois filósofos adjacentes não podem comer ao mesmo tempo, pois eles dividem o talher que está entre eles.

Questão
-------

Como criar uma disciplina entre os filósofos de tal forma que qualquer um deles possa, eventualmente, comer ao ficar com fome?

Rodando os exemplos
===================

A forma mais fácil de rodar os exemplos é usando alguma plataforma online como:
- **[repl.it](https://repl.it):** basta copiar o código do exemplo e colar no site, em um arquivo `cpp`. Neste site, o `c` não oferece acesso à biblioteca pthread, que é necessária, mas o `cpp` oferece acesso.

- **[Coding Ground](https://www.tutorialspoint.com/codingground.htm):** é uma ferramenta avançada que permite a criação de projetos online. A interface é bem simples e direta ao ponto. Permite editar o comando de compilação, o que facilita incluir bibliotecas como a pthread.

WSL (Windows Subsystems for Linux)
==================================

Os códigos foram feitos para compilar no Linux, mas funcionam perfeitamente dentro do WSL.

O WSL permite usar um ambiente Linux dentro do Windows. No caso deste projeto, estou usando WSL + Ubuntu. Instalei o Ubuntu pela Windows Store.

Parece funcionar tudo perfeitamente, inclusive a lib pthread.

Como fazer
----------

- [Windows Subsystem for Linux](https://github.com/Microsoft/vscode-cpptools/blob/master/Documentation/LanguageServer/Windows%20Subsystem%20for%20Linux.md): explica com fazer para configurar o VSCode para editar código C++ com auto-complete usando bibliotecas de dentro do WSL.

- [Windows 10's Windows Subsystem for Linux](https://github.com/Microsoft/vscode-cpptools/blob/master/Documentation/Debugger/gdb/Windows%20Subsystem%20for%20Linux.md): explica como fazer para debugar programas rodando dentro do WSL.

Códigos interativos online
==========================

Os exemplos interativos desse projeto podem ser acessador nesses links:

- https://repl.it/@masbicudo/diningworking1
- https://repl.it/@masbicudo/diningworking2
- https://repl.it/@masbicudo/diningdeadlock1
- https://repl.it/@masbicudo/diningstarve1
