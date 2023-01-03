public class Prisoner extends Thread {
    int id;
    Room room;
    Warden warden;
    int state = 0;
    int count = 0;

    public static final int
            IN_CELL    = 1,
            IN_ROOM    = 2,
            FREE       = 3,
            DEAD       = 4;

    public Prisoner(int id, Room room, Warden warden){
        this.id = id;
        this.room = room;
        this.warden = warden;
    }

    public void run() {
        try {
            this.live();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    synchronized void live() throws InterruptedException {
        while (true) {
            // go back to the cell
            // notify warden that inmate is in cell
            // stay there until asked to go to the room
            //      or until set free
            // go to the room when asked
            // do something in the room
            // get out of the room

            this.state = IN_CELL;
            System.out.println("Prisoner: "+this.id+" in cell");

            // this.warden.notifyPrisonerInCell() // this caused dead lock...
            //              one object shouldn't wait for state of another object
            //              that's why we need to use waitUntilInCell() instead
            // indicate that state changed... allowing waitUntilInCell() to proceed
            this.notifyAll();

            System.out.println("Prisoner: "+this.id+" wait in cell");
            while (this.state == IN_CELL)
                this.wait(); // waitUntilInCell() will proceed on this line of code

            // I got out of the cell...
            // am I free or dead? or should I go to the room?
            if (this.state != IN_ROOM)
                break;

            // I am in the room
            // do my custom action
            this.roomAction();

            // tell warden that I'm leaving the room
            this.warden.notifyPrisonerLeavingRoom();
        }

        if (this.state == FREE)
            // I'm free... I'm free!!!
            System.out.println("Prisoner " + this.id + " is free!");
        else
            // DEAD
            System.out.println("Prisoner " + this.id + " is dead!");
    }

    protected void roomAction() {
        // If I didn't switch the trigger on yet, then do it
        if (this.count < 1 && !this.room.isTriggerSet())
        {
            System.out.println("Prisoner: " + this.id + " turns trigger on!");
            this.count++;
            this.room.setTrigger();
        }
        else {
            System.out.println("Prisoner: " + this.id + " in room... nothing to do!");
        }
    }

    public synchronized void free() {
        this.state = FREE;
        this.notifyAll();
    }

    public synchronized void kill() {
        this.state = DEAD;
        this.notifyAll();
    }

    public synchronized void gotoRoom() {
        System.out.println("Prisoner: " + this.id + " going to room.");
        this.state = IN_ROOM;
        this.notifyAll();
    }

    public synchronized void waitUntilInCell() {
        try {

            while (this.state != Prisoner.IN_CELL)
                this.wait();

        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
