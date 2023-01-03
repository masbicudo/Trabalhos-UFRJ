package Trabalho;

import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * Classe assento representa uma unidade alocavel.
 * Classe nao sincronizada.
 */


public class Assento {
    ReentrantReadWriteLock lock;
    private int id;
    private boolean estaReservado;
    private int owner;

    public void queroLer() {
        lock.readLock().lock();
    }

    public void acabeiDeLer() {
        lock.readLock().unlock();
    }

    public void queroEscrever() {
        lock.writeLock().lock();
    }

    public void acabeiDeEscrever() {
        lock.writeLock().unlock();
    }

    /**
     * Costrutor da Classe , recebe parametro id que 'e um identificador unico do asento.
     * @param id identificador unico do asento.
     */
    public Assento( int id ){
        lock = new ReentrantReadWriteLock();
        this.id = id;
        estaReservado = false;
        owner = 0;
    }

    /**
     * Metodo de reserva concorrente. Esste metodo recebe o lock de escrita da Classe assentos e o monitor de escrita no buffer
     * de logger. Metodo garante o lock de Escrita e o lock de Produtor, porem o unlock fica por conta do metodo chamador.
     * @param owner identificador de quem solicita a reserva (threadId).
     * @return False para nao alocado e true caso alocado com suceso.
     */
    public boolean reserva ( int owner ) {
        queroEscrever();
        try {
            if (estaReservado) {
                return false;
            }
            else {
                this.estaReservado = true;
                this.owner = owner;
                return true;
            }
        }
        finally {
            acabeiDeEscrever();
        }
    }

    /**
     * Metodo de obtencao de id de um assento. Metodo nao sicronizado, para uso em concorrencia o metodo chamador deve garantir
     * o sicronismo
     * @return id
     */
    public int getId(){
        queroLer();
        try {
            return id;
        } finally {
            acabeiDeLer();
        }
    }

    /**
     * Metodo de obtencao do owner de um assento. Metodo nao sicronizado, para uso em concorrencia o metodo chamador deve garantir
     * o sicronismo
     * @return owner
     */
    public int getOwner() {
        queroLer();
        try {
            return owner;
        } finally {
            acabeiDeLer();
        }
    }

    /**
     * Metodo de obtencao do status de um assento. Metodo nao sicronizado, para uso em concorrencia o metodo chamador deve garantir
     * o sicronismo
     * @return status
     */
    public EstadoAssento getStatus(int usuarioId) {
        queroLer();
        try {
            return !estaReservado ? EstadoAssento.Desocupado
                    : owner == usuarioId ? EstadoAssento.OcupadoPorMim
                    : EstadoAssento.OcupadoPorDesconhecido;
        }
        finally {
            acabeiDeLer();
        }
    }

    /**
     * Metodo de cancelamento de uma reserva. Esste metodo recebe o lock de escrita da Classe assentos e o monitor de escrita no buffer
     * de logger. Metodo garante o lock de Escrita e o lock de Produtor, porem o unlock fica por conta do metodo chamador.
     * @param owner identificador de quem solicita a reserva (threadId).
     * @return False para nao cancelado e true caso canselado com suceso.
     */
    public boolean cancela(int owner) {
        queroEscrever();
        try {
            if (estaReservado && this.owner == owner) {
                this.estaReservado = false;
                this.owner = 0;
                return true;
            } else {
                return false;
            }
        }
        finally {
            acabeiDeEscrever();
        }
    }

    /**
     * Metodo de print de um assento. Nesta execussao so e utilizado para debuger.
     * @return
     */
    public String toString(){
        String aux;
        if (estaReservado){
            aux = "Reservado por :" + getOwner();
        }
        else
            aux = "Livre";

        return "O assento de número " + id + " encontra-se " + aux + "\n";
    }

    /**
     * Metodo de alteracao de id, usado como auxilio para criacao de assento (em codigo sequencial) fora da clase assento.
     * @param id id que o assento deve ter apos a execussao do metodo.
     */
    public void setId(int id) {
        queroEscrever();
        try {
            this.id = id;
        }
        finally {
            acabeiDeEscrever();
        }
    }
}
