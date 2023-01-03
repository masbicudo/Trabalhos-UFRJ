import java.util.Scanner;
import Trabalho.*;

public class Principal {

    public static void main(String[] args) throws Exception {
        // inicializacao de variaveis
        final int numUsuariosSimultaneos = 4;
        String logFileName = "log.txt";
        int numAssentos = 10;

        ProdutorConsumidor prodcons_Log = new ProdutorConsumidor(
                10,
                0);

        // inicializacao de assentos, logger  e vetor threads do sistema
        Logger logger = new Logger(
                logFileName,
                0,
                new FilaDeString(prodcons_Log.capacidade),
                prodcons_Log);

        Assentos assentos = new Assentos(numAssentos, logger);
        UsuarioThread[] usuarios = new UsuarioComum[numUsuariosSimultaneos];

        // criando threads
        for (int i = 0; i < numUsuariosSimultaneos; i++) {
            usuarios[i] = new UsuarioComum(i+1, assentos);
        }

        // iniciando todas as threads o mais rápido que der
        logger.start();
        for (int i = 0; i < numUsuariosSimultaneos; i++) {
            usuarios[i].start();
        }

        // Espera todas as threads (menos logger) acabarem
        for (int i = 0; i < numUsuariosSimultaneos; i++) {
            usuarios[i].join();
        }

        // finaliza logger apos fim de todas as threads
        prodcons_Log.cancelar();
        logger.join();
        System.out.println(" Fim do sistema de reserva, início da análize dos logs");

        //inicia um corretor
        corretor.Corretor corretor = new corretor.Corretor(numAssentos, logFileName);

        //corrige resultado do log
        if (corretor.corrige()) {
            System.out.println("sistema funcionando");
        } else
            System.out.println("Dados do sistema estão inconsistentes");
    }
}
