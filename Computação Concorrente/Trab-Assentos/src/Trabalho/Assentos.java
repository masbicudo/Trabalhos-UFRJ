package Trabalho;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * Classe que representa um conjunto de assentos.
 * Essa Classe e sincronizada.
 */
public class Assentos {
    ReentrantReadWriteLock lock = new ReentrantReadWriteLock(true);
    Assento assentos[];
    Logger log;

    /**
     * Contrutor da casse.
     * @param quant quantidade de assentos a serem alocados
     * @param log objeto de controle do padrão protutor/conumidor para escrita de LOG.
     */
    public Assentos(int quant, Logger log) {
        this.log = log;
        assentos = new Assento[quant];
        for (int i = 0; i < quant; i++)
            assentos[i] = new Assento(i);
    }

    private static String geraMapaDeOcupacao(int[] mapaLog) {
        String msg = "[";
        for (int i = 0; i < mapaLog.length; i++)
            msg += (i > 0 ? "," : "") + mapaLog[i];
        msg += "]";
        return msg;
    }

    /**
     * Metodo representa a primeira operacao (visualizar acentos livres) que uma thread pode fazer.
     * O metodo  e realizado ao obter o lock de Leitura sobre a classe Assentos e no Lock (no monitor) de produtor e consumidor
     * para o logger.
     * @param usuarioId id entificacao de quem chama o metodo
     */
    public EstadoAssento[] vizualizaAssentos(int usuarioId) {
        String cod_ope = "V";

        // lendo todos os valores
        // seguindo o padrão de locks encadeados
        // apesar de ser leitura, vamos usar o
        // lock de escrita, pois ele permite apenas um por vez
        EstadoAssento[] mapa = new EstadoAssento[length()];
        int[] mapaLog = new int[length()];
        Slot res[] = new Slot[1];
        OperadorEncadeadoDeAssentos enc = new OperadorEncadeadoDeAssentos() {
            @Override
            public void processa(int indice, Assento assento) {
                mapa[indice] = assento.getStatus(usuarioId);
                mapaLog[indice] = assento.getStatus(usuarioId) == EstadoAssento.Desocupado ? 0 : assento.getOwner();
                if (indice == assentos.length-1)
                    res[0] = log.iniciarEscrita();
            }
        };
        enc.executar(this);

        String msg = cod_ope + ", " + usuarioId + " " + geraMapaDeOcupacao(mapaLog);
        if (res[0] != null) {
            if (!log.finalizarEscrita(res[0], msg))
                log.desisteEscrita(res[0]);
        }

        return mapa;
    }

    /**
     *  Metodo representa a terceira acao que uma thread pode ter sobre o objeto Assentos
     * Neste metodo e alocado um assento dado se disponivel.
     * O metodo  e realizado ao obter o lock de Leitura sobre a classe Assentos e no Lock (no monitor) de produtor e consumidor
     * para o logger.
     * @param assentoId repreenta exatamente o assento que deve ser alocado
     * @param usuarioId usuario que deseja alocar o assento.
     * @return retorna 1 se alocado e 0 se indisponivel
     * @throws Exception Se thread chamadora nao esta ecrita como onwer do assento a tentar ser alocado
     */
    public boolean alocaAssentoDado(int assentoId, int usuarioId) {
        String cod_ope = "A";

        EstadoAssento[] mapa = new EstadoAssento[length()];
        int[] mapaLog = new int[length()];
        Assento reservados[] = new Assento[1];
        Slot res[] = new Slot[1];
        OperadorEncadeadoDeAssentos enc = new OperadorEncadeadoDeAssentos() {
            @Override
            public void processa(int indice, Assento assento) {
                if (assento.getId() == assentoId) {
                    boolean conseguiReservar = assento.reserva(usuarioId);
                    if (conseguiReservar)
                        reservados[0] = assento;
                }

                mapa[indice] = assento.getStatus(usuarioId);
                mapaLog[indice] = assento.getStatus(usuarioId) == EstadoAssento.Desocupado ? 0 : assento.getOwner();
                if (indice == assentos.length-1)
                    res[0] = log.iniciarEscrita();
            }
        };
        enc.executar(this);

        Assento reservado = reservados[0];
        if (reservado != null) {
            String msg = cod_ope + ", " + usuarioId + ", " + reservado.getId() + " " + geraMapaDeOcupacao(mapaLog);
            if (res[0] != null) {
                if (!log.finalizarEscrita(res[0], msg))
                    log.desisteEscrita(res[0]);
            }
            return true;
        }
        log.desisteEscrita(res[0]);

        return false;
    }

    /**
     * Metodo representa a quarta ação que uma thread pode ter sobre o objeto Assentos
     * Neste metodo e liberado um assento se ele pertence a thread chamadora.
     * O metodo  e realizado ao obter o lock de Leitura sobre a classe Assentos e no Lock (no monitor) de produtor e consumidor
     * para o logger.
     * @param assentoId assento previamente alocado.
     * @param usuarioId identificador de thread chamadora
     * @return 1 se liberado, 0 se nao (assento nao estava alocado pela tread chamadora)
     * @throws Exception Se thread tenta liberar um assento que nao e dela.
     */
    public boolean liberaAssento(int assentoId, int usuarioId) {
        String cod_ope = "L";

        EstadoAssento[] mapa = new EstadoAssento[length()];
        int[] mapaLog = new int[length()];
        Assento cancelados[] = new Assento[1];
        Slot res[] = new Slot[1];
        OperadorEncadeadoDeAssentos enc = new OperadorEncadeadoDeAssentos() {
            @Override
            public void processa(int indice, Assento assento) {
                if (assento.getId() == assentoId) {
                    boolean conseguiReservar = assento.cancela(usuarioId);
                    if (conseguiReservar)
                        cancelados[0] = assento;
                }

                mapa[indice] = assento.getStatus(usuarioId);
                mapaLog[indice] = assento.getStatus(usuarioId) == EstadoAssento.Desocupado ? 0 : assento.getOwner();
                if (indice == assentos.length-1)
                    res[0] = log.iniciarEscrita();
            }
        };
        enc.executar(this);

        Assento cancelado = cancelados[0];
        if (cancelado != null) {
            String msg = cod_ope + ", " + usuarioId + ", " + cancelado.getId() + " " + geraMapaDeOcupacao(mapaLog);
            if (res[0] != null) {
                if (!log.finalizarEscrita(res[0], msg))
                    log.desisteEscrita(res[0]);
            }
            return true;
        }
        log.desisteEscrita(res[0]);

        return false;
    }

    /**
     * Metodo length para Clase Assentos
     * @return tamanho do vetor de assentos.
     */
    public int length() {
        return assentos.length;
    }
}
