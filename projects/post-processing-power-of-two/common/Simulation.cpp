#include "Simulation.h"

#include <time.h>

#ifdef WIN32
#else
#include <sys/time.h>
#endif
//#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <unistd.h>
#endif


//#include "../SysHelper/DebugPrint.h"

void sleep_ms(const unsigned int sleep) {
#ifdef WIN32
    Sleep(sleep);
#else
    usleep(sleep * 1000);
#endif
}

extern unsigned int time_acc_ms;
unsigned int getSystemTime_ms() {
    return time_acc_ms;

    /*

  #ifndef WIN32
  struct timeval time;
  gettimeofday(&time,NULL);
  unsigned int retorno = time.tv_sec*1000;
  retorno -= retorno%1000;
  return retorno + time.tv_usec/1000;//(float)time.tv_usec*fact;
  #else
  return timeGetTime();
  #endif
  */
}

unsigned int diffSystime(unsigned int t1, unsigned int t2) {
    if ((t1 < 0xffff && t2 >(0xffff0000)) ||
        (t2 < 0xffff && t1 >(0xffff0000))) {
        //DEBUG_PRINT("ROUND UP TIME!!!!!! The Clock has reseted!!!\n");
        t1 = (t1 + 0xfffff);
        t2 = (t2 + 0xfffff);
    }
    if (t1 > t2)
        return t1 - t2;
    else if (t2 > t1)
        return t2 - t1;
    else
        return 0;
}

GenericSimulation::GenericSimulation(const unsigned int interval, const bool paused) {
    /*
      #ifdef USING_DEBUG
        char str[1024];
        sprintf(str,"[GENERIC SIMULATION] interval=%d", interval);
        DEBUG_PRINT(str);
      #endif
      //*/
    simulationInterval_ms = interval;
    simulationInterval_ms_float_inv = 1.0f / (float)simulationInterval_ms;
    pausedSimulation = paused;
    counter = 0;
    setLastTimeSimulation_ms(getSystemTime_ms());
    changeInterval = -1;
}
GenericSimulation::~GenericSimulation() {};
void GenericSimulation::setCounter(const unsigned int counter) {
    this->counter = counter;
}

void GenericSimulation::makeSimulation(const unsigned int time_ms) {
    if (pausedSimulation) {
        lastTimeSimulation_ms = time_ms - lastDiffTimeSimulation_ms;
        return;
    }
    lastDiffTimeSimulation_ms = diffSystime(time_ms, lastTimeSimulation_ms);
    if (changeInterval > 0) {//change intervall!!
        float new_old = (float)changeInterval / (float)simulationInterval_ms;

        unsigned int newDiff = (unsigned int)((float)lastDiffTimeSimulation_ms*new_old + 0.5);
#ifdef USING_DEBUG
        float lerp = (float)lastDiffTimeSimulation_ms*simulationInterval_ms_float_inv;
        char str[1024];
        sprintf(str, "[GENERICSIMULATION]ChangeIntervall detected!!! (%u)", changeInterval); DEBUG_PRINT(str);
        sprintf(str, "                   lastDiff!!! (%u)", lastDiffTimeSimulation_ms); DEBUG_PRINT(str);
        sprintf(str, "                   lastLerp!!! (%f)", lerp); DEBUG_PRINT(str);
        sprintf(str, "                   new/old (fator) Interval!!! (%f)", new_old); DEBUG_PRINT(str);
        sprintf(str, "                   newDiff(lastDiff/fator)!!! (%u)", newDiff); DEBUG_PRINT(str);
#endif
        lastDiffTimeSimulation_ms = newDiff;
        lastTimeSimulation_ms = time_ms - newDiff;
        simulationInterval_ms = (unsigned int)changeInterval;
        simulationInterval_ms_float_inv = 1.0f / (float)simulationInterval_ms;
        changeInterval = -1;
    }
    while (lastDiffTimeSimulation_ms >= simulationInterval_ms) {
        //simulation tick
        /*
        #ifdef USING_DEBUG
          char str[1024];
          sprintf(str,"[GENERICSIMULATION]simulation(last %u, current %u, interval %u, lastDiffTimeSimulation_ms %u)",
                      lastTimeSimulation_ms, time_ms, simulationInterval_ms, lastDiffTimeSimulation_ms);
          DEBUG_PRINT(str);
        #endif
        */
        processSimulation(counter, time_ms);
        lastTimeSimulation_ms += simulationInterval_ms;
        counter++;
        lastDiffTimeSimulation_ms = diffSystime(time_ms, lastTimeSimulation_ms);
        if (changeInterval > 0) {//change intervall!!
            float new_old = (float)changeInterval / (float)simulationInterval_ms;

            unsigned int newDiff = (unsigned int)((float)lastDiffTimeSimulation_ms*new_old + 0.5);
#ifdef USING_DEBUG
            float lerp = (float)lastDiffTimeSimulation_ms*simulationInterval_ms_float_inv;
            char str[1024];
            sprintf(str, "[GENERICSIMULATION]ChangeIntervall detected!!! (%u)", changeInterval); DEBUG_PRINT(str);
            sprintf(str, "                   lastDiff!!! (%u)", lastDiffTimeSimulation_ms); DEBUG_PRINT(str);
            sprintf(str, "                   lastLerp!!! (%f)", lerp); DEBUG_PRINT(str);
            sprintf(str, "                   new/old (fator) Interval!!! (%f)", new_old); DEBUG_PRINT(str);
            sprintf(str, "                   newDiff(lastDiff/fator)!!! (%u)", newDiff); DEBUG_PRINT(str);
#endif
            lastDiffTimeSimulation_ms = newDiff;
            lastTimeSimulation_ms = time_ms - newDiff;
            simulationInterval_ms = (unsigned int)changeInterval;
            simulationInterval_ms_float_inv = 1.0f / (float)simulationInterval_ms;
            changeInterval = -1;
        }
        //*/
    }
}
#define abs(x) ((x)>=-0.0?(x):-(x))

