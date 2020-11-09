import numpy as np
import matplotlib.pyplot as plt



def log_parser(path_log):
    file = open(path_log, "r")
    file.readline()
    insp_valve_opening = float(file.readline().replace("Inspiratory Valve openning (%) : ", "").replace("\n", ""))
    exp_valve_opening = float(file.readline().replace("Expiratory Valve openning (%) : ", "").replace("\n", ""))
    blower_speed = float(file.readline().replace("Blower speed (%) : ", "").replace("\n", ""))
    file.readline()
    time, pressure, flow = [], [], []
    nb_exp = 1

    while(1):
        data = file.readline()
        if data == "===============================\n":
            np.save("output/exp%d_iv%.1f_ev%.1f_bs%.1f"
                    %(nb_exp, insp_valve_opening, exp_valve_opening, blower_speed),
                    np.array([time, pressure, flow]).T)

            insp_valve_opening = float(file.readline().replace("Inspiratory Valve openning (%) : ", "").replace("\n", ""))
            exp_valve_opening = float(file.readline().replace("Expiratory Valve openning (%) : ", "").replace("\n", ""))
            blower_speed = float(file.readline().replace("Blower speed (%) : ", "").replace("\n", ""))
            file.readline()
            time, pressure, flow = [], [], []

        elif data == "":
            np.save("output/exp%d_iv%.1f_ev%.1f_bs%.1f"
                    % (nb_exp, insp_valve_opening, exp_valve_opening, blower_speed),
                    np.array([time, pressure, flow]).T)
            break
        else:
            line = [float(num) for num in data.split(",")]
            print(line)

            time.append(line[0])
            pressure.append(line[1])
            flow.append(line[2])



log_parser("test1.log")