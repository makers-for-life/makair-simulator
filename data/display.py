import numpy as np
import matplotlib.pyplot as plt


list_label = ["pressure", "flow", "insp_valve", "expe_valve", "blower"]


def display_experience(experience_path):
    data = np.load(experience_path)
    for i in range(5):
        plt.plot(data[:, 0], data[:, i+1] / np.max(np.abs(data[:, i+1])), label=list_label[i])
    plt.grid()
    plt.legend()
    plt.show()


display_experience("output/test1.npy")