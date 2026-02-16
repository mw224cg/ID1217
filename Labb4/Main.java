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
 */

public class Main {

    //Måste göra en main. init trådar + join etc etc
    //Måste fixa felhantering i prog, try/catch block etc
    
}

