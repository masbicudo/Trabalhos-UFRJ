package Trabalho;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class UsuarioThread extends Thread {

    protected final int usuarioId;
    protected static final Random random = new Random();

    public UsuarioThread(int usuarioId) {

        this.usuarioId = usuarioId;
    }

    /**
     * Metodo representa a segunda acao que uma thread pode ter sobre o objeto Assentos
     * Neste metodo é alocado um assento aleatório livre, se disponível.
     * O metodo  e realizado ao obter o lock de Leitura sobre a classe Assentos e no Lock (no monitor) de produtor e consumidor
     * para o logger.
     * @param predicado identificador de quem tenta alocar o assento
     * @return 0 caso nao tenham assentos livres, 1 para caso alocado.
     */
    public int escolherAssento(Assentos assentos, IEscolhedor predicado) {
        while (true) {
            List<Integer> lista = selecionarAssentosPorPredicado(assentos, predicado);

            // Sem assentos vazios retorna 0
            if (lista.size() != 0) {
                // gera indice aleatório para consulta na lista de vazios.
                int indice = Math.abs(random.nextInt() % lista.size());

                // Sequência de alocacao e escrita no buffer.
                Assento assentoEscolhido = assentos.assentos[lista.get(indice)];
                return assentoEscolhido.getId();
            }

            return -1;
        }
    }

    /**
     * metodo cria uma lista com os index em Assentos[] de assentos que possuem status 0 (vazios)
     * @return Lista com os index de assentos vazios.
     */
    private static List<Integer> selecionarAssentosPorPredicado(Assentos assentos, IEscolhedor predicado) {
        List<Integer> lista = new ArrayList<Integer>();
        for (int i = 0; i < assentos.length(); i++) {
            if (predicado.verificar(assentos.assentos[i])) {
                lista.add(i);
            }
        }
        return lista;
    }
}
