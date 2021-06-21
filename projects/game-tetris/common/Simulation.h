#ifndef simulation_h
#define simulation_h

void sleep_ms(const unsigned int sleep);
unsigned int getSystemTime_ms();
unsigned int diffSystime(unsigned int t1, unsigned int t2);

class GenericSimulation {
private:
    unsigned int simulationInterval_ms,
        lastTimeSimulation_ms,
        lastDiffTimeSimulation_ms;//last time that simulation has made
    float simulationInterval_ms_float_inv;
    bool pausedSimulation;
    unsigned int counter;
    int changeInterval;//não acredito que o intervalo possa chegar a ser mais de metade da rep de int
protected:
    GenericSimulation(const unsigned int interval, const bool paused);
    virtual ~GenericSimulation();
    void setCounter(const unsigned int counter);
public:
    void makeSimulation(const unsigned int time_ms);
    unsigned int getSimulationInterval_ms()const;
    void setSimulationInterval_ms(const unsigned int time);
    unsigned int getLastTimeSimulation_ms()const;
    void setLastTimeSimulation_ms(const unsigned int time);
    bool getPausedSimulation()const;
    void setPausedSimulation(const bool v);
    /*
      Auxiliar para determinar a interpolação para ser utilizada na hora de renderizar
    */
    float lerpCalc(const unsigned int time_ms)const;

    /*
      counter é uma variável monotonicamente crescente
    */
    virtual void processSimulation(const unsigned int counter, const unsigned int time_ms) = 0;
};

#endif
