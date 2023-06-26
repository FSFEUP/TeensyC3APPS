import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate


def import_new_raw_data(in_file, out_file):
    with open(in_file) as data_file:
        data = [line.strip().split("\t") for line in data_file.readlines()]
        data = [[int(line[0]), int(line[1]), float(line[2])] for line in data]
        data.sort(key=lambda x: x[-1], reverse=True)
        with open(out_file, "w") as output_file:
            for index in range(len(data)):
                if index >= 1 and ((data[index][0] > data[index - 1][0]) or (data[index][1] > data[index - 1][1])):
                    continue
                output_file.write("\t".join(str(x) for x in data[index]) + "\n")
        apps1_values = [line[0] for line in data]
        apps2_values = [line[1] for line in data]
        compression_values = [line[2] for line in data]
        # adjust for measuing offset
        compression_values = [val - min(compression_values) for val in compression_values]
        return apps1_values, apps2_values, compression_values


# define a 2d plot function that saves the plot to a png file and does not block the program
def plot_2d_data_nonblocking(x_data, y_data, x_label, y_label, title):
    plt.plot(x_data, y_data, "ro")
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.savefig("data_files/" + title + ".png")
    plt.clf()


# create a function that takes in the raw sensor data  and returns the interpolated data for each sensor
def interpolate_data(apps1_vals, apps2_vals, compr_vals):
    # create an interpolation function for each sensor
    apps1_interp = interpolate.interp1d(compr_vals, apps1_vals, kind="linear", fill_value="extrapolate")
    apps2_interp = interpolate.interp1d(compr_vals, apps2_vals, kind="linear", fill_value="extrapolate")
    # create a new array of compression values to interpolate the data to
    new_compr_vals = np.arange(0, max(compr_vals), 0.1)
    # create a new array of interpolated data for each sensor
    new_apps1_vals = apps1_interp(new_compr_vals)
    new_apps2_vals = apps2_interp(new_compr_vals)
    # correct interpolation results according to physical constraints
    for index in range(len(new_apps1_vals)):
        if new_apps1_vals[index] < 0:
            new_apps1_vals[index] = 0
        if new_apps1_vals[index] > 1023:
            new_apps1_vals[index] = 1023

    for index in range(len(new_apps2_vals)):
        if new_apps2_vals[index] < 0:
            new_apps2_vals[index] = 0
        if new_apps2_vals[index] > 1023:
            new_apps2_vals[index] = 1023
    # return the interpolated data
    return new_apps1_vals, new_apps2_vals, new_compr_vals


def __main__():
    apps1_vals, apps2_vals, compr_vals = import_new_raw_data("data_files/raw_data.txt", "data_files/new_data.txt")
    plot_2d_data_nonblocking(compr_vals, apps1_vals, "Sensor Compression (mm)", "APPS 1", "APPS 1 vs. Compression")
    plot_2d_data_nonblocking(compr_vals, apps2_vals, "Sensor Compression (mm)", "APPS 2", "APPS 2 vs. Compression")
    new_apps1_vals, new_apps2_vals, new_compr_vals = interpolate_data(apps1_vals, apps2_vals, compr_vals)
    plot_2d_data_nonblocking(
        new_compr_vals, new_apps1_vals, "Sensor Compression (mm)", "APPS 1", "APPS 1 vs. Compression (Interpolated)"
    )
    plot_2d_data_nonblocking(
        new_compr_vals, new_apps2_vals, "Sensor Compression (mm)", "APPS 2", "APPS 2 vs. Compression (Interpolated)"
    )
    with open("data_files/interpolation_data.txt", "w") as output_file:
        for index in range(len(new_compr_vals)):
            output_file.write(
                "\t".join(str(x) for x in [new_compr_vals[index], new_apps1_vals[index], new_apps2_vals[index]]) + "\n"
            )


if __name__ == "__main__":
    __main__()
