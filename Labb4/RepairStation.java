

/**
 * Programmets monitor, synkroniserar reparatörens arbete och väntkön.
 * Har en counter för totalt antal fria platser.
 * Har en counter för varje fordonstyps fria platser.
 */

public class RepairStation {
    private int countA = 0;
    private int countB = 0;
    private int countC = 0;
    private int totalCount = 0;

    private final int maxA;
    private final int maxB;
    private final int maxC;
    private final int maxTotal;

    public RepairStation(int a, int b, int c, int v) { //constructor,  init max antal platser
        this.maxA = a;
        this.maxB = b;
        this.maxC = c;
        this.maxTotal = v;
    }

    /**
     * Fordonet vill repareras, kontrollerar om det finns plats både totalt och för fordonstypen. Om inte, vänta tills en plats blir ledig.
     * @param type fordonstypen som vill repareras (A, B, C)
     * @throws InterruptedException
     */
    public synchronized void requestRepair(VehicleType type) throws InterruptedException {
        while(!canRepair(type)){
            wait();
        }

        enterStation(type);
    }

    /**
     * Fordonet är färdigreparerat, lämnar stationen och signalerar att en plats har blivit ledig.
     * @param type fordonstypen som har reparerats (A, B, C)
     */
    public synchronized void releaseRepair(VehicleType type) {
        leaveStation(type);
        notifyAll();
    }

    /**
     * Kontrollera om det finns plats för fordonet att repareras, både totalt och för fordonstypen.
     * @param type fordonstypen som vill repareras (A, B, C)
     * @return true om det finns plats, false annars
     */
    private boolean canRepair(VehicleType type){
        if(totalCount >= maxTotal){ //kontroll för total kapacitet
            return false;
        }

        switch(type){ //kontroll för varje fordonstyp
            case A:
                return countA < maxA;
            case B:
                return countB < maxB;
            case C:
                return countC < maxC;
            default:
                return false;
        }



    }

    /**
     * Fordonet går in i stationen, uppdaterar räknare för totalt och fordonstypen.
     * @param type
     */
    private void enterStation(VehicleType type){
        totalCount++;
        switch(type){
            case A:
                countA++;
                break;
            case B:
                countB++;
                break;
            case C:
                countC++;
                break;
        }
    }

    /**
     * Fordonet lämnar stationen, uppdaterar räknare för totalt och fordonstypen.
     * @param type
     */
    private void leaveStation(VehicleType type) {
        totalCount--;
        switch(type){
            case A:
                countA--;
                break;
            case B:
                countB--;
                break;
            case C:
                countC--;
                break;
        }
    }

    
}
