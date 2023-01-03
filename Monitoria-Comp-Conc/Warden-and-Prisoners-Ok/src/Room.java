public class Room {
    public static final int
            TRIG_ON    = 1,
            TRIG_OFF   = 2;

    int trigger;
    boolean hasPrisoner = false;

    public void setTrigger() {
        this.trigger = TRIG_ON;
    }

    public void clearTrigger() {
        this.trigger = TRIG_OFF;
    }

    public boolean isTriggerSet() {
        return this.trigger == TRIG_ON;
    }

    public void closeDoor() {
        System.out.println("Room: door closed.");
        this.hasPrisoner = false;
    }

    public void openDoor() {
        System.out.println("Room: door opened.");
        this.hasPrisoner = true;
    }

    public boolean isDoorOpen() {
        return this.hasPrisoner;
    }
}
