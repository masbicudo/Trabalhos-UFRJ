Intro
=====

Este projeto é um trabalho da disciplina de Computação Concorrente, cujo objetivo é simular a seguinte situação:

Dado um conjunto de assentos, por exemplo, em um cinema, o sistema deve permitir que o usuário entre escolha um assento e reserve-o. Isso é feito usando 3 métodos: visualizar mapa, reservar assento e cancelar reserva. Vários usuários podem fazer essas operações simultaneamente.

Trabalho
========

Implementar o sistema descrito, e testar sua capacidade de atuar de forma concorrente. Para isso, deve-se simular vários usuários fazendo variás operações, em paralelo, sendo que cada operação gera uma entrada de Log. Esse Log deve apresentar as operações e o mapa de assentos de acordo com o resultado da operação.

Para a implementação do Log, a descrição do trabalho requer que seja implementado o padrão produto/consumidor. Ao final da simulação das operações, deve-se garantir que todas as mensagens sejam devidamente consumidas.

Depois de executada a simulação multiusuários, o Log deverá ser verificado, executando uma a uma as instruções registradas de forma linear. O resultado deve ser consistente.

Implementação
=============

Esta implementação faz uso dos seguintes padrões:

 - produtor/consumidor: implementado para dar saída no Log. Cada operação irá produzir uma mensagem para o Log, sendo que existe uma thread responsável pelo Log que atua como consumidor. Ao final do programa de simulação, para garantir o consumo de tudo, o produtor/consumidor aceita um método de cancelamento.

 
