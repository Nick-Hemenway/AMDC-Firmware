import numpy as np
from AMDC import AMDC

class CRAMB():
    
    def __init__(self, amdc, debug = False):
        
        self.amdc = amdc
        self.debug = debug
        
        self.connect()
        
        
    def connect(self):
        self.amdc.connect()
        
    def disconnect(self):
        self.amdc.disconnect()
  
################  THREE PHASE RADIAL COMMANDS  ################     
    def cc_on(self):
        out = self.amdc.cmd('cramb init_cc')
        if self.debug:
            return out
        
    def cc_off(self):
        out = self.amdc.cmd('cramb deinit_cc')
        if self.debug:
            return out
        
    def currents(self, I1, I2):
        
        #I1, I2 are phase currents in amps
        
        I1, I2 = int(I1*1e6), int(I2*1e6) #convert to microamps
        
        out = self.amdc.cmd(f'cramb set_currents {I1} {I2}')
        if self.debug:
            return out
        
    def pole_volts(self, Va, Vb, Vc):
        
        #Va, Vb, Vc are the pole voltages measured w.r.t. negative DC bus
        
        Va, Vb, Vc = int(Va*1e6), int(Vb*1e6), int(Vc*1e6) #convert to microvolts
        
        out = self.amdc.cmd(f'cramb pole_volts {Va} {Vb} {Vc}')
        if self.debug:
            return out
        
    def phase_volts(self, Va, Vb, Vc):
        
        #Va, Vb, Vc are the phase voltages measured w.r.t. negative Vdc/2 (neutral)
        
        Va, Vb, Vc = int(Va*1e6), int(Vb*1e6), int(Vc*1e6) #convert to microvolts
        
        out = self.amdc.cmd(f'cramb phase_volts {Va} {Vb} {Vc}')
        if self.debug:
            return out
        
    def zero(self):
        
        self.currents(0,0)
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
    
    def set_vdc(self, Vdc):
        
        Vdc = int(Vdc*1000) #convert to milivolts
        out = self.amdc.cmd(f'cramb set_vdc {Vdc}')
        if self.debug:
            return out
    
    def set_cc_gains(self, R, L, fb):
        
        R = int(R*1000) #convert to miliohms
        L = int(L*1e6)  #convert to microhenry
        fb = int(fb)
        
        out = self.amdc.cmd(f'cramb set_cc_gains {R} {L} {fb}')
        if self.debug:
            return out
        
    def set_inverter(self, inverter_num):
        
        #note: the default inverter is inverter 6, which is set in C code
        
        self.zero()
        inverter_num = int(inverter_num)
        if inverter_num in [5,6]:
            inverter_num -= 1 #decrement the inverter number by one to match indices in C
            out = self.amdc.cmd(f'cramb set_inverter {inverter_num}')
            if self.debug:
                return out
            
    def pwm_reset(self):
        
        out = self.amdc.cmd('cramb pwm_reset')
        if self.debug:
            return out
        
            
         
################  SINGLE PHASE AXIAL COMMANDS  ################               
            
    def a_cc_on(self):
        out = self.amdc.cmd('cramb_axial init_cc')
        if self.debug:
            return out
        
    def a_cc_off(self):
        out = self.amdc.cmd('cramb_axial deinit_cc')
        if self.debug:
            return out
        
    def a_current(self, I):
        
        #I1, I2 are phase currents in amps
        
        I_command = int(I*1e6) #convert to microamps
        
        out = self.amdc.cmd(f'cramb_axial set_current {I_command}')
        if self.debug:
            return out
        
    def a_pole_volts(self, Va, Vb):
        
        #Va, Vb are the pole voltages measured w.r.t. negative DC bus
        
        Va, Vb = int(Va*1e6), int(Vb*1e6) #convert to microvolts
        
        out = self.amdc.cmd(f'cramb_axial pole_volts {Va} {Vb}')
        if self.debug:
            return out
        
    def a_voltage(self, V):
        
        #Va, Vb, Vc are the phase voltages measured w.r.t. negative Vdc/2 (neutral)
        
        V = int(V*1e6) #convert to microvolts
        
        out = self.amdc.cmd(f'cramb_axial set_volts {V}')
        if self.debug:
            return out
        
    def a_zero(self):
        
        self.a_current(0)
        self.a_cc_off()
        self.a_pole_volts(0,0)
        
    def a_read_adc(self):
        
        out = self.amdc.cmd('cramb_axial read_adc')
        adc_voltage = float(out[1]) #extract list of adc-voltages
        return adc_voltage #returns single float value
        
    def a_read_current(self):
        
        out = self.amdc.cmd('cramb_axial read_current')
        current = float(out[1]) #extract list of adc-voltages
        return current
    
    def a_set_vdc(self, Vdc):
        
        Vdc = int(Vdc*1000) #convert to milivolts
        out = self.amdc.cmd(f'cramb_axial set_vdc {Vdc}')
        if self.debug:
            return out
    
    def a_set_cc_gains(self, R, L, fb):
        
        R = int(R*1000) #convert to miliohms
        L = int(L*1e6)  #convert to microhenry
        fb = int(fb)
        
        out = self.amdc.cmd(f'cramb_axial set_cc_gains {R} {L} {fb}')
        if self.debug:
            return out
        
    def a_set_inverter(self, inverter_num):
        
        #note: the default inverter is inverter 4, which is set in C code
        
        self.a_zero()
        inverter_num = int(inverter_num)
        if inverter_num in [1,2,3,4]:
            inverter_num -= 1 #decrement the inverter number by one to match indices in C
            out = self.amdc.cmd(f'cramb_axial set_inverter {inverter_num}')
            if self.debug:
                return out

    
if __name__ == "__main__":
    
    try:
        cramb.disconnect() #disconnect any previous crambs that may still be active
    except:
        pass
        
    amdc = AMDC(port = 'COM6', cmdEcho = False)
    cramb = CRAMB(amdc, debug = True)
    
    cramb.set_vdc(10)  
    cramb.a_set_vdc(10)
    
    
    
    
    
        
        
        
        