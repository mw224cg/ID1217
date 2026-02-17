
import java.util.ArrayList;

/**
 * En reparatör kan repararera fordon av typen A,B och C
 * Reparatören kan samtidigt reparera som mest:
 * - a st fordon av typen A
 * - b st fordon av typen B
 * - c st fordon av typen C
 * - totalt v st fordon samtidigt.
 * 
 * Fordonen representeras av trådar: Vehicle och klassen RepairStation är programmets monitor
 * som styr synkroniseringen och simulerar reparatören.
 * 
 * Monitorn har två publika metoder: 
 * - requestRepair()
 * - releaseRepair()
 * 
 * Fordonen anropar requestRepair() som kontrollerar om det finns plats:
 * - Finns ej => tråden väntar (wait())
 * - Finns => antal platser i RepairStation uppdateras
 * 
 * Tråden simulerar tiden det tar för rep via sleep()
 * 
 * Fordonet anropar releaseRepair():
 * => Antal platser i repairStation uppdateras
 * => NotifyAll() väcker alla väntande trådar, dessa kontrollerar i sin tur om det finns plats för dem att repareras.
 * 
 * Programmet kör simuleringen n antal gånger för varje fordon (numRepairs i Vehicle).
 */

public class Main {
    public static void main(String[] args) {
        int a = 2;
        int b = 2;
        int c = 2;
        int v = 3;
        int threads = 6;
        int numRepairs = 2;

        RepairStation repairStation = new RepairStation(a, b , c, v);
        ArrayList<Vehicle> vehicles = new ArrayList<>();

        for(int i = 0; i < threads/3; i++){
            vehicles.add(new Vehicle(repairStation, VehicleType.A, numRepairs));
            vehicles.add(new Vehicle(repairStation, VehicleType.B, numRepairs));
            vehicles.add(new Vehicle(repairStation, VehicleType.C, numRepairs));
        }

        System.out.printf("Threads: %d | a = %d b = %d, c = %d, v = %d, Number of repairs/vehicle: %d\n",
        threads, a, b, c, v, numRepairs);
        System.out.println("Starting threads\n");
        System.out.println("EVENT : TYPE|    THREAD    | OCCUPIED COUNT\n");


        for(Vehicle vehicle : vehicles){
            vehicle.start();
        }

        for(Vehicle vehicle : vehicles){
            try {
                vehicle.join();
            } catch (Exception e) {
            }
        }

        System.out.println("Program finished\n");
    }

    
}
