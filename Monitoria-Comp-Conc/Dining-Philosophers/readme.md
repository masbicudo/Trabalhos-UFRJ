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

A resposta requer que os filósofos sejam capazes de se organizar usando alguma forma de [exclusão mútua](https://en.wikipedia.org/wiki/Mutual_exclusion),
pois mais de um filósofo não pode pegar o mesmo talher.

Ambiente
========

[Instruções para Computação Concorrente](../readme.md)

Códigos interativos online
==========================

Os exemplos interativos desse projeto podem ser acessador nesses links:

- https://repl.it/@masbicudo/diningworking1
- https://repl.it/@masbicudo/diningworking2
- https://repl.it/@masbicudo/diningdeadlock1
- https://repl.it/@masbicudo/diningstarve1
