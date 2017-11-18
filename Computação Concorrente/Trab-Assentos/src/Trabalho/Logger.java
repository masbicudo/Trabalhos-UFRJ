package Trabalho;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

/**
 * Thread do Logger
 */
public class Logger extends Thread {
    ProdutorConsumidor prodcons_Log;
    File arquivo;
    FilaDeString fila;
    int id;

    /**
     * Construtor da thread
     *
     * @param name         nome do arquivo de log
     * @param id           identificador da thread
     * @param fila         buffer de escrita
     * @param prodcons_Log monitor de produtor e consumidor para escrita de log.
     * @throws IOException
     */
    public Logger(
            String name,
            int id,
            FilaDeString fila,
            ProdutorConsumidor prodcons_Log
    ) {

        this.prodcons_Log = prodcons_Log;
        if (name != null)
            this.arquivo = new File(name);
        this.id = id;
        this.fila = fila;
    }

    /**
     * Metodo run, Executa um loop infinito de retirada do buffer e ecrita no log.
     * Para finalizar, a thread principar deve chamar a um interrupt para essa thread
     * o monitor ao receber o interrupt retornara a condicao de break.
     */
    public void run() {
        // tentando criar o arquivo de log
        int i;
        for (i = 0; i < 5; i++) {
            //arquivo.mkdirs();
            try {
                arquivo.delete();
                boolean newFile = arquivo.createNewFile();
                if (newFile)
                    break;
            } catch (IOException e) {
                e.printStackTrace();
                try {
                    sleep(1000);
                } catch (InterruptedException e1) {
                    e1.printStackTrace();
                }
            }
        }
        if (i == 5)
            return;

        // loop infinito que consome a fila de mensagens de LOG
        while (true) {
            // lendo o próximo item da fila de LOG
            String mensagem = lerLinha();
            if (mensagem == null)
                break;

            // escrevendo a mensagem no arquivo
            try {
                FileWriter fw = new FileWriter(arquivo, true);
                try {
                    BufferedWriter bw = new BufferedWriter(fw);
                    try {
                        bw.write(mensagem);
                        bw.newLine();
                    } finally {
                        bw.close();
                    }
                } finally {
                    fw.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public String lerLinha() {
        while (true) {
            if (!prodcons_Log.queroConsumir(() -> fila.proximoDaFila().value != null)) {
                return null;
            }

            Slot slot = fila.proximoDaFila();
            if (slot.value != null) {
                String mensagem = slot.value;
                fila.tirarDaFila(slot);
                prodcons_Log.jaConsumi();
                return mensagem;
            }
            prodcons_Log.naoConsumi();
        }
    }

    public boolean finalizarEscrita(Slot slot, String msg) {
        if (msg == null)
            msg = "";

        try {
            slot.value = msg;
            prodcons_Log.jaProduzi();
            return true;
        } catch (Exception ex){
            // o chamador deve chamar desisteEscrita manualmente
            // ou então tentar chamar finalizarEscrita novamente
            return false;
        }
    }

    public void desisteEscrita(Slot slot) {
        fila.tirarDaFila(slot);
        prodcons_Log.naoProduzi();
    }

    public Slot iniciarEscrita() {
        if (prodcons_Log.queroProduzir()) {
            return fila.entrarNaFila();
        }
        return null;
    }
}
