import numpy as np
from numpy.lib.scimath import sqrt
import scipy.optimize as opt
import matplotlib.pyplot as plt
#import Common_Plots as cp

import time

plt.close('all')
np.seterr(divide = 'raise')

class ThreePole():
    
    def __init__(self, Bmax = 1.16, zeta = (1 + 1/np.sqrt(2))/3, theta1 = 110, g = 0.001, R = 0.0401, L = 0.0165, N = 1000, numeric_solve = True):
        
        self.Bmax = Bmax   #maximum allowed field density
        self.zeta = zeta   #propotion maximum field that bias field is e.g. B0 = zeta*Bmax
        self.B0 = self.zeta*self.Bmax #bias field
        
        #geometric properties
        self.theta1 = np.deg2rad(theta1) #angular span of the radial teeth
        self.mu_0 = np.pi*4e-7 #magnetic permeability of free space
        self.g = g #airgap length (m)
        self.R = R #rotor radius (m)
        self.L = L #stator lamination length (m)
        self.N = N #number of turns on each radial pole

        self.A = self.R*self.theta1*self.L #Area in front of tooth (m^2)
        self.beta = 2*np.sin(self.theta1/2)/self.theta1 #tooth area correction factor
        self.k1 = self.beta*self.A/(2*self.mu_0) #geometric constant in force expression
        self.k2 = self.N*self.mu_0/self.g # Bc = k2*Ic
        
        self.hexagon = self.k1*self.Bmax**2 #magnitude of hexagon point that all forces must lie within
        
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

    def b_hat( self,  Bc ):
        
        sv = self.space_vec(Bc)
        bhat = (2/3)*np.abs(sv)
        theta = np.angle(sv)
        
        return theta, bhat
    
    def _residual(self, mag, alpha):
        
        #_residual is the only function with the arguments in the order mag, alpha and it
        #must be this way for brentq function to work
    
        #we must return the minimum field of all solutions to give a correct force output
        B_total = self.total_fields(alpha, mag)
        B_mag = np.abs(B_total)
        max_per_solution = B_mag.max(axis = 0)
        min_solution = max_per_solution.min()
            
        return  min_solution - self.Bmax
    
    def add_curve_fit(self, C2, C1):
        
        self.k1 = C1**2/(4*self.B0**2*C2)
        self.k2 = 2*self.B0*C2/C1
        
    
    def saturated(self, Bc):
        
        B = Bc + self.B0 #total field
        Bmag = np.abs(B)
        check = np.any( Bmag > self.Bmax, axis = 0 )
        
        return check
    
    def __repr__(self):
        
        part1 = f'ThreePole( Bmax = {self.Bmax}, zeta = {self.zeta:.6}, '
        part2 = f'theta1 = {np.rad2deg(self.theta1):.4f}, g = {self.g}, R = {self.R}, '
        part3 = f'L = {self.L}, N = {self.N}, numeric_solve = {self.numeric_solve})'
        
        return part1 + part2 + part3

    def __str__(self):
        
        offset = ' '*5
        heading = '\n' + offset + 'Property:'.ljust(30) + offset + 'Value:'.ljust(10) + 'Units:'.center(6)
        
        prop_label = ['Bmax', '\u03B6', '\u03B8\u2081', 'Rotor Radius', 'Air Gap', 'Stator Lamination Length', 'Number of Turns']
        prop_value = [self.Bmax, np.round(self.zeta,6), np.round(np.rad2deg(self.theta1),6), self.R, self.g, self.L, self.N]
        units = [ 'T', '-', '\u00B0', 'm', 'm', 'm', '-']
        
        
        lines = ''
        for label, value, unit in zip(prop_label, prop_value, units):
            lines += '\n' + offset + f'{label}   '.ljust(30,'.') + offset + f'{value}'.ljust(10) + f'{unit}'.center(6)
            
        return heading + lines + '\n'
    
    def info(self):
        
        print(self.__str__())
        
