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

# Ligo_GW_raw_path = project_directory_path / raw_data_rel_path
# Ligo_GW_processed_path = project_directory_path / processed_data_rel_path

# %%
# * import data
sample_to_plot_fast = pd.read_csv(
    project_directory_path / data_rel_path / "test.csv", index_col=0
)
display(sample_to_plot_fast.head())
# %%
# * getting size parameters

nb_channels = sample_to_plot_fast.shape[1] - 1
nb_samples = sample_to_plot_fast.shape[0]

print(nb_channels)
print(nb_samples)

# %%
# * calculating sample frequency
delta_t_arr = np.zeros(sample_to_plot_fast["Time"].size - 1)

for i in range(sample_to_plot_fast["Time"].size - 1):
    delta_t_arr[i] = (
        1
        / (sample_to_plot_fast["Time"][i + 1] - sample_to_plot_fast["Time"][i])
        * 1000000
    )

print(delta_t_arr.mean())
# %%
fig = plt.figure()
ax = plt.subplot(111)
si = 50
ei = nb_samples
ei = nb_samples - 100
ei = 100

nb_channels = 2
for i in range(nb_channels):
    plt.plot(
        sample_to_plot_fast["Time"][si:ei],
        sample_to_plot_fast[sample_to_plot_fast.columns[i + 1]][si:ei],
        linewidth=1,
        alpha=0.7,
        label=sample_to_plot_fast.columns[i + 1],
    )

plt.xlabel("Time [us]")
plt.ylabel("Strain")

# box = ax.get_position()
# ax.set_position([box.x0, box.y0 + box.height * 0.1, box.width, box.height * 0.9])

# Put a legend below current axis
ax.legend(loc="lower right", fancybox=True, shadow=True, ncol=1)

# plt.legend()
plt.minorticks_on()
plt.grid(True, "both")
plt.tight_layout()
plt.show()

# %%
