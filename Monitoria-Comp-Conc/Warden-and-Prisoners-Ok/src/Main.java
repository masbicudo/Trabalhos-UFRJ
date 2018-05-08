public class Main {
    private static int numPrisoners = 20;

    public static void main(String[] args) throws Exception {
        if (args.length >= 1)
            numPrisoners = Integer.parseInt(args[0]);

        System.out.println(numPrisoners);

        Prisoner[] prisoners = new Prisoner[numPrisoners];
        Room room = new Room();
        Warden warden = new Warden(room, prisoners);
        prisoners[0] = new Leader(0, room, warden, prisoners.length);
        for (int it = 1; it < prisoners.length; it++)
            prisoners[it] = new Prisoner(it, room, warden);

        warden.start();
        for (int it = 0; it < prisoners.length; it++)
            prisoners[it].start();

        warden.join();
        for (int it = 0; it < prisoners.length; it++)
            prisoners[it].join();
    }
}
