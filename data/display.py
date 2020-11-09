import numpy as np
import matplotlib.pyplot as plt


def display_experience(experience_path):
    data = np.load(experience_path, allow_pickle=True)
    plt.plot(data[:,0], data[:,1], data[:,0], data[:,2])
    plt.show()


display_experience("output/exp1_iv20.0_ev10.0_bs30.0.npy")