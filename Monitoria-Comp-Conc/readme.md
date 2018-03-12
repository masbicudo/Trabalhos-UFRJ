Computação Concorrente
======================

Essa disciplina explora a capacidade dos sistemas operacionais e processadores de executar várias rotinas ao mesmo tempo,
sincronizadas entre si de forma que os recursos necessários possam ser compartilhados sem conflitos,
e também que essas rotinas possam se comunicar de forma a produzir um resultado em conjunto.

Problemas
---------

- deadlock: ocorre quando as rotinas ficam esperando algo acontecer, que possui dependência em um evento que ela mesma ainda vai disparar

    - A espera B fazer algo, mas B também está esperando A fazer algo

- starvation: ocorre quando um recurso sempre está ocupado, toda vez que a rotina vai tentar utilizá-lo

    - por falta de sincronia: a rotina sempre pede para usar o recurso na hora errada, se fosse um pouco antes ou um pouco depois ela conseguiria
    - por falta de prioridade: a rotina possui baixa prioridade, sendo que outras de alta prioridade estão sempre utilizando o recurso desejado

Questão
-------

Como criar uma disciplina entre as rotinas de forma que todas possam eventualmente utilizar os recursos e produzir em conjunto um resultado útil?

Rodando os exemplos
===================

A forma mais fácil de rodar os exemplos é usando alguma plataforma online como:

- **[Coding Ground](https://www.tutorialspoint.com/codingground.htm):** é uma ferramenta avançada que permite a criação de projetos online. A interface é bem simples e direta ao ponto. Permite editar o comando de compilação, o que facilita incluir bibliotecas como a pthread. O shell é totalmente interativo, permitindo dar entrada para o programa assim como ver a saída durante a execução.

- **[repl.it](https://repl.it):** basta copiar o código do exemplo e colar no site, em um arquivo `cpp`. Neste site, o `c` não oferece acesso à biblioteca pthread, que é necessária, mas o `cpp` oferece acesso. A ferramente é semi-interativa, pois toda a entrada tem que ser predefinida. Entretanto, a saída é mostrada durante a execução do programa, o que para este caso é o suficiente.

- **[Rextester](http://rextester.com/main):** ferramenta de codificação colaborativa, dá pra ver o cursor de todos que estiverem editando ao mesmo tempo. Infelizmente a ferramente não é interativa, ou seja, a entrada e saída do programa não aparecem durante a execução. Toda a entrada tem que ser predefinida, e toda a saída é dada de uma única vez ao término do programa.

WSL (Windows Subsystems for Linux)
==================================

Os códigos foram feitos para compilar no Linux, mas funcionam perfeitamente dentro do WSL.

O WSL permite usar um ambiente Linux dentro do Windows. No caso deste projeto, estou usando WSL + Ubuntu. Instalei o Ubuntu pela Windows Store.

Parece funcionar tudo perfeitamente, inclusive a lib pthread.

Como fazer
----------

- [Windows Subsystem for Linux](https://github.com/Microsoft/vscode-cpptools/blob/master/Documentation/LanguageServer/Windows%20Subsystem%20for%20Linux.md): explica com fazer para configurar o VSCode para editar código C++ com auto-complete usando bibliotecas de dentro do WSL.

- [Windows 10's Windows Subsystem for Linux](https://github.com/Microsoft/vscode-cpptools/blob/master/Documentation/Debugger/gdb/Windows%20Subsystem%20for%20Linux.md): explica como fazer para debugar programas rodando dentro do WSL.
