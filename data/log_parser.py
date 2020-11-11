import numpy as np
import matplotlib.pyplot as plt


def log_parser(path_log):
    file = open(path_log, "r")
    file.readline()
    insp_valve_opening = float(file.readline().replace("Inspiratory Valve openning (%) : ", "").replace("\n", ""))
    exp_valve_opening = float(file.readline().replace("Expiratory Valve openning (%) : ", "").replace("\n", ""))
    blower_speed = float(file.readline().replace("Blower speed (%) : ", "").replace("\n", ""))
    file.readline()

    file_contents = file.readlines()
    N = len(file_contents)
    output = []
    index_offset = 0
    time_offset = 0


    for i in range(N):
        if i+index_offset<N:
            data = file_contents[i + index_offset]
        else:
            break
        if data == "===============================\n":

            insp_valve_opening = float(file_contents[i + index_offset +1]
                                       .replace("Inspiratory Valve openning (%) : ", "").replace("\n", ""))

            exp_valve_opening = float(file_contents[i + index_offset +2]
                                      .replace("Expiratory Valve openning (%) : ", "").replace("\n", ""))

            blower_speed = float(file_contents[i + index_offset +3]
                                 .replace("Blower speed (%) : ", "").replace("\n", ""))

            time_offset = output[i - 1][0]
            line = [float(num) for num in file_contents[i + index_offset +5].split(",")]
            output.append([line[0] + time_offset, line[1], line[2],
                           insp_valve_opening, exp_valve_opening, blower_speed])

            index_offset += 4


        else:
            line = [float(num) for num in data.split(",")]

            # output[i, :] = np.array([line[0] + time_offset, line[1], line[2],
            #                          insp_valve_opening, exp_valve_opening, blower_speed])
            output.append([line[0] + time_offset, line[1], line[2],
                                     insp_valve_opening, exp_valve_opening, blower_speed])

    np.save("output/test1", output)
    file.close()


log_parser("test1.log")
