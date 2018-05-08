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

            // wait for prisoner to be in the cell
            // ATTENTION: if warden doesn't do this, he could ask a prisoner that is not running
            // the live() method yet to enter the room, and the request will be lost
            // forever.
            p.waitUntilInCell();

            // update counts
            if (this.eachCount[id] == 0)
                this.count++;
            this.eachCount[id]++;

            // let the prisoner enter the room
            // then wait for him to leave
            System.out.println("Warden: orders inmate " + id + " to enter room.");
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
        // there is only one warden, I could have used notify()
        // ATTENTION: here it is safe to use someone else's state
        //      as shared memory because that object (the room)
        //      is not alive, and has no synchronized areas.
        //      When object has synchronized areas, it should
        //      take care of it's own state, by exposing notifyEventX()
        //      when it changes external shared data,
        //      and waitEventX() methods, so that others can wait
        //      on a private event of that object.
        //      In this case, the warden has this notifyPrisonerLeavingRoom(),
        //      to be notified of and external object state change.
        //      Be careful when doing this!!! Any synchronization over
        //      the shared object can cause havoc to the concurrency of
        //      the program such as dead-locks. These kind of problem are
        //      very difficult to find.
        this.room.closeDoor();
        this.notifyAll();
    }

    public synchronized void notifyPrisonerInCell() {
        // there is only one warden, I could have used notify()
        this.notifyAll();
    }
}
