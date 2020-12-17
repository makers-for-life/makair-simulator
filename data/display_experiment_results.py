import numpy as np
import matplotlib.pyplot as plt
import argparse


list_label = ["pressure(mmH2O)", "flow(ml.s-1)", "opening_insp_valve(%)", "opening_expe_valve(%)", "blower(%)"]


def display_experience(experience_path):
    data = np.loadtxt(experience_path, delimiter=";")
    plt.subplot(3,1,1)
    for i in range(2,5):
        plt.plot(data[:, 0], data[:, i+1] , label=list_label[i])
    plt.grid()
    plt.legend()

    plt.subplot(3, 1, 2)
    plt.plot(data[:, 0], data[:, 1])
    plt.title(list_label[0])
    plt.grid()

    plt.subplot(3, 1, 3)
    plt.plot(data[:, 0], data[:, 2])
    plt.title(list_label[1])
    plt.xlabel("time(s)")
    plt.grid()
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("csv_file_path", help= "path of the log file to be displayed")
    args = parser.parse_args()
    display_experience(args.csv_file_path)

# display_experience("experiment_output/test1.csv")
