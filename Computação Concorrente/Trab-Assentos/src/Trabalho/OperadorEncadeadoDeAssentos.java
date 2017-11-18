package Trabalho;

public abstract class OperadorEncadeadoDeAssentos {
    protected abstract void processa(int indice, Assento assento);

    public void executar(Assentos assentos) {
        Assento pendente = null;
        try {
            for (int i = 0; i < assentos.length(); i++) {
                Assento corrente = assentos.assentos[i];
                corrente.queroEscrever();
                try {
                    if (pendente != null)
                        pendente.acabeiDeEscrever();
                    pendente = corrente;
                    corrente = null;

                    this.processa(i, pendente);
                } finally {
                    if (corrente != null)
                        corrente.acabeiDeEscrever();
                }
            }
        } finally {
            if (pendente != null)
                pendente.acabeiDeEscrever();
        }
    }
}
