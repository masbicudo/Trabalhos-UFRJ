public class Leader extends Prisoner {
    public Leader(int id, Room room, Warden warden, int prisoners) {
        super(id, room, warden);
        this.count = prisoners - 1;
    }

    protected void roomAction() {
        // I'm the leader... did someone turn the trigger on?
        // If so, decrement the number of remaining prisoners and turn if off.
        if (this.room.isTriggerSet())
        {
            this.count--;
            System.out.println("Leader: turns trigger off, and counts " + this.count + " prisoners remaining.");
            if (this.count == 0)
                this.warden.requestFreedom();
            this.room.clearTrigger();
        }
        else {
            System.out.println("Leader: in room... nothing to do!");
        }
    }
}
