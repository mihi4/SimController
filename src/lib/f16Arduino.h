#pragma once

// bits for airplane power data and sim status
#define BUSBAT					1    // true if at least the battery bus is powered
#define BUSEMER					2     // true if at least the emergency bus is powered
#define BUSESSENTIAL	 		4    // true if at least the essential bus is powered
#define BUSNONESSENTIAL			8    // true if at least the non-essential bus is powered
#define MAINGENON               16
#define STBYGENON               32
#define PLANEFLYING	            64   // true, if player is attached to plane, FLYING in enum FlyStates


   
