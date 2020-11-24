import numpy as np
import matplotlib.pyplot as plt
import os



def log_parser(path_log):
    output_path = 'experiment_output'
    if not os.path.exists(output_path):
        os.mkdir(output_path)

    file = open(path_log, "r")
    # reading the first "===============================\n"
    file.readline()
    # reading the data in the first header
    insp_valve_opening = float(file.readline().replace("Inspiratory Valve openning (%) : ", "").replace("\n", ""))
    exp_valve_opening = float(file.readline().replace("Expiratory Valve openning (%) : ", "").replace("\n", ""))
    blower_speed = float(file.readline().replace("Blower speed (%) : ", "").replace("\n", ""))
    # skiping units
    file.readline()

    file_contents = file.readlines()  # contains all the file as str
    N = len(file_contents)
    output = []
    index_offset = 0  # offset due to lines lost with header
    time_offset = 0

    for i in range(N):
        if i + index_offset < N:
            data = file_contents[i + index_offset]
        else:
            break
        if data == "===============================\n":  # new header detected

            insp_valve_opening = float(file_contents[i + index_offset + 1]
                                       .replace("Inspiratory Valve openning (%) : ", "").replace("\n", ""))

            exp_valve_opening = float(file_contents[i + index_offset + 2]
                                      .replace("Expiratory Valve openning (%) : ", "").replace("\n", ""))

            blower_speed = float(file_contents[i + index_offset + 3]
                                 .replace("Blower speed (%) : ", "").replace("\n", ""))

            # in the input file, time is reseted at each new header. In output file, is is continuous (no reset)
            time_offset = output[i - 1][0]
            line = [float(num) for num in file_contents[i + index_offset + 5].split(",")]
            output.append([line[0] + time_offset, line[1], line[2],
                           insp_valve_opening, exp_valve_opening, blower_speed])

            index_offset += 4


        else:
            line = [float(num) for num in data.split(",")]  # converting the element of the str list into float list

            output.append([line[0] + time_offset, line[1], line[2],
                           insp_valve_opening, exp_valve_opening, blower_speed])

    np.savetxt(output_path+"/test1.csv", output, delimiter=";")
    file.close()


log_parser("test1.log")
