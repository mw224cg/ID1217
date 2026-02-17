/**
 * Varje fordon är en tråd som representerar ett fordon som behöver repareras.
 * Varje fordon har en typ (A, B, C) och ett antal gånger det ska repareras.
 * Varje fordon försöker repareras i stationen, väntar om det inte finns plats, och signalerar när det är klart.
 * Varje fordon simulerar restid och reparationstid med Thread.sleep().
 * 
 */

public class Vehicle extends Thread{
    private final RepairStation station;
    private final VehicleType type;
    private final int numRepairs; //antal gånger varje fordon (tråd) ska repareras (köras)
    
    public Vehicle(RepairStation station, VehicleType type, int numRepairs) {
        this.station = station;
        this.type = type;
        this.numRepairs = numRepairs;
    } 

    public void run(){
        try {
            for(int i = 0; i < numRepairs; i++){
                Thread.sleep(5000); //simulera restid till stationen

                station.requestRepair(type);

                Thread.sleep(5000); //simulera reparationstid

                station.releaseRepair(type);
            }
        } catch (Exception e) {}
    }


}
