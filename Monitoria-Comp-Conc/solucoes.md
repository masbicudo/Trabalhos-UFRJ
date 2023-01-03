Não existe um guia fechado para solucionar problemas de deadlock.
Entretanto, existem algumas estratégias que podem funcionar:

- **combinação de recursos (versão para produtor-consumidor):** se vários recursos produzidos são necessários, ao invés de criar um objeto de sincronia
  para cada, crie um objeto para cada combinação de tudo que é necessário.

  Exemplo: thread 1 precisa dos recursos A, B e C; thread 2 dos recursos B, C e D:

  Thread 1: Ao invés de fazer: `wait( A ); wait( B ); wait( C )`, faça:

        wait( A_B_C )

  Thread 2: Ao invés de fazer: `wait( B ); wait( C ); wait( D )`, faça:
  
        wait( B_C_D )

  Na thread produtora, ela vai dar um sinal só, dependendo do que estiver no conjunto de itens:
  
        if (estoque contém A, B, C) signal( A_B_C )
        if (estoque contém B, C, D) signal( B_C_D )

- **ordenação de recursos:** mantenha uma lista da ordem em que os locks podem ser feitos

  Exemplo: thread 1 precisa de A e pode precisar de B; thread 2 precida de B e pode precisar de A

  Vamos dizer que a ordem seja A e depois B, então ambas as threads
  vão dar lock nessa ordem, sendo que a thread 2 sairá prejudicada, pois não conseguirá expressar o caráter opcional do lock sobre A:

        lock( A ); lock( B )

- **combinação de recursos (varsão para locks):** uma outra forma da combinação de recursos permite locks opcionais, mas requer uma quantidade de locks exponencial

  Exemplo: thread 1 precisa de A e pode precisar de B; thread 2 precida de B e pode precisar de A

  Thread 1:

        lock( A_B ); lock( A ) ... if(x) lock( B )

  Thread 2:

        lock( A_B ); lock( B ) ... if(x) lock( A )

  Exemplo 2: note que todas as combinações que envolvam o recurso na seção crítica devem ser lockados. Thread 1 => A { B { C } }; Thread 2 => C { A { B } };
  Thread 3 => B { A, C }

  Thread 1:

        lock( A_B_C ); lock( A_B ); lock( A_C ); lock( A )
        ...
        if(x) {
            lock( B_C ); lock( B )
            ...
            lock( C )
            ...
        }

  Thread 2:

        lock( A_B_C ); lock( A_C ); lock( B_C ); lock( C )
        ...
        lock( A_B ); lock( A )
        ...
        if(x) {
            lock( B )
            ...
        }

  Thread 3:

        lock( A_B ); lock( B_C ); lock( B )
        ...
        lock( A )
        ...
        unlock( A )
        lock( C )
        ...
        unlock( C )
        ...
