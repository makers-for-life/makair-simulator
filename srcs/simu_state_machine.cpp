/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simu_state_machine.cpp
 * @brief Manage the state machine of the simulator.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/simu_state_machine.h"
#include <tgmath.h>


// FUNCTIONS ==================================================================

SimuStateMachine::SimuStateMachine()
  : m_running(true)
{}

void SimuStateMachine::init() {}

ActuatorsData SimuStateMachine::compute(SensorsData cmds, float dt, float time){

  static float T = 10; //period of respiration
  static float alpha = .5; // inspiration ratio (Ti/T)
  ActuatorsData output; 
  output.blower=30; // In this case, the blower in functionning at constant regime

  //Inspiration phase : the inspiration vavle is open, expiration valve is closed
  if(fmod(time, T) <= alpha * T){
      output.expirationValve = 0;
      output.inspirationValve = 99.9;
  }
  
  //expiration phase : the expiration valve is open, inspiration valve is closed
  else{
      output.expirationValve = 99.9;
      output.inspirationValve = 0;
  }

  //after 5 periods, the simulation is stopped
  if(time >= 5.*T){
    m_running = false;
  }
  return(output);
}

bool SimuStateMachine::isRunning(){
    return m_running;
}

