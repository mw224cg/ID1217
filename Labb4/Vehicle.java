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
                Thread.sleep(1000); //simulera restid till stationen

                station.requestRepair(type);

                Thread.sleep(1000); //simulera reparationstid

                station.releaseRepair(type);
            }
        } catch (Exception e) {}
    }


}