########################### Solver Methods ####################################
    
    def poly_coef_y(self, Fx, Fy):
        
        #given the force components Fx and Fy, this function returns the roots of the
        #depressed quartic of the form:
        # y^4 + p*y^2 + q*y + r = 0
        
        p = 3*(Fx/self.k1 - 9*(self.B0)**2)
        q = 18*(self.B0)*(Fy/self.k1)
        r = -(9/4)*(Fy/self.k1)**2
        
        return p, q, r  
    
    def poly_coef_x(self, Fx, Fy):
        
        #given the force components Fx and Fy, this function returns the roots of the
        #depressed quartic of the form:
        # y^4 + p*y^2 + q*y + r = 0
        
        p = -3*(Fx/self.k1 + 9*self.B0**2)
        q = 18*self.B0*(3*self.B0**2 + Fx/self.k1)
        r = -(27*self.B0**2*(Fx/self.k1) + (9/4)*(Fy/self.k1)**2)
        
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
                x = 1.5*(2*self.B0 - (Fy/self.k1)*(1/y))
            except:
                p, q, r = self.poly_coef_x( Fx, Fy )
                x = self.root_calc( p, q, r ) #calculating roots of quartic in x
                y = (Fy/self.k1)/( 2*self.B0 - (2/3)*x )
                
            sv = x + y*1j 
        return sv
    
########################### Output Methods ####################################
            
    def force(self, Bc):
        
        #takes in a control field vector and returns the output force direction
        #and magnitude
        
        b = self.space_vec(Bc)
        b2 = self.space_vec(Bc**2)
        
        F = self.k1*(b2 + 2*self.B0*b)
        
        mag = np.abs(F)
        alpha = np.rad2deg(np.angle(F))
        alpha = (alpha + 720)%360 
        
        return alpha, mag
    
    def rated_force(self):
    
        opt_result = opt.minimize_scalar(self.max_force, bounds = (0,60), method = 'bounded')
        alpha = opt_result.x
        fr = self.max_force(alpha)
            
        return fr
    
    def max_force(self, alpha):
        
        #takes input force vector angle and returns the maximum achievable force
        #magnitude in that direction
        
        try:
            mag = opt.brentq(self._residual, 0.1, 1.1*self.hexagon, args = (alpha,))
        except: 
            mag = opt.newton(self._residual, x0 = 1, args = (alpha,), maxiter = 100)
                  
        return mag
    
    
    def full_output(self, alpha, mag):
            
        sv = self.inverse( alpha, mag )
        
        N = 25
        out = ['\nInput:']
        inputline = 'Magnitude = {},  \u03B1 = {}'.format(mag, alpha).center(30)
        out.append(inputline)
        out.append('\nOutput:')
        title = 'B-Space-Vector:'.center(N) + 'B-Total:'.center(N) + 'Magnitude:'.center(N) + '\u03B1:'.ljust(N)
        out.append(title)
        
        for b in sv:
            x = np.real(b)
            y = np.imag(b)
            Bc = self.sv_to_Bvec(b)
            B = Bc + self.B0
            sv_string = '{} + {}j'.format(np.round(x,3), np.round(y,3))
            alpha, mag = self.force( Bc )
            line = sv_string.center(N) + '{}'.format(B.round(3)).center(N) + \
            '{}'.format(np.round(mag,1)).center(N) + '{}'.format(np.round(alpha,1)).ljust(N)
            out.append(line)
        out.append('\n\n')
            
        msg = '\n'.join(out)
        print(msg)
        
    def control_fields(self, alpha, mag, check_saturation = False):
        
        sv = self.inverse( alpha, mag )
        Bc = self.sv_to_Bvec(sv)
        
        if check_saturation:
            not_saturated = ~self.saturated(Bc)
            Bc = Bc[:, not_saturated]
        
        return Bc
    
    def total_fields(self, alpha, mag, check_saturation = False):
        
        B_total = self.control_fields(alpha, mag, check_saturation) + self.B0
        
        return B_total
    
    def control_currents(self, alpha, mag, ampturns = False, check_saturation = True):
        
        Bc = self.control_fields(alpha, mag, check_saturation = check_saturation)
        Ic = Bc/self.k2
        norm = np.linalg.norm(Ic, axis = 0)
        idx = norm.argmin()
        Ic = Ic[:,idx]
        out = Ic
        
        if ampturns:
            out = Ic*self.N
            
        return out
        
    def __call__(self, alpha, mag, check_saturation = False ):

        Bc = self.control_fields(alpha, mag, check_saturation = check_saturation)
        
        return Bc


def main():
    
    theta1 = 75 #degrees
    B0_simulation = 0.70804 #Tesla (from Jmag simulation)
    turns = 308 #number of turns in each radial coil
    zeta = (1 + 1/np.sqrt(2))/3 #optimal bias field
    Bmax = B0_simulation/zeta #determine what Bmax needs to be to have optimal bias field
    
    b = ThreePole(theta1 = theta1, Bmax = Bmax, N = turns)
    Ic = b.control_currents(30, 100, ampturns=True)
    print(Ic)

    
if __name__ == "__main__":  Ic = main()























