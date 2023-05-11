# %%
import numpy as np
import cmath as cmath
import matplotlib.pyplot as plt
from pathlib import Path
import pandas as pd

from IPython.display import display

# %%
# # * defining paths

project_directory_path = Path(".")

data_rel_path = "Data/Output_teensy"

# %%
# * import data
# data needs to be printed in csv format to serial and then saved to csv (with putty, Pyserial or sth else)
try:
    sample_to_plot_fast = pd.read_csv(
        project_directory_path / data_rel_path / "test.csv", index_col=0
    )
except FileNotFoundError:
    print("There is no test.csv file")

# as a reference
properly_working = pd.read_csv(
    project_directory_path / data_rel_path / "A0_A1_B0_to_voltage_divider.csv",
    index_col=0,
)

faulty_hradware_connections = pd.read_csv(
    project_directory_path / data_rel_path / "faulty_hardware_connections_03052023.csv",
    index_col=0,
)

# display(sample_to_plot_fast.head())

# %%
# * getting size parameters

try:
    sample_to_plot_fast
except NameError:
    exit()

nb_channels = sample_to_plot_fast.shape[1] - 1
nb_samples = sample_to_plot_fast.shape[0]

print(nb_channels)
print(nb_samples)

# %%
# mean of time stamps, when difference
print(sample_to_plot_fast["Time"].mean() * 1 / 600000000)

# %%
# * calculating sample frequency
delta_t_arr = np.zeros(sample_to_plot_fast["Time"].size - 1)
nb_missing_data = 0

for i in range(sample_to_plot_fast["Time"].size - 1):
    try:
        delta_t_arr[i] = (
            1
            / (sample_to_plot_fast["Time"][i + 1] - sample_to_plot_fast["Time"][i])
            * 1000000
        )
    except KeyError:
        nb_missing_data += 1  # no need because mean takes care of size

print(delta_t_arr.mean())
# %%
# when it is clockcycles
delta_t_arr = np.zeros(sample_to_plot_fast["Time"].size - 1)
nb_missing_data = 0

for i in range(sample_to_plot_fast["Time"].size - 1):
    try:
        delta_t_arr[i] = 1 / (
            (sample_to_plot_fast["Time"][i + 1] - sample_to_plot_fast["Time"][i])
            * (1 / 600000000)
        )
    except KeyError:
        nb_missing_data += 1  # no need because mean takes care of size

print(delta_t_arr.mean())
# %%
# * plotting test data ----------------
fig = plt.figure()
ax = plt.subplot(111)
si = 50
ei = nb_samples
ei = nb_samples - 100
ei = 500

nb_channels = 5
for i in range(nb_channels):
    plt.plot(
        sample_to_plot_fast["Time"][si:ei],
        sample_to_plot_fast[sample_to_plot_fast.columns[i + 1]][si:ei]
        * 4  # 4 times
        * 2.5  # VREF
        / 32767,  # max representable value 16bit, to get volts
        linewidth=1,
        alpha=0.7,
        label=sample_to_plot_fast.columns[i + 1],
    )

plt.xlabel("Time [us]")
plt.ylabel("Voltage")

# box = ax.get_position()
# ax.set_position([box.x0, box.y0 + box.height * 0.1, box.width, box.height * 0.9])

# Put a legend below current axis
ax.legend(loc="lower right", fancybox=True, shadow=True, ncol=1)

# plt.legend()
plt.minorticks_on()
plt.grid(True, "both")
plt.tight_layout()
plt.show()

for i in range(sample_to_plot_fast["A0"].size - 1):
    if sample_to_plot_fast["A0"][i] >= 15000:
        print(
            "Time:"
            + str(sample_to_plot_fast["Time"][i])
            + ", Index: "
            + str(i)
            + ", Value: "
            + str(sample_to_plot_fast["A0"][i])
            + " in buffer:"
            + str(i // 1024)
            + str(" pos:")
            + str(i % 1024)
        )

# %%
# *** plotting the reference sampling
# ! if no test.csv, only run this cell (and change the if statement)
if 0:
    nb_chan_prop = properly_working.shape[1] - 1
    nb_samples_prop = properly_working.shape[0]
    delta_t_prop = np.zeros(properly_working["Time"].size - 1)

    nb_chan_fault = faulty_hradware_connections.shape[1] - 1
    nb_samples_fault = faulty_hradware_connections.shape[0]
    delta_t_fault = np.zeros(faulty_hradware_connections["Time"].size - 1)

    fig = plt.figure()
    ax = plt.subplot(211)
    si = 0
    ei = nb_samples_prop - 100

    nb_channels = 5
    for i in range(nb_channels):
        plt.plot(
            properly_working["Time"][si:ei],
            properly_working[properly_working.columns[i + 1]][si:ei] * 4 * 2.5 / 32767,
            linewidth=1,
            alpha=0.7,
            label=properly_working.columns[i + 1],
        )
    ax.legend(loc="lower right", fancybox=True, shadow=True, ncol=1)

    ax = plt.subplot(212)
    si = 0
    ei = nb_samples_fault - 100

    for i in range(nb_channels):
        plt.plot(
            faulty_hradware_connections["Time"][si:ei],
            faulty_hradware_connections[faulty_hradware_connections.columns[i + 1]][
                si:ei
            ],
            linewidth=1,
            alpha=0.7,
            label=faulty_hradware_connections.columns[i + 1],
        )

    plt.xlabel("Time [us]")
    plt.ylabel("Voltage")

    # Put a legend below current axis
    ax.legend(loc="lower right", fancybox=True, shadow=True, ncol=1)

    # plt.legend()
    plt.minorticks_on()
    plt.grid(True, "both")
    plt.tight_layout()
    plt.show()

# %%
