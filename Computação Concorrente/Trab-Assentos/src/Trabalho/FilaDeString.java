package Trabalho;

import java.util.ArrayList;

/**
 * Clase que representa uma fila de Strings.
 */
public class FilaDeString {
    private Slot[] slots;
    private int fimDaFila = 0;

    public FilaDeString(int capacidade) {
        slots = new Slot[capacidade];
        for (int i = 0; i < capacidade; i++)
            slots[i] = new Slot();
    }

    public synchronized Slot entrarNaFila() {
        Slot slot = slots[fimDaFila];
        fimDaFila++;
        return slot;
    }

    public synchronized void tirarDaFila(Slot slot) {
        int id;
        for (id = 0; id < slots.length; id++)
            if (slots[id] == slot)
                break;

        for (; id < slots.length-1; id++)
            slots[id] = slots[id+1];

        slots[id] = slot;
        slot.value = null;
        fimDaFila--;
    }

    public synchronized Slot proximoDaFila() {
        return slots[0];
    }

    public synchronized int length() {
        return fimDaFila;
    }
}
