import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":
    data = np.loadtxt("simulation_output/log_simulator.csv", delimiter=';')
    plt.plot(data[:,0], data[:,2],"*")
    plt.show()