#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <stdio.h>

#define TIME_STEP 64
#define MAX_SPEED 6.28

int main(int argc, char **argv) {
  wb_robot_init();

  // Initialisation des moteurs
  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");

  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);
  
  // Initialisation des capteurs de proximité
  WbDeviceTag prox_sensors[8];
  char prox_sensor_name[50];
  for (int ind = 0; ind < 8; ++ind) {
    sprintf(prox_sensor_name, "ps%d", ind);
    prox_sensors[ind] = wb_robot_get_device(prox_sensor_name);
    wb_distance_sensor_enable(prox_sensors[ind], TIME_STEP);
  }

  // Variables de vitesse
  double left_speed = MAX_SPEED;
  double right_speed = MAX_SPEED;
  
  // Boucle principale
  while (wb_robot_step(TIME_STEP) != -1) {
    // Lecture des capteurs (Note le 's' à prox_sensors)
    bool left_wall = wb_distance_sensor_get_value(prox_sensors[5]) > 80;
    bool left_corner = wb_distance_sensor_get_value(prox_sensors[6]) > 80;
    bool front_wall = wb_distance_sensor_get_value(prox_sensors[7]) > 80;

    // Logique de navigation (Suivi de mur gauche)
    if (front_wall) {
      left_speed = MAX_SPEED;
      right_speed = -MAX_SPEED; // Tourne à droite si obstacle devant
    } else {
      if (left_wall) {
        left_speed = MAX_SPEED;
        right_speed = MAX_SPEED; // Avance tout droit le long du mur
      } else {
        left_speed = MAX_SPEED / 8; // Cherche le mur en tournant à gauche
        right_speed = MAX_SPEED;
      }
      
      // --- Nouvelle portion : Éviter de trop coller au mur ---
        if (left_corner == true) {
           // Si le coin avant-gauche s'approche trop, on s'écarte vers la droite
           left_speed = MAX_SPEED;
           right_speed = MAX_SPEED / 8;
                                   }
    }

    // Application des vitesses aux moteurs
    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
  }

  wb_robot_cleanup();
  return 0;
}
