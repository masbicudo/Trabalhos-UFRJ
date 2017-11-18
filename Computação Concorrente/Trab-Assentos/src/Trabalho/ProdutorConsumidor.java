package Trabalho;

/**
 * Classe de sincronização no padrão produtor consumidor.
 * Essa classe possui um método de cancelamento, o que permite
 * cancelar a linha de produção, mas ainda assim, sempre terminando
 * de consumir todos os itens pendentes, já produzidos,
 * ou em processo de produção.
 */
public class ProdutorConsumidor {
    private int espacos_livres;
    private int espacos_ocupados;
    private final Object lockLivres = new Object();
    private  final Object lockOcupados = new Object();
    private boolean cancelado = false;
    public final int capacidade;

    public ProdutorConsumidor(int espacos_livres, int espacos_ocupados) {
        this.espacos_livres = espacos_livres;
        this.espacos_ocupados = espacos_ocupados;
        this.capacidade = espacos_livres + espacos_ocupados;
    }

    /**
     * Método que indica o cancelamento da linha de produção.
     * Itens já produzidos ou em produção serão todos processados pelos consumidores.
     */
    public void cancelar() {
        // A ordem desses locks é IMPORTANTE!
        // No método queroConsumir, eventualmente
        // o lockLivres também será lockado dentro
        // do lockOcupados. Locks em ordens inversas
        // em dois ou mais métodos podem causar
        // dead-lock.
        synchronized (lockOcupados) {
            synchronized (lockLivres) {

                // Cancelando e impedindo novos produtores
                // de iniciarem o processo de produção.
                // Todos que estiverem esperando uma oportunidade
                // para produzir vão ser avisados sobre o cancelamento.
                cancelado = true;
                lockLivres.notifyAll();

                // Se nem todos os espaços estão livres
                // quer dizer que tem algum produtor que
                // está no meio do processo de produção,
                // e logo irá liberar algo para o consumidor
                // chamando lockOcupados.notifyAll...
                // já que o produtor vai fazer isso,
                // não é necessário fazer isso aqui.
                // Sem isso, também funciona, pois o método
                // queroConsumir irá verificar isso também.
                // Mas pelo menos economizamos tempo de
                // processamento.
                if (espacos_livres == capacidade)
                    lockOcupados.notifyAll();
            }
        }
    }

    /**
     * Método que indica que o produtor está prestes a produzir algo.
     * Se não houverem espaços livres no estoque então o método fica esperando.
     * Quando o método retornar, o valor booleano indica se pode ou não produzir.
     */
    public boolean queroProduzir() {
        synchronized(lockLivres){
            if (cancelado)
                return false;

            while(espacos_livres == 0) {
                try {

                    lockLivres.wait();

                    if (cancelado)
                        return false;

                } catch (InterruptedException e) {
                    e.printStackTrace();
                    return false;
                }
            }

            espacos_livres--;
            return true;
        }
    }

    /**
     * Método que indica que o produtor acabou de produzir algo.
     * Isso aumenta o número de itens prontos no estoque, liberando para quem quiser pegar.
     */
    public void jaProduzi() {
        synchronized (lockOcupados) {
            espacos_ocupados++;
            lockOcupados.notifyAll();
        }
    }

    /**
     * Método que indica que o produtor desistiu de produzir algo.
     * Isso aumenta o número de espaços vazios no estoque, liberando para quem quiser colocar algo lá.
     */
    public void naoProduzi() {
        // o efeito é igual ao de terminar de consumir um item
        // ou seja, ele produz e o fato de desistir é o mesmo que
        // ele mesmo consumir
        jaConsumi();
    }

    /**
     * Método que indica que o consumidor desistiu de consumir algo.
     * Isso aumenta o número de itens prontos no estoque, liberando para quem quiser pegar.
     */
    public void naoConsumi() {
        // o efeito é igual ao de terminar de produzir um item
        // ou seja, ele tava consumindo mas desistiu e colocou de volta no estoque
        jaProduzi();
    }

    /**
     * Método que indica que o consumidor está querendo um item para processar.
     * Se não houverem itens prontos no estoque então o método fica esperando.
     * Quando o método retornar, o valor booleano indica se pode ou não consumir.
     */
    public boolean queroConsumir(IPredicado condicaoExterna) {
        synchronized(lockOcupados){

            if (cancelado)
                synchronized(lockLivres) {
                    if (espacos_livres == capacidade)
                        return false;
                }

            while(espacos_ocupados == 0 && (condicaoExterna == null || condicaoExterna.verificar())){

                try {

                    lockOcupados.wait();

                    if (cancelado)
                        synchronized(lockLivres) {
                            if (espacos_livres == capacidade)
                                return false;
                        }

                } catch (InterruptedException e) {
                    e.printStackTrace();
                    return false;
                }
            }

            espacos_ocupados--;
            return true;
        }
    }

    /**
     * Método que indica que o consumidor acabou de consumir um item.
     * Isso aumenta o número de espaços vazios no estoque, liberando para quem quiser colocar algo lá.
     */
    public void jaConsumi() {
        synchronized (lockLivres) {
            espacos_livres++;
            lockLivres.notifyAll();
        }
    }
}
