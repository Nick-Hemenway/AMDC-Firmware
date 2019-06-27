import numpy as np
from numpy.lib.scimath import sqrt
import scipy.optimize as opt
import matplotlib.pyplot as plt
#import Common_Plots as cp

import time

plt.close('all')
np.seterr(divide = 'raise')

class ThreePole_CRAMB():
    
    def __init__(self, C1 = 175.677, C2 = 48.016, N = 308, numeric_solve = True):
        
        self.C1 = C1
        self.C2 = C2
        self.N = N
        
        self.numeric_solve = numeric_solve #boolean (True by default): if True, the roots of the depressed
                                           #depressed quartic will be solved numerically instead of analytically
        
 
########################### Utility Methods #################################                    
    
    def space_vec(self, x):
        
        #takes in any three quantities and returns their space vector
        #note that the input is an array which is unpacked internally
        
        x1, x2, x3 = x
        a = (2*np.pi)/3 #global factor used in several functions
        A = np.exp(a*1j)
        sv = x1 + A*x2 + A**2*x3
        
        return sv
   
    def sv_to_Bvec( self, sv ):
        
        #this function converts a spacevector back to a cartesian vector
        #it is assumed that the cartesian vector contains no zero-sequence component        
        x = np.real(sv)
        y = np.imag(sv)
        B1 = (2/3)*x
        B2 = -(1/3)*x + sqrt(3)/3*y
        B3 = -(1/3)*x - sqrt(3)/3*y
        B = np.array([B1, B2, B3])

        return B

    def I_hat( self,  Ic ):
        
        #Ic is vector of control currents
        
        sv = self.space_vec(Ic)
        Ihat = (2/3)*np.abs(sv)
        theta = np.angle(sv)
        
        return theta, Ihat


        
