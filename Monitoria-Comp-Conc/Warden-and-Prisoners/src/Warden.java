import java.util.Random;

public class Warden extends Thread {
    Room room;
    int count;
    int[] eachCount;
    Prisoner[] prisoners;
    boolean freedomRequested = false;

    public Warden(Room room, Prisoner[] prisoners){
        this.room = room;
        this.count = 0;
        this.prisoners = prisoners;
        this.eachCount = new int[prisoners.length];
    }

    public void run() {
        try {
            this.live();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    synchronized void live() throws InterruptedException {
        Random random = new Random();

        while (true) {
            // select random prisoner
            int id = random.nextInt(this.prisoners.length);
            Prisoner p = this.prisoners[id];

            // update counts
            if (this.eachCount[id] == 0)
                this.count++;
            this.eachCount[id]++;

            // let the prisoner enter the room
            // then wait for him to leave
            this.room.openDoor();
            p.gotoRoom();
            while (this.room.isDoorOpen())
                this.wait();

            // check whether I got an answer
            if (this.freedomRequested) {
                // They want to get free...
                // let's check whether they won the game
                for (int it = 0; it < this.prisoners.length; it++) {
                    if (this.count == this.prisoners.length)
                        this.prisoners[it].free();
                    else
                        this.prisoners[it].kill();
                }
                break;
            }
        }
    }

    public void requestFreedom() {
        this.freedomRequested = true;
    }

    public synchronized void notifyPrisonerLeavingRoom() {
        this.room.closeDoor();
        this.notifyAll();
    }
}
