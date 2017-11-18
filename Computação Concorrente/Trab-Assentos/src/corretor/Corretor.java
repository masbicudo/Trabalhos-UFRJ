package corretor;

import Trabalho.Assentos;
import Trabalho.FilaDeString;
import Trabalho.Logger;
import Trabalho.ProdutorConsumidor;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * classe corretora.
 */
public class Corretor {
    Assentos assentos;
    File arquivo;
    Logger log;

    public Corretor(int numAssentos, String name) {
        ProdutorConsumidor prodcons = new ProdutorConsumidor(1,0);
        log = new Logger(null, 0, new FilaDeString(prodcons.capacidade), prodcons);
        assentos = new Assentos(numAssentos, log);
        arquivo = new File(name);
    }

    /**
     * Corrige o programa de acordo com o log de maneira sequencial.
     * @return tree se log esta certo e false se esta errado
     *
     * @throws IOException
     */
    public boolean corrige() throws IOException {
        Pattern pattern = Pattern.compile("^([VAL]),\\s*(\\d+)(?:,\\s*(\\d+))?\\s*\\[(.*)]$");

        FileReader fr = new FileReader(arquivo);
        BufferedReader br = new BufferedReader(fr);
        try {
            //leitura do aquivo e filtro de dados
            while (br.ready()) {
                String linha = br.readLine();
                Matcher matcher = pattern.matcher(linha);
                matcher.find();

                String metodo = matcher.group(1);
                int usuarioId = Integer.parseInt(matcher.group(2));
                int assentoId = -1;
                try {
                    assentoId = Integer.parseInt(matcher.group(3));
                } catch (Exception ex) {
                }

                String linhaSimulada = null;

                //faz de maneira sequencial o metodo descrito no log e compara
                switch (metodo) {
                    case "V":
                        assentos.vizualizaAssentos(usuarioId);
                        break;
                    case "A":
                        assentos.alocaAssentoDado(assentoId, usuarioId);
                        break;
                    case "L":
                        assentos.liberaAssento(assentoId, usuarioId);
                        break;
                }

                linhaSimulada = log.lerLinha();

                if (!linhaSimulada.equals(linha)) {
                    return false;
                }
            }
            return true;
        } catch (IOException ex) {
            ex.printStackTrace();
            return false;
        }
        finally {
            br.close();
            fr.close();
        }
    }
}


