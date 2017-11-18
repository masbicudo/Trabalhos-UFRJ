package Trabalho;

/**
 * Classe Thread 1 representa o primeiro conjunto de ações.
 */
public class UsuarioComum extends UsuarioThread {
    Assentos assentos;

    public UsuarioComum(int usuarioId, Assentos assentos){
        super(usuarioId);

        this.assentos = assentos;
    }

    public void run() {
        int usuarioId = this.usuarioId;

        boolean temNamorada = random.nextBoolean() && random.nextBoolean();

        // o usuário vai tentar no máximo 5 vezes antes de desistir
        // ele é bem paciente
        for (int tentativa = 0; tentativa < 5; tentativa++) {

            // usuário chegou na tela de selecionar assentos
            // e vai visualizar o mapa de alocação de todos os assentos
            EstadoAssento[] estadoAssentos = assentos.vizualizaAssentos(usuarioId);

            // 1/4 de chance do usuário clicar errado
            boolean usuarioClicouErrado = random.nextBoolean() && random.nextBoolean();

            int assentoEscolhido;
            if (usuarioClicouErrado) {
                // qual assento o usuário vai escolher, depende do predicado passado,
                // se houverem várias possibilidades, então uma aleatória dentre as
                // possíveis será escolhida... neste caso, o usuário irá pré-selecionar
                // apenas assentos que estiverem livres
                assentoEscolhido = escolherAssento(
                        assentos,
                        assento -> assento.getStatus(usuarioId) == EstadoAssento.OcupadoPorDesconhecido);
            } else {
                // qual assento o usuário vai escolher, depende do predicado passado,
                // se houverem várias possibilidades, então uma aleatória dentre as
                // possíveis será escolhida... neste caso, o usuário irá pré-selecionar
                // apenas assentos que estiverem livres
                assentoEscolhido = escolherAssento(
                        assentos,
                        assento -> assento.getStatus(usuarioId) == EstadoAssento.Desocupado);
            }

            if (assentoEscolhido == -1) {
                // minha nossa... o lugar está cheio
                // o usuário vai esperar um pouco antes de tentar de novo
                // vai que alguém desiste
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                continue;
            }

            if (assentos.alocaAssentoDado(assentoEscolhido, usuarioId)) {
                // consegui alocar

                // o usuário agora deseja verificar se seu assento realmente está alocado
                estadoAssentos = assentos.vizualizaAssentos(usuarioId);

                if (estadoAssentos[assentoEscolhido] != EstadoAssento.OcupadoPorMim) {
                    // isso não deve ser possível de acontecer,
                    // mas se acontecer o usuário fica enfurecido
                    // e vai embora
                    return;
                }

                // talvez o usuário queira escolher um assento para a namorada dele
                // e esse assento tem que ser bem do lado dele
                if (temNamorada) {
                    final int meuAssento = assentoEscolhido;
                    int assentoNamorada = escolherAssento(
                            assentos,
                            new IEscolhedor() {
                                @Override
                                public boolean verificar(Assento assento) {
                                    return assento.getStatus(usuarioId) == EstadoAssento.Desocupado
                                            && Math.abs(assento.getId() - meuAssento) == 1;
                                }
                            });

                    if (assentoNamorada == -1) {
                        // nenhum disponível ao lado dele
                        // tem que cancelar o dele e começar de novo
                        assentos.liberaAssento(assentoEscolhido, usuarioId);

                        // usuário vai tentar um outro lugar diferente
                        continue;
                    }

                    if (assentos.alocaAssentoDado(assentoNamorada, usuarioId)) {
                        // assento da namorada foi alocado

                        // o usuário agora deseja verificar se seu assento realmente está alocado
                        estadoAssentos = assentos.vizualizaAssentos(usuarioId);

                        if (estadoAssentos[assentoNamorada] != EstadoAssento.OcupadoPorMim) {
                            // isso não deve ser possível de acontecer,
                            // mas se acontecer o usuário fica enfurecido
                            // e vai embora

                            assentos.liberaAssento(assentoEscolhido, usuarioId);
                            return;
                        }

                        boolean usuarioMudouDeIdeia = random.nextBoolean() && random.nextBoolean();
                        if (usuarioMudouDeIdeia) {
                            assentos.liberaAssento(assentoEscolhido, usuarioId);
                            assentos.liberaAssento(assentoNamorada, usuarioId);
                            continue;
                        }

                        // usuário terminou e foi embora satisfeito
                        return;
                    }
                }
            }
            else {
                // não consegui alocar
                // o usuário vai tentar novamente então
                continue;
            }
        }
    }
}