########################### Solver Methods ####################################
    
    def poly_coef_y(self, Fx, Fy):
        
        #given the force components Fx and Fy, this function returns the roots of the
        #depressed quartic of the form:
        # y^4 + p*y^2 + q*y + r = 0
        
        p = (-27*self.C1**2 + 12*self.C2*Fx)/(4*self.C2**2)
        q = (9*self.C1*Fy)/(self.C2**2)
        r = -(9*Fy**2)/(4*self.C2**2)
        
        return p, q, r  
    
    def poly_coef_x(self, Fx, Fy):
        
        #given the force components Fx and Fy, this function returns the roots of the
        #depressed quartic of the form:
        # y^4 + p*y^2 + q*y + r = 0
        
        p = -(27*self.C1**2 + 12*self.C2*Fx)/(4*self.C2**2)
        q = (27*self.C1**3 + 36*self.C1*self.C2*Fx)/(4*self.C2**3)
        r = -(27*self.C1**2*Fx + 9*self.C2*Fy**2)/(4*self.C2**3)
        
        return p, q, r  
    
    def _m_calc( self, p, q, r ):
    
        #the bearing class solves the roots of the depressed quartic of the form:
        # y^4 + p*y^2 + q*y + r = 0 analytically by using Cardano's method. This function
        #returns the roots 'm' of the auxilliary cubic equation used in Cardano's method
        #NOTE: for Cardano's method to work, the root 'm' cannot be equal to zero
        d1 = -16*p**2 - 192*r
        d2 = 128*p**3 + 1728*q**2 - 4608*p*r
        term = d2 + sqrt( 4*d1**3 + d2**2 )
        try:
            c = np.cbrt(term)
        except:
            c = term**(1/3)    
            
        t1 = p/3
        t2 = ( d1  /  (12*2**(2/3)*c) )
        t3 = c/(24*2**(1/3))    
        m1 = -t1 - t2 + t3
        
        a = 1 + 1j*sqrt(3)
        b = 1 - 1j*sqrt(3)
        m2 = -t1 + a*t2/2 - b*t3/2
        m3 = -t1 + b*t2/2 - a*t3/2
        
        m = np.array([m1, m2, m3]) #all three possible roots for m
        
        if len(m.shape)<2: #if m is one dimensional array: true when p, q,and r are scalars
            index = np.argmax(np.abs(m))
            return m[index] #returning maximum magnitude m to ensure m != 0
        else:
            i = np.abs(m).argmax(axis = 0)
            m_out = np.choose(i, m) #selecting maximum magnitude m for all solutions at once
                                    #if p, q, r are arrays and not scalars
            return m_out

    def root_calc( self, p, q, r):
        
        #this function actually calculates the roots of the depressed quartic of the form:
        # y^4 + p*y^2 + q*y + r = 0
        #it can do so numerically or analtically using Cardano's method. If the attribute
        #'numeric_solve' is set to True, it will solve the roots numerically
        
        if self.numeric_solve:
            roots = np.roots([1,0,p,q,r])
        else:

            m = self._m_calc( p, q, r )
            
            t1 = sqrt(2*m)
            t2 = 2*p + 2*m
            t3 = sqrt(2/m)*q
            #calculate four roots
            r1 = (  t1 + sqrt(-(t2 + t3))  )/2
            r2 = (  t1 - sqrt(-(t2 + t3))  )/2
            r3 = ( -t1 + sqrt(-(t2 - t3))  )/2
            r4 = ( -t1 - sqrt(-(t2 - t3))  )/2
            roots = np.array([r1, r2, r3, r4])
            
        #find all valid roots (roots are only valid if they are not complex)
        # roots = roots[~np.iscomplex(roots)] #doesn't always for analytical solution
        valid = np.abs(np.imag(roots)) < 1e-8 #imaginary component must be very small
        roots = np.real(roots[valid])    
        
        return roots
    
    def inverse( self, alpha, mag ):
        
        #this function takes in a desired force vector direction and magnitude
        #and returns an array of the four spacevectors (complex numbers) that are
        #solutions to the inverse problem
        
        if mag == 0: #treat magnitude of zero as special case to avoid numerical issues when computing inverse
            sv = np.zeros(4) 
            
        else:
            alpha_r = np.deg2rad(alpha) #convert input angle to radians
            #calculate force components
            Fx = mag*np.cos(alpha_r)
            Fy = mag*np.sin(alpha_r)
            
            #checking edge cases to make sure we don't divide by zero (1/y). 
            #if we are within one degree of the positive or negative x-axis, 
            #calculate roots of quartic in x first, else, calculate roots of 
            #quartic in y first.
            # if np.all(np.abs([0-alpha, 180 - alpha, 360 - alpha]) > 1): #if alpha is further than 1 degree from horizontal
            try:
                p, q, r = self.poly_coef_y( Fx, Fy )
                y = self.root_calc( p, q, r ) #calculating roots of quartic in y
                x = 1.5*(self.C1/self.C2 - Fy/(self.C2*y))
            except:
                p, q, r = self.poly_coef_x( Fx, Fy )
                x = self.root_calc( p, q, r ) #calculating roots of quartic in x
                y = (3*Fy)/(3*self.C1 - 2*self.C2*x)
                
            sv = x + y*1j 
            
        return sv
    
########################### Output Methods ####################################
            
    def force(self, Ic):
        
        #takes in a control currents vector and returns the output force direction
        #and magnitude
        
        i = self.space_vec(Ic)
        i2 = self.space_vec(Ic**2)
        
        F = self.C2*i2 + self.C1*i
        
        mag = np.abs(F)
        alpha = np.rad2deg(np.angle(F))
        alpha = (alpha + 720)%360 
        
        return alpha, mag
    
    def control_currents(self, alpha, mag, ampturns = False):
        
        Ic_sv = self.inverse( alpha, mag )
        Ic = self.sv_to_Bvec(Ic_sv)
        norm = np.linalg.norm(Ic, axis = 0)
        idx = norm.argmin()
        Ic = Ic[:,idx]
        out = Ic
        
        if ampturns:
            out = Ic*self.N
            
        return out


def main():
    
    b = ThreePole_CRAMB()
    Ic = b.control_currents(30, 100, ampturns= True)
    print(Ic)

    Ic = np.array([1, -.5, -.5])
    print(b.force(Ic))
    
if __name__ == "__main__":  Ic = main()























