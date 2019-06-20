import numpy as np

class Cramb():
    
    def __init__(self, amdc):
        
        self.amdc = amdc
        
    def connect(self):
        self.amdc.connect()
        
    def disconnect(self):
        self.amdc.disconnect()
        
    def cc_on(self):
        self.amdc.cmd('cramb init_cc')
        
    def cc_off(self):
        self.amdc.cmd('cramb deinit_cc')
        
    def currents(self, I1, I2):
        
        #I1, I2 are phase currents in amps
        
        I1, I2 = int(I1*1e6), int(I2*1e6) #convert to microamps
        
        self.amdc.cmd(f'cramb set_currents {I1} {I2}')
        
    def pole_volts(self, Va, Vb, Vc):
        
        #Va, Vb, Vc are the pole voltages measured w.r.t. negative DC bus
        
        Va, Vb, Vc = int(Va*1e6), int(Vb*1e6), int(Vc*1e6) #convert to microvolts
        
        self.amdc.cmd(f'cramb pole_volts {Va} {Vb} {Vc}')
        
    def phase_volts(self, Va, Vb, Vc):
        
        #Va, Vb, Vc are the phase voltages measured w.r.t. negative Vdc/2 (neutral)
        
        Va, Vb, Vc = int(Va*1e6), int(Vb*1e6), int(Vc*1e6) #convert to microvolts
        
        self.amdc.cmd(f'cramb phase_volts {Va} {Vb} {Vc}')
        
    def zero(self):
        
        self.cc_off()
        self.pole_volts(0,0,0)
        
    def read_adc(self):
        
        out = self.amdc.cmd('cramb read_adc')
        adc_voltage = [float(out[k]) for k in range(1,4)] #extract list of adc-voltages
        return adc_voltage
        
    def read_currents(self):
        
        out = self.amdc.cmd('cramb read_currents')
        currents = [float(out[k]) for k in range(1,4)] #extract list of adc-voltages
        return currents
        
        
        
        