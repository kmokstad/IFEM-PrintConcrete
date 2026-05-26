from splipy.io import G2
import splipy.curve_factory as cf
import splipy.surface_factory as sf
import numpy as np

def helix1(t):
    return np.vstack([(t+1)*np.cos(t*2*np.pi), (t+1)*np.sin(t*2*np.pi)]).T
def helix2(t):
    return np.vstack([(t+2)*np.cos(t*2*np.pi), (t+2)*np.sin(t*2*np.pi)]).T

c1 = cf.fit(helix1, 0, 2)
c2 = cf.fit(helix2, 0, 2)
s1 = sf.edge_curves(c1, c2)

with G2('out.g2') as myfile:
    myfile.write(s1.rebuild(p=2, n=(21,2)))
