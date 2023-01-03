package Trabalho;

public enum EstadoAssento {
    Desocupado(0),
    OcupadoPorDesconhecido(1),
    OcupadoPorMim(2);

    private final int value;
    private EstadoAssento(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