unsigned int GenericSimulation::getSimulationInterval_ms()const { return simulationInterval_ms; }
void GenericSimulation::setSimulationInterval_ms(const unsigned int time) {
#ifdef USING_DEBUG
    char str[1024];
    sprintf(str, "[GENERICSIMULATION] Set interval(%u)", time);
    DEBUG_PRINT(str);
#endif
    if (time == simulationInterval_ms)
        changeInterval = -1;
    else if (changeInterval != (int)time)
        changeInterval = (int)time;

    /*
      double lerp = (double)time*simulationInterval_ms_float_inv;//newt/oldt

    //  float lerp = (float)lastDiffTimeSimulation_ms*simulationInterval_ms_float_inv;
      unsigned int aux = (unsigned int)(lerp*(double)lastDiffTimeSimulation_ms + 0.5);
      lastTimeSimulation_ms += lastDiffTimeSimulation_ms-aux;
      lastDiffTimeSimulation_ms = aux;


      double lrp = (1.0-(double)time/(double)simulationInterval_ms)*(double)lastDiffTimeSimulation_ms;
      int aux = (int)(absv(lrp)+0.5);
      lastTimeSimulation_ms += aux;

      simulationInterval_ms = time;
      simulationInterval_ms_float_inv = (float)(1.0/(float)simulationInterval_ms);
    //*/
}

unsigned int GenericSimulation::getLastTimeSimulation_ms()const { return lastTimeSimulation_ms; }
void GenericSimulation::setLastTimeSimulation_ms(const unsigned int time) { lastTimeSimulation_ms = time; }

bool GenericSimulation::getPausedSimulation()const { return pausedSimulation; }
void GenericSimulation::setPausedSimulation(const bool v) { pausedSimulation = v; }

/*
  Auxiliar para determinar a interpolação para ser utilizada na hora de renderizar
*/
float GenericSimulation::lerpCalc(const unsigned int time_ms)const {
    if (pausedSimulation)
        return (float)lastDiffTimeSimulation_ms*simulationInterval_ms_float_inv;
    unsigned int dff = diffSystime(time_ms, lastTimeSimulation_ms);
    if (dff > simulationInterval_ms) return 1.0;
    return (float)dff*simulationInterval_ms_float_inv;
}
